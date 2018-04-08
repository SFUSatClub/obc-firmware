/*
 * test_spiffs_rtos.c
 *
 *  Created on: Mar 13, 2018
 *      Author: Richard
 */
#include "test_spiffs_rtos.h"
#include "sfu_uart.h"
#include "sfu_rtc.h"
void spiffs_write_check_test(void *pvParameters) {
	/* we probably won't really use this - prefer tests within sfu_fs_structure since they use our wrappers */

	uint32_t counter;
	counter = 0;
//	int32_t check_result;
	my_spiffs_mount();
	while (1) {
		sfu_file_write("new", "wassup%d", counter);
		counter++;
//		if ( xSemaphoreTake( spiffsTopMutex, pdMS_TO_TICKS(SPIFFS_TOP_TIMEOUT_MS) ) == pdTRUE) {
//			my_spiffs_mount(); // need to mount every time because as task gets suspended, we lose the mount
//
//			snprintf(buf, 20, "hello world %d",counter); // make up some data
//			serialSendQ("Write");
//			// Open and write
//			spiffs_file fd = SPIFFS_open(&fs, "new", SPIFFS_CREAT | SPIFFS_APPEND | SPIFFS_RDWR, 1);
//			if (SPIFFS_write(&fs, fd, buf, strlen(buf)) < 0) {
////				printf("Error on SPIFFS write, %i\r\n", SPIFFS_errno(&fs));
//				check_result = SPIFFS_check(&fs);
////				printf("SPIFFS CHECK: %d", check_result);
//			}
//			counter++;
//
//			SPIFFS_close(&fs, fd);
//			xSemaphoreGive(spiffsTopMutex);
//		} else {
//			serialSendQ("Check can't get top mutex.");
//		}
		vTaskDelay(pdMS_TO_TICKS(1500));
	}
}

void spiffs_check_task(void *pvParameters) {
	/* task to test out spiffs operation - use this if our FS seems to be totally blown up */

	spiffs_stat s;
	uint32_t total, used, counter;
	counter = 0;

	while (1) {
		char buf[70] = { '\0' };

		if (xSemaphoreTake(spiffsTopMutex, pdMS_TO_TICKS(SPIFFS_TOP_TIMEOUT_MS)) == pdTRUE) {
			my_spiffs_mount();

			// Get file stats
			spiffs_file fd = SPIFFS_open(&fs, "new", SPIFFS_CREAT | SPIFFS_APPEND | SPIFFS_RDWR, 0);
			if (SPIFFS_fstat(&fs, fd, &s) < 0) {
				snprintf(buf, 50, "Spiffs check error %d", SPIFFS_errno(&fs));
				serialSendQ(buf);
			}

			// Get filesystem stats
			if (SPIFFS_info(&fs, &total, &used) < 0) {
				snprintf(buf, 70, "Spiffs info error %d", SPIFFS_errno(&fs));
				serialSendQ(buf);
//				SPIFFS_vis(&fs);
				SPIFFS_fflush(&fs, fd);
			} else {
				snprintf(buf, 70, "File: %s: %d, %d \r\nSPIFFS: %d, used: %d,--- %d, H: %d", s.name, s.size, s.obj_id,
						total, used, counter, xPortGetFreeHeapSize());
				serialSendQ(buf);
				counter++;
			}
			SPIFFS_close(&fs, fd);
			xSemaphoreGive(spiffsTopMutex);
		} else {
			serialSendQ("Check can't get top mutex.");
		}
		vTaskDelay(pdMS_TO_TICKS(5000));
	}
}

void sfu_file_write(char* file_name, char *fmt, ...) {
	/* Lets us write data to the filesystem using printf format specifiers.
	 *
	 * 	Max data supported varies with the time stamp, but consider SFU_MAX_DATA_WRITE
	 * 	= #SFU_WRITE_DATA_BUF - 12 characters, as the max to be safe.
	 *  This value is to allow for 10-char stamp, separator character between stamp and data, and an
	 *  end character to allow us to separate file entries.
	 *
	 * Todo:
	 * 	- search for correct file
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

	x = getCurrentRTCTime(); // replace with RTC read

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
	if (xSemaphoreTake(spiffsTopMutex, pdMS_TO_TICKS(SPIFFS_TOP_TIMEOUT_MS)) == pdTRUE) {
		my_spiffs_mount(); // need to mount every time because as task gets suspended, we lose the mount

		spiffs_file fd = SPIFFS_open(&fs, file_name, SPIFFS_CREAT | SPIFFS_APPEND | SPIFFS_RDWR, 1);
		if (SPIFFS_write(&fs, fd, buf, strlen(buf) + 1) < 0) {
//				printf("Error on SPIFFS write, %i\r\n", SPIFFS_errno(&fs));
//				check_result = SPIFFS_check(&fs);
//				printf("SPIFFS CHECK: %d", check_result);
		}

		SPIFFS_close(&fs, fd);
		xSemaphoreGive(spiffsTopMutex);
	} else {
		serialSendQ("FWrite can't get top mutex.");
	}
}
