/*
 * sfu_task_main.c
 *
 *  Created on: May 23, 2017
 *      Author: steven
 */

#include "sfu_tasks.h"
#include "sfu_task_main.h"
#include "sfu_task_radio.h"
#include "sfu_scheduler.h"

TaskHandle_t xSerialTaskHandle = NULL;
TaskHandle_t xRadioTaskHandle = NULL;

TaskHandle_t xBlinkyTaskHandle = NULL;

void vMainTask(void *pvParameters) {
    xTaskCreate( hundredBlinky, /* Pointer to the function that implements the task. */
                 "blinky",/* Text name for the task. This is to facilitate debugging only. */
                 configMINIMAL_STACK_SIZE, /* Stack depth - small microcontrollers will use much less stack than this. */
                 NULL, /* This example does not use the task parameter. */
                 1, /* This task will run at priority 1. */
                 &xBlinkyTaskHandle );

	xTaskCreate(vSerialTask, "serial", 300, NULL, 3, &xSerialTaskHandle);
    xTaskCreate(vRadioTask, "radio", 300, NULL, 4, &xRadioTaskHandle);
    Event_t test_event = {.creation_time = 10, .target_time = 30};
    addEvent(test_event);
    addEvent(test_event);
    showActiveEvents();
    serialSendln("main tasks created");
	while (1) {
		serialSendQ("main");

		vTaskDelay(pdMS_TO_TICKS(6100));
	}

}
