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
TaskHandle_t xTickleTaskHandle = NULL;
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
    xTaskCreate(vTickleTask, "tickle", 128, NULL, 5, &xTickleTaskHandle);
    Event_t test_event = {.creation_time = 10, .target_time = 30};
    addEvent(test_event);
    addEvent(test_event);
    char buffer[32] = {0};
    int i;
    for(i = 0; i < MAX_EVENTS; i++) {
    	const Event_t e = schedule.events[i];
    	if (e._status.active) {
        	snprintf(buffer, 32, "#%d\ne.creation_time: %d", i, e.creation_time);
        	serialSendln(buffer);
        	snprintf(buffer, 32, "e.target_time: %d", e.target_time);
        	serialSendln(buffer);
    	}
    }
    serialSendln("main tasks created");
	while (1) {
		serialSendQ("main");

		vTaskDelay(pdMS_TO_TICKS(6100));
	}

}
