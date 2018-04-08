/*
 * sfu_task_main.c
 *
 *  Created on: May 23, 2017
 *      Authors: Steven and Richard
 */

#include "sfu_tasks.h"
#include "sfu_task_main.h"
#include "sfu_task_radio.h"
#include "sfu_scheduler.h"
#include "sfu_rtc.h"
#include "sfu_state.h"

#include "flash_mibspi.h"
#include "sfu_startup.h"

//  ---------- SFUSat Tests (optional) ----------
#include "sfu_triumf.h"
#include "unit_tests/unit_tests.h"

TaskHandle_t xSerialTaskHandle = NULL;
TaskHandle_t xRadioTaskHandle = NULL;
TaskHandle_t xTickleTaskHandle = NULL;
TaskHandle_t xBlinkyTaskHandle = NULL;
TaskHandle_t xADCTaskHandle = NULL;
TaskHandle_t xStateTaskHandle = NULL;
TaskHandle_t xFlashReadHandle = NULL;
TaskHandle_t xFlashWriteHandle = NULL;


TaskHandle_t xRadioRXHandle = NULL;
TaskHandle_t xRadioTXHandle = NULL;
TaskHandle_t xRadioCHIMEHandle = NULL;

void vMainTask(void *pvParameters) {
	/**
	 * Hardware initialization
	 */
	gioInit();
	spiInit();
	flash_mibspi_init();

	// ---------- SFUSat INIT ----------
	rtcInit();
	stateMachineInit(); // we start in SAFE mode


	// ---------- BRINGUP/PRELIMINARY PHASE ----------
	serialSendln("SFUSat Started!");

	watchdog_busywait(3000); // to allow time for serial to connect up to script
	simpleWatchdog(); // do this just to be sure we hit the watchdog before entering RTOS
	printStartupType();

	// ---------- INIT RTOS FEATURES ----------
	// TODO: encapsulate these
	//  xQueue = xQueueCreate(5, sizeof(char *));
	xSerialTXQueue = xQueueCreate(30, sizeof(portCHAR *));
	xSerialRXQueue = xQueueCreate(10, sizeof(portCHAR));
	serialSendQ("created queue");

	xFlashMutex = xSemaphoreCreateMutex();
	xRTCMutex = xSemaphoreCreateMutex();
	// ---------- INIT TESTS ----------
	// TODO: if tests fail, actually do something
	// Also, we can't actually run some of these tests in the future. They erase the flash, for example
	//test_flash();
	test_adc_init();
	//test_triumf_init();
	//    uint32_t time;
	//    time = no_rtos_test_getCurrentRTCTime();

	setStateRTOS_mode(&state_persistent_data); // tell state machine we're in RTOS control so it can print correctly

// --------------------------- SPIN UP TOP LEVEL TASKS ---------------------------
	xTaskCreate( blinky,  						// Function for the task to run
			"blinky", 							// Text name for the task. This is to facilitate debugging only.
			configMINIMAL_STACK_SIZE,  			// Stack depth - in words. So 4x this = bytes, 32x this = bits.
			NULL, 								// Task parameter lets you pass stuff in to the task BY REFERENCE. So watch that your data doesn't get deleted. Should probably use a queue instead.
			BLINKY_TASK_DEFAULT_PRIORITY,	  	// Priorities are in sfu_tasks.h
			&xBlinkyTaskHandle );				// Task handles are above

	//NOTE: Task priorities are #defined in sfu_tasks.h
	xTaskCreate(vSerialTask, "serial", 300, NULL, SERIAL_TASK_DEFAULT_PRIORITY, &xSerialTaskHandle);
	xTaskCreate(vStateTask, "state", 400, NULL, STATE_TASK_DEFAULT_PRIORITY, &xStateTaskHandle);
//	xTaskCreate(vADCRead, "read ADC", 600, NULL, FLASH_WRITE_DEFAULT_PRIORITY, &xADCTaskHandle);

//	xTaskCreate(vFlashRead2, "read", 600, NULL, 4, &xFlashReadHandle);
//	xTaskCreate(vFlashWrite, "write", 600, NULL, FLASH_WRITE_DEFAULT_PRIORITY, &xFlashWriteHandle);

	//xTaskCreate(vRadioTask, "radio", 800, NULL, RADIO_TASK_DEFAULT_PRIORITY, &xRadioTaskHandle);
	//vTaskSuspend(xRadioTaskHandle);
	//	xTaskCreate(vTickleTask, "tickle", 128, NULL, WATCHDOG_TASK_DEFAULT_PRIORITY, &xTickleTaskHandle);

	// TODO: watchdog tickle tasks for internal and external WD. (Separate so we can hard reset ourselves via command, two different ways)
	// TODO: ADC task implemented properly with two sample groups
	// TODO: tasks take in the system state and maybe perform differently (ADC will definitely do this)
	// TODO: appropriate task for filesystem flash


// --------------------------- OTHER TESTING STUFF ---------------------------
	// Right when we spin up the main task, get the heap (example of a command we can issue)
	CMD_t test_cmd = {.cmd_id = CMD_GET, .subcmd_id = CMD_GET_HEAP};
	Event_t test_event = {.seconds_from_now = 3, .action = test_cmd};
	addEvent(test_event);

	// Example of scheduling a task
	test_event.seconds_from_now = 6;
	test_event.action.subcmd_id = CMD_GET_TASKS;
	addEvent(test_event);

//	CMD_t test_schd = {
//			.cmd_id = CMD_SCHED,
//			.subcmd_id = CMD_SCHED_ADD,
//			.cmd_sched_data = (CMD_SCHED_DATA_t){
//				.seconds_from_now = 8,
//						.scheduled_cmd_id = CMD_TASK,
//						.scheduled_subcmd_id = CMD_TASK_SUSPEND,
//						.scheduled_cmd_data = {
//								0x40,
//								0x00
//				}
//			}
//	};
//	addEventFromScheduledCommand(&test_schd);

	showActiveEvents();

	serialSendln("main tasks created");

	while (1) {
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
