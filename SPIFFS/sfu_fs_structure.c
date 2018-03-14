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
void sfu_create_fs_test(void *pvParameters){

	while(1){
		sfu_create_files();
		vTaskDelay(pdMS_TO_TICKS(5000));
	}
}


// ------------ Core Functions -------------------------

/* sfu_create_files
 * - This function creates a file for every subsystem suffix with SPIFFS' current
 * - index prefix.
 *
 * Preconditions:
 * 	- all files with the index prefix have been deleted
 * 	- index prefix in spiffs is the one to create files for
 * 	- so if we roll over and want to set up a new set of 'a' files,
 * 		- remove all 'a' files
 * 		- remove all 'b' files (we want to give ourselves lots of room)
 * 		- set index prefix to 'a'
 * 		- call this function
 */
void sfu_create_files() {
	char genBuf[20] = { '\0' };
	char nameBuf[2] = { '\0' };
	spiffs_file fd;
	uint8_t i;
	if (xSemaphoreTake(spiffsTopMutex, pdMS_TO_TICKS(SPIFFS_READ_TIMEOUT_MS)) == pdTRUE) {
		my_spiffs_mount(); // need to mount every time because as task gets suspended, we lose the mount

		// Create, write the file
		nameBuf[0] = *(char *)fs.user_data; // file name prefix. Void pointer help: https://www.geeksforgeeks.org/void-pointer-c/

		for (i = 0; i < FSYS_NUM_SUBSYS; i++) { // run through each subsys and create a file for it
			nameBuf[1] = (char)(FSYS_OFFSET + i); // make the file suffix

			fd = SPIFFS_open(&fs, nameBuf, SPIFFS_CREAT | SPIFFS_APPEND | SPIFFS_RDWR, 0); // create file with appropriate name
			if (fd < 0) { // check that the create worked
				snprintf(genBuf, 20, "OpenFile: %i", SPIFFS_errno(&fs));
				serialSendQ(genBuf);
			} else { // write to it
				write_fd(fd, "Created"); // created, timestamp will be added automatically
			}

			// Close the file
			if (SPIFFS_close(&fs, fd) < 0) {
//				clearBuf(genBuf, 20);
				snprintf(genBuf, 20, "CloseF: %i", SPIFFS_errno(&fs));
				serialSendQ(genBuf);
			}
			snprintf(genBuf, 20, "Create: %s", nameBuf);
			serialSendQ(genBuf);
//			clearBuf(genBuf, 20);
		}
		xSemaphoreGive(spiffsTopMutex);
	} else {
		serialSendQ("Create can't get top mutex");
	}
}

/* sfu_write_fd
 *
 * Given a file descriptor handle, write the printf style data to it and auto-timestamp.
 *  ------ !!! MUST BE CALLED FROM WITHIN A MUTEX !!! --------------
 * Having a file descriptor means we've already opened the file up
 */
void write_fd(spiffs_file fd, char *fmt, ...){
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
		char buf[SFU_WRITE_DATA_BUF] = { '\0' };

		x = getCurrentRTCTime();

		va_list argptr;
		va_start(argptr, fmt);

		utoa2(x, buf, 10, 0); // We can store time has hex in the future for byte savings, but it'd be kinda gross.
		x = strlen(buf); // reuse this
		buf[x] = '|'; // add a separator between time and data
		x++; // new strlen after adding sep

		if (sfu_vsnprintf(&buf[x], SFU_WRITE_DATA_BUF - 1 - x, fmt, argptr) > (SFU_WRITE_DATA_BUF - 2 - x)) { // 32 - x so that we always end with a \0, 31 - x for warning
			serialSendQ("Error: file write data too big.");
			// we'll log this error for our notice. However, vsnprintf will protect us from writing past the end of the buffer. Worst case we lose some data.
			// error log
		}
		va_end(argptr);
		serialSendQ("write");

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
void sfu_write_fname(char f_suffix ){
// given a file name (from the defines), find and open it into an fd, call write_fd
	// FIND
	// WRITE
	// CLOSE
}
