/*
 * sfu_fs_structure.c
 *
 *  Created on: Mar 13, 2018
 *      Author: Richard
 */
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
		nameBuf[0] = char()// spiffs prefix
		for (i = 0; i < FS_NUM_SUBSYS; i++) { // run through each subsys
			nameBuf[1] = (char)(FS_SUBSYS_OFFSET + i); // make the file suffix

			fd = SPIFFS_open(&fs, nameBuf, SPIFFS_CREAT | SPIFFS_RDWR, 0); // create file with appropriate name
			if (fd < 0) { // check that the create worked
				snprintf(genBuf, 20, "OpenFile: %i", SPIFFS_errno(&fs));
				serialSendQ(genBuf);
			} else { // write to it
				// write function
			}

			// Close the file
			if (SPIFFS_close(&fs, fd) < 0) {
				snprintf(genBuf, 20, "CloseF: %i", SPIFFS_errno(&fs));
				serialSendQ(genBuf);
			}
		}
		xSemaphoreGive(spiffsTopMutex);
	} else {
		serialSendQ("Create can't get top mutex");
	}
}

/* sfu_write_fd
 *
 * Given a file descriptor handle, write the printf style data to it and auto-timestamp.
 * MUST BE CALLED FROM WITHIN A MUTEX
 */
void sfu_write_fd(){
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

		x = 4294967292; // replace with RTC read

		va_list argptr;
		va_start(argptr, fmt);

		utoa2(x, buf, 10, 0); // We can store time has hex in the future for byte savings, but it'd be kinda gross.
		x = strlen(buf); // reuse this
		buf[x] = '|'; // add a separator between time and data
		x++; // new strlen after adding sep

		if (sfu_vsnprintf(&buf[x], SFU_WRITE_DATA_BUF - 1 - x, fmt, argptr) > (SFU_WRITE_DATA_BUF - 2 - x)) { // 32 - x so that we always end with a \0, 31 - x for warning
			serialSendQ("Error: file write data too big.");
			// we'll log this error for our notice. However, vsnprintf will protect us from writing past the end of the buffer.
			// error log
		}
		va_end(argptr);
		serialSendQ("write");


		spiffs_file fd = SPIFFS_open(&fs, file_name, SPIFFS_CREAT | SPIFFS_APPEND | SPIFFS_RDWR, 1);
		if (SPIFFS_write(&fs, fd, buf, strlen(buf) + 1) < 0) {
//				printf("Error on SPIFFS write, %i\r\n", SPIFFS_errno(&fs));
//				check_result = SPIFFS_check(&fs);
//				printf("SPIFFS CHECK: %d", check_result);
		}

		SPIFFS_close(&fs, fd);
}

/* sfu_write_fname
 *
 * Given a file name (through the #define), write the printf-formatted data to it and timestamp
 */
void sfu_write_fname(){
// given a file name (from the defines), find and open it into an fd, call write_fd
}

}
