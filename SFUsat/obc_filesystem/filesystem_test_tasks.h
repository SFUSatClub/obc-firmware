/*
 * filesystem_test_tasks.h
 *
 *  Created on: Aug 6, 2018
 *      Author: Richard
 */

#ifndef SFUSAT_OBC_FILESYSTEM_FILESYSTEM_TEST_TASKS_H_
#define SFUSAT_OBC_FILESYSTEM_FILESYSTEM_TEST_TASKS_H_

#include "sfusat_spiffs.h"
#include "FreeRTOS.h"
#include "rtos_task.h"
#include "sfu_flags.h"

extern TaskHandle_t xSPIFFSHandle;
extern TaskHandle_t xSPIFFSRead;

/* Tasks */
void fs_rando_write(void *pvParameters);
void fs_read_task(void *pvParameters);
void fs_test_tasks();

#endif /* SFUSAT_OBC_FILESYSTEM_FILESYSTEM_TEST_TASKS_H_ */
