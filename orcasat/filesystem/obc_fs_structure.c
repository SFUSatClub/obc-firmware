/*
 * sfu_fs_structure.c
 *
 *  Created on: Mar 13, 2018
 *      Author: Richard
 */

#include <obc_flags.h>
#include <obc_fs_structure.h>
#include <obc_rtc.h>
#include <obc_spiffs.h>
#include <obc_task_logging.h>
#include <obc_tasks.h>
#include <obc_uart.h>
#include <obc_utils.h>
#include "sys_common.h"
#include "spiffs.h"

TaskHandle_t xSPIFFSHandle = NULL; // RA
TaskHandle_t xSPIFFSRead = NULL; // RA
uint32_t fs_num_increments;

// * Todo:
// add error handler that will attempt to create files if they don't exist or something
// * delete oldest function (calls this), called by rescue
// fs rescue task


/* Dump file
 * - reads a chunk of a file, then prints it out on the UART
 * - used for downloading an entire file
 * */
void dumpFile(char prefix, char suffix){
    #define DUMP_BUF_SIZE 30	/* number of bytes to send out at once. Should eventually match radio TX buffer size */
	spiffs_file fd = -1;
	spiffs_stat s;
	int16_t res;
	char fname[3] = {'\0'};
	char buf[DUMP_BUF_SIZE + 1] = {'\0'}; /* +1 to allow for null, which is required for UART transmission */
	fname[0] = prefix;
	fname[1] = suffix;

	if (xSemaphoreTake(spiffsTopMutex, pdMS_TO_TICKS(SPIFFS_READ_TIMEOUT_MS)) == pdTRUE) {
		my_spiffs_mount();
		fd = SPIFFS_open(&fs, (const char *)fname, SPIFFS_RDONLY, 0);

		if (fd < 0) { 	/* if there's an error opening */
			snprintf(buf, DUMP_BUF_SIZE, "DFno: %i", SPIFFS_errno(&fs));
			serialSendQ(buf);
		}
		else {
			res = SPIFFS_fstat(&fs,fd , &s);
			uint32_t i;

			snprintf(buf, DUMP_BUF_SIZE, "FILE: %s %d", fname, s.size);
			serialSendln(( char*)buf);	/* send file name and size */

			for(i = 0; i < s.size + 1; i += DUMP_BUF_SIZE){ /* loop through, read and transmit DUMP_BUF_SIZE bytes at a time */
				res = SPIFFS_lseek(&fs, fd, i, SPIFFS_SEEK_SET);	/* lseek increments file index to i'th byte */
				if(res < 0){
					snprintf(buf, DUMP_BUF_SIZE, "DFnr: %i", SPIFFS_errno(&fs));
				}
				if (SPIFFS_read(&fs, fd, buf, DUMP_BUF_SIZE) < 0) {
					snprintf(buf, DUMP_BUF_SIZE, "DFnr: %i", SPIFFS_errno(&fs));
				}

				/* Read in bytes which can have nulls embedded. Need to remove nulls for transmission as a chunk, since serial functions use strlen */
				uint8_t cnt;
				for(cnt = 0; cnt < DUMP_BUF_SIZE; cnt ++){
					if(buf[cnt] == '\0') buf[cnt] = '\7'; /* replace empty with weird character so it's easy to pick out  */
				}
				buf[DUMP_BUF_SIZE] = '\0';	/* null terminate the chunk so that send function's strlen works */
				serialSendln(( char*)buf);	/* either send out err msg or the data itself */
				clearBuf(buf,DUMP_BUF_SIZE);
			}
			SPIFFS_close(&fs, fd);
			snprintf(buf, DUMP_BUF_SIZE, "FILE_END: %s", fname);
			serialSendln(( char*)buf);	/* either send out err msg or the data itself */
		}
		xSemaphoreGive(spiffsTopMutex);
	}
	else {
		serialSendQ("DFwe: can't get top mutex");
	}
}

/* CurrentPrefix
 * 	- returns the filesystem's current prefix
 * */
char currentPrefix(){

	return *(char *) fs.user_data;
}

// -------------- Tasks for testing --------------------
/* lifecycle
 * - inits, deletes the files in the system as we will in flight
 * - another task is required to write/read the files as would be done normally
 */
void vFilesystemLifecycleTask(void *pvParameters) {
	sfu_fs_init();
	while (1) {
		if(fs_num_increments == 0){
			sfusat_spiffs_init();
			sfu_create_files_wrapped();
			fs_test_tasks();
		}
		vTaskDelay(FSYS_LOOP_INTERVAL);
		delete_oldest(); // this handles deletion and creation
		volatile uint32_t level;
		level = uxTaskGetStackHighWaterMark( NULL);
	}
}

/* some tasks that can be used to demonstrate fs functionality */
void fs_test_tasks(){
//	xTaskCreate(fs_rando_write, "rando write", 500, NULL, 2, &xSPIFFSHandle);
//	xTaskCreate(fs_read_task, "FS read", 400, NULL, 3, xSPIFFSRead);
}

/* random write
 * - writes some random + nonrandom data into the system log file
 * - used to show that file sizes do grow
 */
void fs_rando_write(void *pvParameters){
	while(1){
		vTaskDelay(pdMS_TO_TICKS(4000));
//		char randomData[5] = {'d'};
//		sfu_write_fname(FSYS_SYS, "foo %s", randomData);
		volatile uint16_t thang;
		thang = 34;
		sfu_write_fname(FSYS_SYS, "foo %i", thang);
	}
}

void fs_read_task(void *pvParameters){
	uint8_t data[20];
	while(1){
		vTaskDelay(pdMS_TO_TICKS(6000));
		sfu_read_fname(OBC_CURRENT, data, 20);
		serialSendQ((char*)data);
	}
}

// ------------ Core Functions -------------------------
void sfu_fs_init() {
	// Todo: grab this from FEE or from config file
	fs_num_increments = 0;
}

void delete_oldest() {
	/* delete_oldest
	 * - this function is run at the end of every day.
	 * - it increments the file prefix and deletes any old files
	 */

	// take the mutex since we don't want any writes while we're messing with this
	if (xSemaphoreTake(spiffsTopMutex, pdMS_TO_TICKS(SPIFFS_READ_TIMEOUT_MS)) == pdTRUE) {
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
	/* increment_prefix
	 * - increments the fs prefix once we've filled up the files for the day
	 */

	// CALL WITHIN MUTEX
	if (*(char *) fs.user_data == (PREFIX_START + PREFIX_QUANTITY - 1)) { // if we're at the end, roll back around
//		sfu_prefix = PREFIX_START;
		write_flag_prefix(PREFIX_START);

	} else { 							// else just increment to next prefix
//		sfu_prefix = sfu_prefix + 1;
		write_flag_prefix(sfu_prefix + 1);
	}
	fs_num_increments++;
}

void sfu_delete_prefix(const char prefix) {
	/*	sfu_delete_prefix
	 * 		- This function deletes all files with the specified prefix
	 * 		- Used to get rid of the oldest set of files when we loop back around
	 */

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
				snprintf(genBuf, 20, "Del: %i",s.size);
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
	char nameBuf[3] = { '\0' };
	spiffs_file fd;
	uint8_t i;
	my_spiffs_mount();

	// Create and write to the file
	for (i = 0; i < FSYS_NUM_SUBSYS; i++) { // run through each subsys and create a file for it
		create_filename(nameBuf, (char) (FSYS_OFFSET + i));
		fd = SPIFFS_open(&fs, nameBuf, SPIFFS_CREAT | SPIFFS_APPEND | SPIFFS_RDWR, 0); // create file with appropriate name

		if (fd < 0) { // check that the create worked
			snprintf(genBuf, 20, "OpenFile: %i", SPIFFS_errno(&fs));
			serialSendQ(genBuf);
		} else { // write to it
			write_fd(fd, "Created"); // first entry is creation time
		}

		if (SPIFFS_close(&fs, fd) < 0) {
			snprintf(genBuf, 20, "CloseF: %i", SPIFFS_errno(&fs));
			serialSendQ(genBuf);
		}
		clearBuf(genBuf, 20);
		snprintf(genBuf, 11, "Create: %s", nameBuf);

		if(serialSendQ((const char*)genBuf) != pdPASS){
			serialSendQ("CREATE PRINT FAIL");
		}
	}
}

void sfu_create_files_wrapped(){
	/* Simply a wrapper around create files. We don't want to try to write while we're creating new files */
	if (xSemaphoreTake(spiffsTopMutex, pdMS_TO_TICKS(SPIFFS_READ_TIMEOUT_MS)) == pdTRUE) {
		sfu_create_persistent_files();
		sfu_create_files();
		xSemaphoreGive(spiffsTopMutex);
	} else {
		serialSendQ("FCnm");
	}
}


/* sfu_create_persistent_files
 * - This function creates files that do not get periodically deleted (ex. flags file)
 */
void sfu_create_persistent_files() {
	// CALL WITHIN MUTEX
	char genBuf[20] = { '\0' };
	char nameBuf[3] = { '\0' };
	spiffs_file fd;
	my_spiffs_mount();

	// Create file to contain flags information
	create_filename(nameBuf, FSYS_FLAGS);
	fd = SPIFFS_open(&fs, (const char *) nameBuf, SPIFFS_RDWR, 0); /* attempt to open */

	if (fd < 0) {
		/* doesn't already exist, so create it */
		fd = SPIFFS_open(&fs, (const char *) nameBuf, SPIFFS_CREAT | SPIFFS_RDWR, 0); // create file with appropriate name
		if(fd < 0){
			/* uh oh. some sort of error and we're hosed */
			snprintf(genBuf, 20, "OpenFile: %i", SPIFFS_errno(&fs));
			serialSendQ(genBuf);
		} else{
			/* created ok - register a as the prefix*/
			snprintf(genBuf, 20, "Create Flags");
			serialSendQ(genBuf);
			if (SPIFFS_close(&fs, fd) < 0) {
				snprintf(genBuf, 20, "CloseF: %i", SPIFFS_errno(&fs));
				serialSendQ(genBuf);
			}
			sfu_write_fname_offset_noMutex(nameBuf[1], PREFIX_FLAG_START, PREFIX_FLAG_BASE);
			/* file closed above implicitly */
			sfu_prefix = read_flag_prefix_noMutex();
		}
	} else {
		/* already existed */
		serialSendQ("Detected flag file");
		sfu_prefix = read_flag_prefix_noMutex();
		/* implicit closure above */
	}

	clearBuf(genBuf, 20);
	snprintf(genBuf, 11, "Create: %s", nameBuf);

	if(serialSendQ((const char*)genBuf) != pdPASS){
		serialSendQ("CREATE PRINT FAIL");
	}
}
/* write_fd
 *
 * Given a file descriptor handle, write the printf style data to it and auto-timestamp.
 *  ------ !!! MUST BE CALLED FROM WITHIN A MUTEX !!! --------------
 * Having a file descriptor means we've already opened the file up
 */
void write_fd(spiffs_file fd, char *fmt, ...) {
	char buf[SFU_WRITE_DATA_BUF] = { '\0' };
	volatile va_list argptr;
	va_start(argptr, fmt);
	format_entry(buf, fmt, argptr);
	va_end(argptr);

	if (SPIFFS_write(&fs, fd, buf, strlen(buf) + 1) < 0) {
		snprintf(buf, 20, "FDwe: %i", SPIFFS_errno(&fs));
		serialSendQ(buf);
	}
	// YOU MUST CLOSE THE FILE
}

/* sfu_write_fname
 * - Given a file name (through the #define), write the printf-formatted data to it and timestamp
 */
void sfu_write_fname(char f_suffix, char *fmt, ...) {
	char nameBuf[3] = { '\0' };
	char buf[SFU_WRITE_DATA_BUF] = { '\0' };

	volatile va_list argptr;
	va_start(argptr, fmt);
	format_entry(buf, fmt, argptr);
	va_end(argptr);
	spiffs_file fd;

	// Formatting done, enter mutex and open + write the file
	if (xSemaphoreTake(spiffsTopMutex, pdMS_TO_TICKS(SPIFFS_READ_TIMEOUT_MS)) == pdTRUE) {
		create_filename(nameBuf, f_suffix);

		my_spiffs_mount();
		 fd = SPIFFS_open(&fs, nameBuf, SPIFFS_APPEND | SPIFFS_RDWR, 0);

		if (fd < 0) { 	// if there's an error opening
			snprintf(buf, 20, "FNoe: %i", SPIFFS_errno(&fs));
			serialSendQ(buf);
		}
		else { 		// no error, write to it
			if (SPIFFS_write(&fs, fd, buf, strlen(buf) + 1) < 0) {
				snprintf(buf, 20, "FNww: %i", SPIFFS_errno(&fs));
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

/* Lets us write data to the filesystem using printf format specifiers.
 *
 * 	Max data supported varies with the time stamp, but consider SFU_MAX_DATA_WRITE
 * 	= #SFU_WRITE_DATA_BUF - 12 characters, as the max to be safe.
 *  This value is to allow for 10-char stamp, separator character between stamp and data, and an
 *  end character to allow us to separate file entries.
 *
 * File entry format: "<timestamp>|<data>\0"
 * where:
 * 		timestamp is max 10 chars (maxed out 32-bit int)
 * 		| separates timestamp and arbitrary data
 * 		\0 is the null terminator and also the end of the entry
 *
 * 	In the comments, SFU_WRITE_DATA_BUF has an assumed value of 33 bytes
 */
void format_entry(char* buf, char *fmt, va_list argptr) {
	uint32_t x;
	x = getCurrentRTCTime();

	utoa2(x, buf, 10, 0); 	// We can store time has hex in the future for byte savings, but it'd be kinda gross.
	x = strlen(buf); 		// reuse this
	buf[x] = '|'; 			// add a separator between time and data
	x++; 					// new strlen after adding sep

	if (sfu_vsnprintf(&buf[x], SFU_WRITE_DATA_BUF - 1 - x, fmt, argptr) > (SFU_WRITE_DATA_BUF - 2 - x)) { // 32 - x so that we always end with a \0, 31 - x for warning
		serialSendQ("Error: file write data too big.");
		// we'll log this error for our notice. However, vsnprintf will protect us from writing past the end of the buffer. Worst case we lose some data.
		addLogItem(logtype_filesystem, error_1);
	}
}

/* sfu_read_fname
 * 		- given a file suffix (the log type to access), reads <size> bytes from the current log into outbuf
 */
void sfu_read_fname(char f_suffix, uint8_t * outbuf, uint32_t size){
	char nameBuf[3] = { '\0' };
	char buf[20] = {'\0'};
	spiffs_file fd;
	create_filename(nameBuf, f_suffix);

	if (xSemaphoreTake(spiffsTopMutex, pdMS_TO_TICKS(SPIFFS_READ_TIMEOUT_MS)) == pdTRUE) {
		my_spiffs_mount();
		fd = SPIFFS_open(&fs, (const char *)nameBuf, SPIFFS_RDONLY, 0);

		if (fd < 0) { 	// if there's an error opening
			snprintf(buf, 20, "FRno: %i", SPIFFS_errno(&fs));
			serialSendQ(buf);
		}
		else { // read the file
		  if (SPIFFS_read(&fs, fd, (uint8_t *)outbuf, size) < 0) {
			  snprintf(buf, 20, "FRnr: %i", SPIFFS_errno(&fs));
			  serialSendQ(buf);
		  }
		  SPIFFS_close(&fs, fd);
		}
		xSemaphoreGive(spiffsTopMutex);
	}
	else {
		serialSendQ("FRwe: can't get top mutex");
	}
}

void sfu_write_fname_offset(char f_suffix, uint32_t offset, char *fmt, ...) {
	char nameBuf[3] = { '\0' };
	char buf[SFU_WRITE_DATA_BUF] = { '\0' };

	volatile va_list argptr;
	va_start(argptr, fmt);
	format_entry(buf, fmt, argptr);
	va_end(argptr);
	spiffs_file fd;

	// Formatting done, enter mutex and open + write the file
	if (xSemaphoreTake(spiffsTopMutex, pdMS_TO_TICKS(SPIFFS_READ_TIMEOUT_MS)) == pdTRUE) {
		create_filename(nameBuf, f_suffix);

		my_spiffs_mount();

		fd = SPIFFS_open(&fs, nameBuf, SPIFFS_RDWR, 0);
		SPIFFS_lseek(&fs, fd, offset, SPIFFS_SEEK_SET);

		if (fd < 0) { 	// if there's an error opening
			snprintf(buf, 20, "FNoe: %i", SPIFFS_errno(&fs));
			serialSendQ(buf);
		}
		else { 		// no error, write to it
			if (SPIFFS_write(&fs, fd, fmt, strlen(fmt) + 1) < 0) {
				snprintf(buf, 20, "FNww: %i", SPIFFS_errno(&fs));
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
		serialSendQ("ONwe: can't get top mutex");
	}
}

void sfu_write_fname_offset_noMutex(char f_suffix, uint32_t offset, char *fmt, ...) {
	char nameBuf[3] = { '\0' };
	char buf[SFU_WRITE_DATA_BUF] = { '\0' };

	volatile va_list argptr;
	va_start(argptr, fmt);
	format_entry(buf, fmt, argptr);
	va_end(argptr);
	spiffs_file fd;

	// Formatting done, enter mutex and open + write the file
	create_filename(nameBuf, f_suffix);

	my_spiffs_mount();

	fd = SPIFFS_open(&fs, nameBuf, SPIFFS_RDWR, 0);
	SPIFFS_lseek(&fs, fd, offset, SPIFFS_SEEK_SET);

	if (fd < 0) { 	// if there's an error opening
		snprintf(buf, 20, "FNoe: %i", SPIFFS_errno(&fs));
		serialSendQ(buf);
	}
	else { 		// no error, write to it
		if (SPIFFS_write(&fs, fd, fmt, strlen(fmt) + 1) < 0) {
			snprintf(buf, 20, "FNww: %i", SPIFFS_errno(&fs));
			serialSendQ(buf);
		}
		if (SPIFFS_close(&fs, fd) < 0) {
			//clearBuf(genBuf, 20);
			snprintf(buf, 20, "FNce: %i", SPIFFS_errno(&fs));
			serialSendQ(buf);
		}
	}
}

void create_filename(char* namebuf, char file_suffix) {
	/* there's only one flag file, it has prefix 'a' */
	if(file_suffix == FSYS_FLAGS){
		namebuf[0] = 'z';
	}
	else{
		namebuf[0] = *(char *) fs.user_data; // file name prefix. Void pointer help: https://www.geeksforgeeks.org/void-pointer-c/
	}
	namebuf[1] = file_suffix;
}

void sfu_read_fname_offset_noMutex(char f_suffix, uint8_t* outbuf, uint8_t size, uint32_t offset){
	char buf[20] = {'\0'};
	char nameBuf[3] = { '\0' };
	create_filename(nameBuf, f_suffix);

	int16_t res;
	spiffs_file fd;

	my_spiffs_mount();
	fd = SPIFFS_open(&fs, (const char *) nameBuf, SPIFFS_RDWR, 0);

	res = SPIFFS_lseek(&fs, fd, offset, SPIFFS_SEEK_SET);	/* lseek increments file index to i'th byte */
	if (res < 0) { 	// if there's an error opening
		snprintf(buf, 20, "FROno: %i", SPIFFS_errno(&fs));
		serialSendQ(buf);
	}
	else { // read the file
	  if (SPIFFS_read(&fs, fd, (uint8_t *)outbuf, size) < 0) {
		  snprintf(buf, 20, "FROnr: %i", SPIFFS_errno(&fs));
		  serialSendQ(buf);
	  }

	  SPIFFS_close(&fs, fd);
	}
}

void sfu_read_fname_offset(char f_suffix, uint8_t* outbuf, uint8_t size, uint32_t offset){
	char buf[20] = {'\0'};
	char nameBuf[3] = { '\0' };

	int16_t res;
	spiffs_file fd;

	if (xSemaphoreTake(spiffsTopMutex, pdMS_TO_TICKS(SPIFFS_READ_TIMEOUT_MS)) == pdTRUE) {
		create_filename(nameBuf, f_suffix);
		my_spiffs_mount();
		fd = SPIFFS_open(&fs, (const char *) nameBuf, SPIFFS_RDWR, 0);

		res = SPIFFS_lseek(&fs, fd, offset, SPIFFS_SEEK_SET);	/* lseek increments file index to i'th byte */
		if (res < 0) { 	// if there's an error opening
			snprintf(buf, 20, "FROno: %i", SPIFFS_errno(&fs));
			serialSendQ(buf);
		}
		else { // read the file
		  if (SPIFFS_read(&fs, fd, (uint8_t *)outbuf, size) < 0) {
			  snprintf(buf, 20, "FROnr: %i", SPIFFS_errno(&fs));
			  serialSendQ(buf);
		  }

		  SPIFFS_close(&fs, fd);
		}
		xSemaphoreGive(spiffsTopMutex);
	}
	else {
		serialSendQ("FROwe: can't get top mutex");
	}
}

char read_flag_prefix_noMutex(void){
	// read at prefix offset
	uint8_t buf[20] = {'\0'};
	sfu_read_fname_offset_noMutex(FSYS_FLAGS, buf, PREFIX_FLAG_LEN + 1, 0);//PREFIX_FLAG_START);
	return buf[PREFIX_FLAG_LEN];
}

void write_flag_prefix(char input_prefix){
	// CALL WITHIN MUTEX
		char genBuf[20] = { '\0' };
		char nameBuf[3] = { '\0' };
		char writeBuf[] = PREFIX_FLAG_BASE;
		spiffs_file fd;
		my_spiffs_mount();

		create_filename(nameBuf, FSYS_FLAGS);
		fd = SPIFFS_open(&fs, (const char *) nameBuf, SPIFFS_RDWR, 0); /* attempt to open */

		if (fd < 0) {
			snprintf(genBuf, 20, "Update pfix !open");
			serialSendQ(genBuf);
		} else{
			snprintf(genBuf, 20, "Updating Prefix");
			serialSendQ(genBuf);
			writeBuf[PREFIX_FLAG_LEN] = input_prefix;
			sfu_write_fname_offset_noMutex(nameBuf[1], PREFIX_FLAG_START, writeBuf);
				/* file closed above implicitly */
			sfu_prefix = read_flag_prefix_noMutex();
			if(sfu_prefix != input_prefix){
				snprintf(genBuf, 20, "PREFIX MISMATCH");
				serialSendQ(genBuf);
				sfu_prefix = 'a';	/* worst case we just hard code a prefix */
			}
		}
}
