/*
 * test_spiffs.h
 *
 *  Created on: Mar 13, 2018
 *      Author: Richard
 *
 *      USAGE - do the following in task_main:
 *      	sfusat_spiffs_init();
 *      	xTaskCreate(spiffs_check_task, "check spiffs", 1400, NULL, 4, &xSPIFFSCheck);
 *			xTaskCreate(spiffs_write_check_test, "write spiffs", 1000, NULL, 3, &xSPIFFSHandle);
 *
 *		You should see us constantly write to a file and should see its size growing.
 *		It'll write about 60 bytes between runs of the check task.
 *
 *		NOTE: bring these up if there are some deep FS issues. Otherwise, prefer our test tasks in
 *		sfu_fs_structure, since they use our wrappers and overall fs structure.
 */

#ifndef SFUSAT_TEST_SEQUENCES_TEST_SPIFFS_RTOS_H_
#define SFUSAT_TEST_SEQUENCES_TEST_SPIFFS_RTOS_H_
#include "sfusat_spiffs.h"
#include "sfu_fs_structure.h"
#include "../SPIFFS/spiffs.h"
#include "FreeRTOS.h"
#include "rtos_semphr.h"

void spiffs_write_check_test(void *pvParameters); // test of the file write mechanism
void spiffs_check_task(void *pvParameters); // returns some stats of the file from write_check_task so we can see it's working
void sfu_file_write(char* file_name, char *fmt, ...); // printf style writes to files

#endif /* SFUSAT_TEST_SEQUENCES_TEST_SPIFFS_RTOS_H_ */
