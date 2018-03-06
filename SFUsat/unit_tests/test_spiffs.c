/*
 * test_spiffs.c
 *
 *  Created on: Mar 4, 2018
 *      Author: Richard
 */

#include "FreeRTOS.h"
#include "rtos_task.h"
#include "rtos_queue.h"
#include "rtos_semphr.h"
#include "sfusat_spiffs.h"
#include "printf.h"
uint32_t counter;

void spiffs_write_task(void *pvParameters){
	counter = 0;
	char buf[20];

	while(1){
		my_spiffs_mount();

		snprintf(buf, 20, "hello");
		 spiffs_file fd = SPIFFS_open(&fs, "my_file", SPIFFS_CREAT | SPIFFS_TRUNC | SPIFFS_RDWR, 0);

		 if (SPIFFS_write(&fs, fd, buf, 20) < 0){
			   printf("Error on SPIFFS write, %i\r\n", SPIFFS_errno(&fs));
		   }
		   counter++;
			vTaskDelay(pdMS_TO_TICKS(3000));
	}
}

//void spiffs_read_task(void *pvParameters){
//	fd = SPIFFS_open(&fs, "my_file", SPIFFS_RDWR, 0);
//	if (SPIFFS_read(&fs, fd, (u8_t *)buf, 12) < 0){
//		   printf("Error on SPIFFS Read, %i\r\n", SPIFFS_errno(&fs));
//	}
//	printf(buf);
//
//	SPIFFS_close(&fs, fd);
//}

