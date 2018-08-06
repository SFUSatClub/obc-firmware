/*
 * filesystem_test_tasks.c
 *
 *  Created on: Aug 6, 2018
 *      Author: Richard
 */
#include "sfu_uart.h"
#include "sfu_utils.h"
#include "sys_common.h"
#include "sfu_fs_structure.h"
#include "filesystem_test_tasks.h"

TaskHandle_t xSPIFFSHandle = NULL;
TaskHandle_t xSPIFFSRead = NULL;

// -------------- Tasks for testing --------------------

/* some tasks that can be used to demonstrate fs functionality */
void fs_test_tasks(){
	xTaskCreate(fs_rando_write, "rando write", 500, NULL, 2, &xSPIFFSHandle);
	xTaskCreate(fs_read_task, "FS read", 400, NULL, 3, xSPIFFSRead);
}

/* random write
 * - writes some random + nonrandom data into the system log file
 * - used to show that file sizes do grow
 */
void fs_rando_write(void *pvParameters){
	while(1){
		vTaskDelay(pdMS_TO_TICKS(4000));
		char randomData[5];
		sfu_write_fname(FSYS_SYS, "foo %s", randomData);
//		volatile uint16_t thang;
//		thang = 34;
//		sfu_write_fname(FSYS_SYS, "foo %i", thang);
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

