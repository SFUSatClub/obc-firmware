/*
 * sfu_fs_structure.c
 *
 *  Created on: Mar 13, 2018
 *      Author: Richard
 */

#include "sys_common.h"
#include "sfu_fs_structure.h"
#include "sfusat_spiffs.h"
#include "spiffs.h"
#include "sfu_uart.h"
#include "sfu_utils.h"
#include "sfu_rtc.h"
// -------------- Tasks for testing --------------------
void sfu_create_fs_test(void *pvParameters) {
	sfu_fs_init();
	sfu_create_files();
	while (1) {
		vTaskDelay(pdMS_TO_TICKS(5000));
		delete_oldest(); // this handles deletion and creation
	}
}

// ------------ Core Functions -------------------------
void sfu_fs_init() {
	// Todo: grab this from FEE or from config file
	fs_num_increments = 0;
}
/* increment_prefix
 * - increments the fs prefix once we've filled up the files for the day
 *
 *
 * Todo:
 * remove create from file writes, add error handler that will attempt to create files if they don't exist or something
 * delete oldest function (calls this), called by rescue
 * add a counter for number of rollovers we do - this will also be used to not delete files until after the first fillup.
 */
/* delete_oldest
 * - this function is run at the end of every day.
 * - it increments the file prefix and deletes any old files
 */
void delete_oldest() {
	// take the mutex since we don't want any writes while we're messing with this
	if (xSemaphoreTake(spiffsTopMutex, pdMS_TO_TICKS(SPIFFS_READ_TIMEOUT_MS)) == pdTRUE) {
		// increment prefix
		increment_prefix();

		// delete the files with the current prefix since we're replacing it
		if (fs_num_increments >= PREFIX_QUANTITY) {
			sfu_delete_prefix(*(char *) fs.user_data);
		}
		// create fresh files
		sfu_create_files();
		xSemaphoreGive(spiffsTopMutex);
	} else {
		serialSendQ("Del oldest can't get top mutex");
	}
}

void increment_prefix() {
	// CALL WITHIN MUTEX
	if (*(char *) fs.user_data == (PREFIX_START + PREFIX_QUANTITY - 1)) { // if we're at the end, roll back around
		sfu_prefix = PREFIX_START;
	} else { // else just increment
		sfu_prefix = sfu_prefix + 1; // increment to next prefix
	}
	fs_num_increments++;
}

/*sfu_delete_prefix
 * - This function deletes all files with the specified prefix
 * - Used to get rid of the oldest set of files when we loop back around
 */
void sfu_delete_prefix(const char prefix) {
	// MUST CALL WITHIN MUTEX

	spiffs_DIR d;
	struct spiffs_dirent e;
	struct spiffs_dirent *pe = &e;
	int32_t res;
	char genBuf[20] = { '\0' };
	spiffs_file fd = -1;
	spiffs_stat s;

	SPIFFS_opendir(&fs, "/", &d);
	while ((pe = SPIFFS_readdir(&d, pe))) {
		if ((const char *) prefix == (char *) pe->name[0]) { // we kinda don't have strncmp, but we only compare one char anyway
			// found one
			fd = SPIFFS_open_by_dirent(&fs, pe, SPIFFS_RDWR, 0);
			if (fd < 0) {
				snprintf(genBuf, 20, "Fdo: %i", SPIFFS_errno(&fs));
				serialSendQ(genBuf);
			}

// Get file stats
			if (SPIFFS_fstat(&fs, fd, &s) < 0) {
					snprintf(genBuf, 20, "Fds check error %d", SPIFFS_errno(&fs));
					serialSendQ(genBuf);
			}
// Remove file
			res = SPIFFS_fremove(&fs, fd);
			if (res < 0) {
				snprintf(genBuf, 20, "Fdr: %i", SPIFFS_errno(&fs));
				serialSendQ(genBuf);
			}
			res = SPIFFS_close(&fs, fd);
			if (res != -10008 && res != -10009) { // will return file closed or deleted since we deleted it
				snprintf(genBuf, 20, "Fdc: %i", SPIFFS_errno(&fs));
				serialSendQ(genBuf);
			} else {
				snprintf(genBuf, 20, "Deleted: %i",s.size);
				serialSendQ(genBuf);
			}
		}
	}
	SPIFFS_closedir(&d);
}

/* sfu_create_files
 * - This function creates a file for every subsystem suffix with SPIFFS' current index prefix.
 *
 * Preconditions:
 * 	- all files with the index prefix have been deleted
 * 	- index prefix in spiffs is the one to create files for
 * 	- so if we roll over and want to create up a new set of 'a' files,
 * 		- set index prefix to 'a'
 * 		- remove all existing 'a' files
 * 		- call this function
 */
void sfu_create_files() {
	// CALL WITHIN MUTEX

	char genBuf[20] = { '\0' };
	char nameBuf[2] = { '\0' };
	spiffs_file fd;
	uint8_t i;
//	if (xSemaphoreTake(spiffsTopMutex, pdMS_TO_TICKS(SPIFFS_READ_TIMEOUT_MS)) == pdTRUE) {
	my_spiffs_mount(); // need to mount every time because as task gets suspended, we lose the mount

// Create and write to the file
	for (i = 0; i < FSYS_NUM_SUBSYS; i++) { // run through each subsys and create a file for it
		create_filename(nameBuf, (char) (FSYS_OFFSET + i));
		fd = SPIFFS_open(&fs, nameBuf, SPIFFS_CREAT | SPIFFS_APPEND | SPIFFS_RDWR, 0); // create file with appropriate name
		if (fd < 0) { // check that the create worked
			snprintf(genBuf, 20, "OpenFile: %i", SPIFFS_errno(&fs));
			serialSendQ(genBuf);
		} else { // write to it
			write_fd(fd, "Created"); // created, timestamp will be added automatically
		}

		if (SPIFFS_close(&fs, fd) < 0) {
//				clearBuf(genBuf, 20);
			snprintf(genBuf, 20, "CloseF: %i", SPIFFS_errno(&fs));
			serialSendQ(genBuf);
		}
		clearBuf(genBuf, 20);
		snprintf(genBuf, 20, "Create: %s", nameBuf);
// Todo: this often prints garbage for some reason. Full queue?
		serialSendQ((const char*)genBuf);
	}
//		xSemaphoreGive(spiffsTopMutex);
//	} else {
//		serialSendQ("Create can't get top mutex");
//	}
}

/* sfu_write_fd
 *
 * Given a file descriptor handle, write the printf style data to it and auto-timestamp.
 *  ------ !!! MUST BE CALLED FROM WITHIN A MUTEX !!! --------------
 * Having a file descriptor means we've already opened the file up
 */
void write_fd(spiffs_file fd, char *fmt, ...) {
	char buf[SFU_WRITE_DATA_BUF] = { '\0' };
	va_list argptr;
	va_start(argptr, fmt);
	format_entry(buf, fmt, argptr);
	va_end(argptr);

//	serialSendQ("write");

	if (SPIFFS_write(&fs, fd, buf, strlen(buf) + 1) < 0) {
		snprintf(buf, 20, "FDwe: %i", SPIFFS_errno(&fs));
		serialSendQ(buf);
	}
	// YOU MUST CLOSE THE FILE
}

/* sfu_write_fname
 *
 * Given a file name (through the #define), write the printf-formatted data to it and timestamp
 */
void sfu_write_fname(char f_suffix, char *fmt, ...) {
	char nameBuf[2] = { '\0' };
	char buf[SFU_WRITE_DATA_BUF] = { '\0' };

	va_list argptr;
	va_start(argptr, fmt);
	format_entry(buf, fmt, argptr);
	va_end(argptr);

	create_filename(nameBuf, f_suffix);

// Formatting done, enter mutex and open + write the file
	if (xSemaphoreTake(spiffsTopMutex, pdMS_TO_TICKS(SPIFFS_READ_TIMEOUT_MS)) == pdTRUE) {
		my_spiffs_mount(); // need to mount every time because as task gets suspended, we lose the mount
		spiffs_file fd = SPIFFS_open(&fs, nameBuf, SPIFFS_CREAT | SPIFFS_TRUNC | SPIFFS_RDWR, 0);

		if (fd < 0) { 	// if there's an error opening
			snprintf(buf, 20, "FNoe: %i", SPIFFS_errno(&fs));
			serialSendQ(buf);
		} else { 		// otherwise, write
			serialSendQ("fname_write");
			if (SPIFFS_write(&fs, fd, buf, strlen(buf) + 1) < 0) {
				snprintf(buf, 20, "FNwe: %i", SPIFFS_errno(&fs));
				serialSendQ(buf);
			}
			if (SPIFFS_close(&fs, fd) < 0) {
				//clearBuf(genBuf, 20);
				snprintf(buf, 20, "FNce: %i", SPIFFS_errno(&fs));
				serialSendQ(buf);
			}
		}
		xSemaphoreGive(spiffsTopMutex);
	} else {
		serialSendQ("FNwe: can't get top mutex");
	}
}

void format_entry(char* buf, char *fmt, va_list argptr) {
	/* Lets us write data to the filesystem using printf format specifiers.
	 *
	 * 	Max data supported varies with the time stamp, but consider SFU_MAX_DATA_WRITE
	 * 	= #SFU_WRITE_DATA_BUF - 12 characters, as the max to be safe.
	 *  This value is to allow for 10-char stamp, separator character between stamp and data, and an
	 *  end character to allow us to separate file entries.
	 *
	 * File entry format: "timestamp|data\0"
	 * where:
	 * 		timestamp is max 10 chars (maxed out 32-bit int)
	 * 		| separates timestamp and arbitrary data
	 * 		\0 is the null terminator and also the end of the entry
	 *
	 * 	In the comments, SFU_WRITE_DATA_BUF has an assumed value of 33 bytes
	 */
	uint32_t x;
	x = getCurrentRTCTime();

	va_start(argptr, fmt);

	utoa2(x, buf, 10, 0); // We can store time has hex in the future for byte savings, but it'd be kinda gross.
	x = strlen(buf); // reuse this
	buf[x] = '|'; // add a separator between time and data
	x++; // new strlen after adding sep

	if (sfu_vsnprintf(&buf[x], SFU_WRITE_DATA_BUF - 1 - x, fmt, argptr) > (SFU_WRITE_DATA_BUF - 2 - x)) { // 32 - x so that we always end with a \0, 31 - x for warning
		serialSendQ("Error: file write data too big.");
		// we'll log this error for our notice. However, vsnprintf will protect us from writing past the end of the buffer. Worst case we lose some data.
		// #todo: error log
	}
	va_end(argptr);
}

void create_filename(char* namebuf, char file_suffix) {
	namebuf[0] = *(char *) fs.user_data; // file name prefix. Void pointer help: https://www.geeksforgeeks.org/void-pointer-c/
	namebuf[1] = file_suffix;
}
