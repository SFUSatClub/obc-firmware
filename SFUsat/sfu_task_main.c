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
#include "sfu_rtc.h"

TaskHandle_t xSerialTaskHandle = NULL;
TaskHandle_t xRadioTaskHandle = NULL;
TaskHandle_t xTickleTaskHandle = NULL;
TaskHandle_t xBlinkyTaskHandle = NULL;
TaskHandle_t xADCTaskHandle = NULL;

void vMainTask(void *pvParameters) {
    xTaskCreate( hundredBlinky, /* Pointer to the function that implements the task. */
                 "blinky",/* Text name for the task. This is to facilitate debugging only. */
                 configMINIMAL_STACK_SIZE, /* Stack depth - small microcontrollers will use much less stack than this. */
                 NULL, /* This example does not use the task parameter. */
                 1, /* This task will run at priority 1. */
                 &xBlinkyTaskHandle );

//    xTaskCreate(vDemoADCTask, "ADC_demo", 300, NULL, 3, &xADCTaskHandle);
	xTaskCreate(vSerialTask, "serial", 300, NULL, 3, &xSerialTaskHandle);
    xTaskCreate(vRadioTask, "radio", 300, NULL, 5, &xRadioTaskHandle);
    xTaskCreate(vTickleTask, "tickle", 128, NULL, 4, &xTickleTaskHandle);

    CMD_t test_cmd = {.cmd_id = CMD_GET, .subcmd_id = CMD_GET_HEAP};
    Event_t test_event = {.seconds_from_now = 3, .action=test_cmd};
    addEvent(test_event);

    test_event.seconds_from_now = 6;
    test_event.action.subcmd_id = CMD_GET_TASKS;
    addEvent(test_event);

    CMD_t test_schd = {
    	.cmd_id = CMD_SCHED,
		.subcmd_id = CMD_SCHED_ADD,
		.cmd_sched_data = (CMD_SCHED_DATA_t){
			.seconds_from_now = 8,
			.scheduled_cmd_id = CMD_TASK,
			.scheduled_subcmd_id = CMD_TASK_SUSPEND,
			.scheduled_cmd_data = {
				0x40,
				0x00
			}
		}
    };
    addEventFromScheduledCommand(&test_schd);

    showActiveEvents();

    serialSendln("main tasks created");

	while (1) {
		serialSendQ("main");
		CMD_t g;
		if (getAction(&g)) {
			char buffer[16] = {0};
			snprintf(buffer, 16, "%d:%d:%s", g.cmd_id, g.subcmd_id, g.cmd_data);
			checkAndRunCommand(&g);
			serialSendQ(buffer);
		}
		tempAddSecondToHET();
		vTaskDelay(pdMS_TO_TICKS(1000));
	}

}
