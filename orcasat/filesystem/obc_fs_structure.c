/*
 * sfu_fs_structure.c
 *
 *  Created on: Mar 13, 2018
 *      Author: Richard
 */

#include "sys_common.h"
#include "obc_fs_structure.h"
#include "obc_spiffs.h"
#include "spiffs.h"
#include "obc_uart.h"
#include "obc_utils.h"
#include "obc_rtc.h"
#include "obc_tasks.h"
#include "obc_task_logging.h"
#include "obc_flags.h"
#include "filesystem_test_tasks.h"

uint32_t fs_num_increments;
char sfu_prefix; 					// filesystem prefix

// * Todo:
// add error handler that will attempt to create files if they don't exist or something
// * delete oldest function (calls this), called by rescue
// fs rescue task

/* Private functions */
static void refresh_files(); 									/* handles deletion and creation. Note: takes about a second to run */
static void sfu_create_log_files_noMutex(); 								/* creates files w/ current prefix and records creation time */
static void sfu_create_all_files(); 						/* creates files w/ current prefix and records creation time, wrapped in mutex */
static void sfu_create_persistent_files_noMutex();
static void create_filename(char* namebuf, char file_suffix); /* creates filename with appropriate prefix and suffix */
static void sfu_write_fname_offset_noMutex(char f_suffix, uint32_t offset, char *fmt, ...);
static void sfu_read_fname_offset_noMutex(char f_suffix, uint8_t* outbuf, uint8_t size, uint32_t offset);
static void format_entry(char* buf, char *fmt, va_list argptr); /* formats our file entries with timestamp and data */
static void write_fd(spiffs_file fd, char *fmt, ...); 			/* printf style write to an already open file */
static void sfu_delete_prefix_noMutex(const char prefix); 				/* deletes the files with the specified prefix */
static void increment_prefix_noMutex();
static void sfu_write_fname_offset(char f_suffix, uint32_t offset, char *fmt, ...);
static void sfu_read_fname_offset(char f_suffix, uint8_t* outbuf, uint8_t size, uint32_t offset);
static void writeAllFlagsToFlash_noMutex();
static bool readAllFlagsFromFlash_noMutex(flag_memory_table_wrap_t *flagWrap);

/* Filesystem Lifecycle
 * - creates and deletes files when they're old
 */
void vFilesystemLifecycleTask(void *pvParameters) {
	sfu_fs_init();
	while (1) {
		if(fs_num_increments == 0){
			sfusat_spiffs_init();
			sfu_create_all_files();
//			fs_test_tasks();	// Only enable for testing
		}
		vTaskDelay(FSYS_LOOP_INTERVAL);
		refresh_files(); // this handles deletion and creation
		volatile uint32_t level;
		level = uxTaskGetStackHighWaterMark( NULL);
	}
}


/* Dump file
 * - reads a chunk of a file, then prints it out on the UART
 * - used for downloading an entire file
 * */
void dumpFile(char prefix, char suffix){
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
char getCurrentPrefix(){
	return sfu_prefix;
}


void sfu_fs_init() {
	// Todo: grab this from FEE or from config file
	fs_num_increments = 0;
	sfu_prefix = PREFIX_START;
}

/* refresh_files
 * - this function is run at the end of every day.
 * - it increments the file prefix and deletes any old files
 */
static void refresh_files() {
	// take the mutex since we don't want any writes while we're messing with this
	if (xSemaphoreTake(spiffsTopMutex, pdMS_TO_TICKS(SPIFFS_READ_TIMEOUT_MS)) == pdTRUE) {
		increment_prefix_noMutex();

		// delete the files with the current prefix since we're replacing it
		if (fs_num_increments >= PREFIX_QUANTITY) {
			sfu_delete_prefix_noMutex(sfu_prefix);
		}
		// create fresh files
		sfu_create_log_files_noMutex();
		xSemaphoreGive(spiffsTopMutex);
	} else {
		serialSendQ("Del oldest can't get top mutex");
	}
}

/* increment_prefix_noMutex
 * - increments the fs prefix once we've filled up the files for the day
 */
static void increment_prefix_noMutex() {
	// CALL WITHIN MUTEX
	if ((sfu_prefix >= (PREFIX_START + PREFIX_QUANTITY - 1)) || sfu_prefix < PREFIX_START) { // if we're at the end, roll back around

	sfu_prefix = PREFIX_START;
		// RA: FLAGS
//		write_flag_prefix(PREFIX_START);
		FLAG(PREFIX_FLAG,flag) = PREFIX_START;
		FLAG(PREFIX_FLAG,timestamp) = getCurrentRTCTime();
		write_PREFIX_FLAG(flag_memory_table.PREFIX_FLAG.all);
	} else { 							// else just increment to next prefix
		sfu_prefix = sfu_prefix + 1;
		// RA: FLAGS
//		write_flag_prefix(sfu_prefix + 1);
		FLAG(PREFIX_FLAG,flag) = sfu_prefix;
		FLAG(PREFIX_FLAG,timestamp) = getCurrentRTCTime();
		write_PREFIX_FLAG(flag_memory_table.PREFIX_FLAG.all);

		/* read back and check. Set to PREFIX_START if there's an issue */
		flagCharWrap_t pfix_read;
		read_PREFIX_FLAG(pfix_read.all);
		if(pfix_read.payload.flag != sfu_prefix){
			serialSendQ("PREFIX MISMATCH");
			sfu_prefix = PREFIX_START;
			FLAG(PREFIX_FLAG,flag) = sfu_prefix;
			write_PREFIX_FLAG(flag_memory_table.PREFIX_FLAG.all);
		}
		else{
			serialSendln("PREFIX MATCH");
		}
	}
	fs_num_increments++;
}

/*	sfu_delete_prefix_noMutex
 * 		- This function deletes all files with the specified prefix
 * 		- Used to get rid of the oldest set of files when we loop back around
 */
static void sfu_delete_prefix_noMutex(const char prefix) {
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

/* sfu_create_all_files
 * - Creates a file for every subsystem suffix with SPIFFS' current prefix.
 * - also create flag files if they don't exist yet
 *
 * Preconditions:
 *  - given a prefix:
 * 		- all files with the prefix have been deleted
 * 		- prefix in spiffs (sfu_prefix) is the one to create files for
 *
 * 	- Example: if we roll over and want to create up a new set of 'a' files,
 * 		- set prefix to 'a'
 * 		- remove all existing 'a' files
 * 		- call this function
 */
static void sfu_create_all_files(){
	if (xSemaphoreTake(spiffsTopMutex, pdMS_TO_TICKS(SPIFFS_READ_TIMEOUT_MS)) == pdTRUE) {
		sfu_create_persistent_files_noMutex();
		sfu_create_log_files_noMutex();
		xSemaphoreGive(spiffsTopMutex);
	} else {
		serialSendQ("FCnm");
	}
}

/* sfu_create_log_files_noMutex
 * - create the set of filesystem log files for the current sfu_prefix
 */
static void sfu_create_log_files_noMutex() {
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


/* sfu_create_persistent_files
 * - Creates the flag file if it doesn't yet exist
 */
static void sfu_create_persistent_files_noMutex() {
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
			/* created ok - write out the default flags*/
			snprintf(genBuf, 20, "Create Flags");
			serialSendQ(genBuf);
			if (SPIFFS_close(&fs, fd) < 0) {
				snprintf(genBuf, 20, "CloseF: %i", SPIFFS_errno(&fs));
				serialSendQ(genBuf);
			}

			/* initialize flag table and write to flash */
			initFlagTable();
			writeAllFlagsToFlash_noMutex();
			// RA: FLAGS
//			sfu_write_fname_offset_noMutex(nameBuf[1], PREFIX_FLAG_START, PREFIX_FLAG_BASE);
			/* file closed above implicitly */
//			sfu_prefix = read_flag_prefix_noMutex();
			if (SPIFFS_close(&fs, fd) < 0) {
					snprintf(genBuf, 20, "CloseF: %i", SPIFFS_errno(&fs));
					serialSendQ(genBuf);
			}
			sfu_prefix = FLAG(PREFIX_FLAG, flag);
		}
	} else {
		/* already existed */
		serialSendQ("Detected flag file");
		// RA: FLAGS
//		sfu_prefix = read_flag_prefix_noMutex();
//		sfu_prefix = 'a';
		flag_memory_table_wrap_t flags;
		readAllFlagsFromFlash_noMutex(&flags);
		flag_memory_table = flags.flagTable;
		sfu_prefix = FLAG(PREFIX_FLAG, flag);

		/* implicit closure above */
	}

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
static void write_fd(spiffs_file fd, char *fmt, ...) {
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
static void format_entry(char* buf, char *fmt, va_list argptr) {
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

static void sfu_write_fname_offset(char f_suffix, uint32_t offset, char *fmt, ...) {
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

static void sfu_write_fname_offset_noMutex(char f_suffix, uint32_t offset, char *fmt, ...) {
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

static void create_filename(char* namebuf, char file_suffix) {
	/* there's only one flag file, it has prefix 'z' so we don't make a bunch of them */
	if(file_suffix == FSYS_FLAGS){
		namebuf[0] = 'z';
	}
	else{
		namebuf[0] = sfu_prefix; // file name prefix. Void pointer help: https://www.geeksforgeeks.org/void-pointer-c/
	}
	namebuf[1] = file_suffix;
}

static void sfu_read_fname_offset_noMutex(char f_suffix, uint8_t* outbuf, uint8_t size, uint32_t offset){
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

static void sfu_read_fname_offset(char f_suffix, uint8_t* outbuf, uint8_t size, uint32_t offset){
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




/* New flag file system */
void writeAllFlagsToFlash(){
	flag_memory_table_wrap_t flags;
	flags.flagTable = flag_memory_table;
	char nameBuf[3] = { '\0' };
	char buf[SFU_WRITE_DATA_BUF] = { '\0' };

	spiffs_file fd;

	// Formatting done, enter mutex and open + write the file
	if (xSemaphoreTake(spiffsTopMutex, pdMS_TO_TICKS(SPIFFS_READ_TIMEOUT_MS)) == pdTRUE) {
		create_filename(nameBuf, FSYS_FLAGS);
		my_spiffs_mount();
		fd = SPIFFS_open(&fs, nameBuf, SPIFFS_APPEND | SPIFFS_RDWR, 0);

		if (fd < 0) { 	// if there's an error opening
			snprintf(buf, 20, "FNoe: %i", SPIFFS_errno(&fs));
			serialSendQ(buf);
		}
		else { 		// no error, write to it
			if (SPIFFS_write(&fs, fd, flags.flagTableBytes, sizeof(flags)) < 0) {
				snprintf(buf, 20, "FFww: %i", SPIFFS_errno(&fs));
				serialSendQ(buf);
			}
			if (SPIFFS_close(&fs, fd) < 0) {
				//clearBuf(genBuf, 20);
				snprintf(buf, 20, "FFce: %i", SPIFFS_errno(&fs));
				serialSendQ(buf);
			}
		}
		xSemaphoreGive(spiffsTopMutex);
	} else {serialSendQ("FFwe: can't get top mutex");
	}
}

bool readAllFlagsFromFlash(flag_memory_table_wrap_t *flagWrap){
	char nameBuf[3] = { '\0' };
	char buf[20] = {'\0'};
	spiffs_file fd;
	create_filename(nameBuf, FSYS_FLAGS);

	if (xSemaphoreTake(spiffsTopMutex, pdMS_TO_TICKS(SPIFFS_READ_TIMEOUT_MS)) == pdTRUE) {
		my_spiffs_mount();
		fd = SPIFFS_open(&fs, (const char *)nameBuf, SPIFFS_RDONLY, 0);

		if (fd < 0) { 	// if there's an error opening
			snprintf(buf, 20, "FFno: %i", SPIFFS_errno(&fs));
			serialSendQ(buf);
			xSemaphoreGive(spiffsTopMutex);
			return 0;
		}
		else { // read the file
		  if (SPIFFS_read(&fs, fd, flagWrap, FLAG_TABLE_SIZE) < 0) {
			  snprintf(buf, 20, "FDnr: %i", SPIFFS_errno(&fs));
			  serialSendQ(buf);
		  }
		  SPIFFS_close(&fs, fd);
		}
		xSemaphoreGive(spiffsTopMutex);
		return 1;
	}
	else {
		serialSendQ("FRwe: can't get top mutex");
	}
	return 0;
}

static void writeAllFlagsToFlash_noMutex(){
	flag_memory_table_wrap_t flags;
	flags.flagTable = flag_memory_table;
	char nameBuf[3] = { '\0' };
	char buf[SFU_WRITE_DATA_BUF] = { '\0' };

	spiffs_file fd;

	// Formatting done, enter mutex and open + write the file
	create_filename(nameBuf, FSYS_FLAGS);
	my_spiffs_mount();
	fd = SPIFFS_open(&fs, nameBuf, SPIFFS_APPEND | SPIFFS_RDWR, 0);

	if (fd < 0) { 	// if there's an error opening
		snprintf(buf, 20, "FNoe: %i", SPIFFS_errno(&fs));
		serialSendQ(buf);
	}
	else { 		// no error, write to it
		if (SPIFFS_write(&fs, fd, flags.flagTableBytes, sizeof(flags)) < 0) {
			snprintf(buf, 20, "FFww: %i", SPIFFS_errno(&fs));
			serialSendQ(buf);
		}
		if (SPIFFS_close(&fs, fd) < 0) {
			//clearBuf(genBuf, 20);
			snprintf(buf, 20, "FFce: %i", SPIFFS_errno(&fs));
			serialSendQ(buf);
		}
	}
}

static bool readAllFlagsFromFlash_noMutex(flag_memory_table_wrap_t *flagWrap){
	char nameBuf[3] = { '\0' };
	char buf[20] = {'\0'};
	spiffs_file fd;
	create_filename(nameBuf, FSYS_FLAGS);
	fd = SPIFFS_open(&fs, (const char *)nameBuf, SPIFFS_RDONLY, 0);

	if (fd < 0) { 	// if there's an error opening
		snprintf(buf, 20, "FFno: %i", SPIFFS_errno(&fs));
		serialSendQ(buf);
		xSemaphoreGive(spiffsTopMutex);
		return 0;
	}
	else { // read the file
	  if (SPIFFS_read(&fs, fd, flagWrap, FLAG_TABLE_SIZE) < 0) {
		  snprintf(buf, 20, "FDnr: %i", SPIFFS_errno(&fs));
		  serialSendQ(buf);
	  }
	  SPIFFS_close(&fs, fd);
	}
	return 1;
}



void writeFlagRaw(uint8_t *bytes, uint8_t size, uint32_t offset){
	char nameBuf[3] = { '\0' };
	char buf[SFU_WRITE_DATA_BUF] = { '\0' };
	spiffs_file fd;

	// Formatting done, enter mutex and open + write the file
	create_filename(nameBuf, FSYS_FLAGS);
	my_spiffs_mount();

	fd = SPIFFS_open(&fs, nameBuf, SPIFFS_RDWR, 0);
	SPIFFS_lseek(&fs, fd, offset, SPIFFS_SEEK_SET);

	if (fd < 0) { 	// if there's an error opening
		snprintf(buf, 20, "FNoe: %i", SPIFFS_errno(&fs));
		serialSendQ(buf);
	}
	else { 		// no error, write to it
		if (SPIFFS_write(&fs, fd, bytes, size) < 0) {
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

void readFlagRaw(uint8_t *bytes, uint8_t size, uint32_t offset){
	char buf[20] = {'\0'};
	char nameBuf[3] = { '\0' };

	int16_t res;
	spiffs_file fd;

	create_filename(nameBuf, FSYS_FLAGS);
	my_spiffs_mount();
	fd = SPIFFS_open(&fs, (const char *) nameBuf, SPIFFS_RDWR, 0);

	res = SPIFFS_lseek(&fs, fd, offset, SPIFFS_SEEK_SET);	/* lseek increments file index to i'th byte */
	if (res < 0) { 	// if there's an error opening
		snprintf(buf, 20, "FROno: %i", SPIFFS_errno(&fs));
		serialSendQ(buf);
	}
	else { // read the file
	  if (SPIFFS_read(&fs, fd, (uint8_t *)bytes, size) < 0) {
		  snprintf(buf, 20, "FROnr: %i", SPIFFS_errno(&fs));
		  serialSendQ(buf);
	  }
	  SPIFFS_close(&fs, fd);
	}
}

