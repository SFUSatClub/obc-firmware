/*
 * sfu_task_main.c
 *
 *  Created on: May 23, 2017
 *      Authors: Steven and Richard
 */

#include "examples/obcsat_examples.h"
#include "obc_adc.h"
#include "obc_fs_structure.h"
#include "obc_gps.h"
#include "obc_i2c.h"
#include "obc_rtc.h"
#include "obc_scheduler.h"
#include "obc_spiffs.h"
#include "obc_startup.h"
#include "obc_state.h"
#include "obc_task_logging.h"
#include "obc_task_main.h"
#include "obc_task_radio.h"
#include "obc_tasks.h"
#include "obc_triumf.h"
#include "printf.h"
#include "flash_mibspi.h"
#include "sun_sensor.h"
#include "stlm75.h"
#include "deployables.h"
#include "bq25703.h"
#include "stdtelem.h"
#include "unit_tests/unit_tests.h"
#include "libmpack/src/object.h"

/**
 * Perpetual tasks - these run all the time.
 */
TaskHandle_t xSerialTaskHandle = NULL;
TaskHandle_t xRadioTaskHandle = NULL;
TaskHandle_t xTickleTaskHandle = NULL;
TaskHandle_t xBlinkyTaskHandle = NULL;
TaskHandle_t xStateTaskHandle = NULL;
TaskHandle_t xFilesystemTaskHandle = NULL;

/**
 * Currently unused tasks.
 */
TaskHandle_t xRadioCHIMEHandle = NULL;
TaskHandle_t xLogToFileTaskHandle = NULL;

/**
 * This is the vMainTask for all platforms except the launchpad platform.
 *
 * Note: Assure changes made here are also reflected in launchpad's vMainTask.
 */
#if defined(PLATFORM_OBC_V0_5) || defined(PLATFORM_OBC_V0_4) || defined(PLATFORM_OBC_V0_3)
void vMainTask(void *pvParameters) {
	/**
	 * Hardware initialization phase.
	 */
	serialInit();
	gioInit();
	/**
	 * Start the watchdog right away so we don't reset.
	 */
	xTaskCreate(vExternalTickleTask, "tickle", 128, NULL, WATCHDOG_TASK_DEFAULT_PRIORITY, &xTickleTaskHandle);
	sfuADCInit();
	spiInit();
	flash_mibspi_init();
	sfu_i2c_init();
	serialGPSInit();
	rtcInit();

    gio_interrupt_example_rtos_init();
    /**
     * The state machine starts in SAFE mode.
     */
	stateMachineInit();


	/**
	 * RTOS initialization phase.
	 */
	serialSendln("ORCASAT Started!");

	printStartupType();
	xSerialTXQueue = xQueueCreate(30, sizeof(portCHAR *));
	xSerialRXQueue = xQueueCreate(10, sizeof(portCHAR));
	xLoggingQueue = xQueueCreate(LOGGING_QUEUE_LENGTH, sizeof(LoggingQueueStructure_t));

	/**
	 * Test initialization phase.
	 *
	 * TODO: if tests fail, actually do something
	 * Also, we can't actually run some of these tests in the future. They erase the flash, for example
	 */
	test_adc_init();
	readGPS();
	//flash_erase_chip();

	setStateRTOS_mode(&state_persistent_data); // tell state machine we're in RTOS control so it can print correctly
	bms_test();

	/**
	 * Spin up top level tasks.
	 *
	 * TODO: Transition to static task allocations.
	 */
	xTaskCreate( blinky,  						// Function for the task to run
			"blinky", 							// Text name for the task. This is to facilitate debugging only.
			configMINIMAL_STACK_SIZE,  			// Stack depth - in words. So 4x this = bytes, 32x this = bits.
			NULL, 								// Task parameter lets you pass stuff in to the task BY REFERENCE. So watch that your data doesn't get deleted. Should probably use a queue instead.
			BLINKY_TASK_DEFAULT_PRIORITY,	  	// Priorities are in sfu_tasks.h
			&xBlinkyTaskHandle );				// Task handles are above

	xTaskCreate(vSerialTask				, "serial"	, 400, NULL, SERIAL_TASK_DEFAULT_PRIORITY	, &xSerialTaskHandle);
	xTaskCreate(vStateTask				, "state"	, 400, NULL, STATE_TASK_DEFAULT_PRIORITY	, &xStateTaskHandle);
	xTaskCreate(vFilesystemLifecycleTask, "fs"		, 500, NULL, FLASH_TASK_DEFAULT_PRIORITY	, &xFilesystemTaskHandle);
	xTaskCreate(vRadioTask				, "radio"	, 600, NULL, portPRIVILEGE_BIT |
																	RADIO_TASK_DEFAULT_PRIORITY	, &xRadioTaskHandle);
	xTaskCreate(deploy_task				, "deploy"	, 128, NULL, 4								, &deployTaskHandle);

	/**
	 * Standard telemetry tasks.
	 */
	xTaskCreate(generalTelemTask		, "t_gen", 300, NULL, 3					 , &xgeneralTelemTaskHandle);
	xTaskCreate(temperatureTelemTask	, "t_temp", 700, NULL, STDTELEM_PRIORITY , &xtemperatureTelemTaskHandle);
	xTaskCreate(transmitTelemUART		, "t_send", 900, NULL, STDTELEM_PRIORITY , &xTransmitTelemTaskHandle);
	xTaskCreate(obcCurrentTelemTask		, "t_curr", 900, NULL, 3				 , &xobcCurrentTelemTaskHandle);
	xTaskCreate(BMSTelemTask			, "t_bms", 900, NULL, 3					 , &xBMSTelemTaskHandle);

	/* Startup things that need RTOS */
	// RA: FLAGS
	logPBISTFails();
	sfu_startup_logs();

	/**
	 * Non-essential "tests".
	 * These are mainly to demonstrate usage of the command/scheduling system.
	 *
	 * TODO: the following command/scheduling system will likely change soon.
	 */

	/**
	 * Right when we spin up the main task, get the heap (example of a command we can issue)
	 */
	CMD_t test_cmd = {.cmd_id = CMD_GET, .subcmd_id = CMD_GET_HEAP};
	Event_t test_event = {.seconds_from_now = 3, .action = test_cmd};
	addEvent(test_event);

	/**
	 * Example of scheduling a command.
	 *
	 * This will run the "get tasks" command 1 second after boot.
	 */
	test_event.seconds_from_now = 1;
	test_event.action.subcmd_id = CMD_GET_TASKS;
	addEvent(test_event);

	/**
	 * Example of scheduling a more complicated command.
	 *
	 * Commented out since this will suspend TASK_BLINKY.
	 */
//	CMD_t test_schd = {
//			.cmd_id = CMD_SCHED,
//			.subcmd_id = CMD_SCHED_ADD,
//			.cmd_sched_data = (CMD_SCHED_DATA_t){
//				.seconds_from_now = 8,
//						.scheduled_cmd_id = CMD_TASK,
//						.scheduled_subcmd_id = CMD_TASK_SUSPEND,
//						.scheduled_cmd_data = {
//								0x40, // This is TASK_BLINKY (0x04) but shifted left since first 4 bits is expected to be the ID. See definition of CMD_SCHED_DATA.
//								0x00
//				}
//			}
//	};
//	addEventFromScheduledCommand(&test_schd);

	showActiveEvents();

	serialSendln("main tasks created");



	  mpack_token_t tokbuf[0xff];
	  size_t tokbufpos = 0;
	  char mpackbuf[256];
	  char *buf = mpackbuf;
	  size_t buflen = sizeof(mpackbuf);
	  mpack_tokbuf_t writer = (mpack_tokbuf_t) MPACK_TOKBUF_INITIAL_VALUE;
	  tokbuf[tokbufpos++] = mpack_pack_sint(0);
	  tokbuf[tokbufpos++] = mpack_pack_sint(1);
	  tokbuf[tokbufpos++] = mpack_pack_sint(0x7f);
	  tokbuf[tokbufpos++] = mpack_pack_sint(0xff);
	  tokbuf[tokbufpos++] = mpack_pack_sint(0xffff);
	  tokbuf[tokbufpos++] = mpack_pack_sint(0xffffffff);
	  tokbuf[tokbufpos++] = mpack_pack_sint(0x7fffffffffffffff);
	  size_t i;
	  for (i = 0; i < tokbufpos; i++)
	    mpack_write(&writer, &buf, &buflen, tokbuf + i);
	  uint8_t expected[] = {
	    0x00,
	    0x01,
	    0x7f,
	    0xcc, 0xff,
	    0xcd, 0xff, 0xff,
	    0xce, 0xff, 0xff, 0xff, 0xff,
	    0xcf, 0x7f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff
	  };











	/**
	 * This is how the command scheduler currently works:
	 * 		- the main task is responsible for running the scheduler
	 * 		- the scheduler currently relies on the main task to call tempAddSecondToHET every second
	 * 			- TODO: hook up the RTC and HET (high end timer) to the scheduler to avoid this "hack"
	 * 		- if the current call of getAction has a scheduled event ready to fire, getAction will return a copy
	 * 		of this command to scheduled_cmd
	 * 		- checkAndRunCommand will then execute the command as appropriate
	 */
	while (1) {
		CMD_t scheduled_cmd;

		if (getAction(&scheduled_cmd)) {
			char buffer[16] = {0};
			snprintf(buffer, 16, "%d:%d:%s", scheduled_cmd.cmd_id, scheduled_cmd.subcmd_id, scheduled_cmd.cmd_data);
			checkAndRunCommand(&scheduled_cmd);
			serialSendQ(buffer);
		}
		tempAddSecondToHET();
		vTaskDelay(pdMS_TO_TICKS(1000));
	}

}
#else

/**
 * MainTask for launchpad
 * 	- launchpad doesn't usually have external HW connected
 * 	- running our HW init functions with non-existent hardware will often hang the system
 * 	- so skip certain HW initialization, task creation for HW-specific things
 */

void vMainTask(void *pvParameters) {
	/**
	 * Hardware initialization
	 */
	serialInit();
	gioInit();
	spiInit();
//	flash_mibspi_init();

	// ---------- SFUSat INIT ----------
	rtcInit();
    gio_interrupt_example_rtos_init();
	stateMachineInit(); // we start in SAFE mode

	// ---------- BRINGUP/PRELIMINARY PHASE ----------
	serialSendln("SFUSat Started!");
	printStartupType();

	// ---------- INIT RTOS FEATURES ----------
	// TODO: encapsulate these
	xSerialTXQueue = xQueueCreate(30, sizeof(portCHAR *));
	xSerialRXQueue = xQueueCreate(10, sizeof(portCHAR));
	xLoggingQueue = xQueueCreate(LOGGING_QUEUE_LENGTH, sizeof(LoggingQueueStructure_t));

	serialSendQ("created queue");

	// ---------- INIT TESTS ----------
	// TODO: if tests fail, actually do something
	// Also, we can't actually run some of these tests in the future. They erase the flash, for example

	// test_flash();
// 	test_triumf_init();
//	flash_erase_chip();

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
//	xTaskCreate(sfu_fs_lifecycle, "fs life", 1500, NULL, 4, &xFSLifecycle);
	xTaskCreate(vLogToFileTask, "logging", 500, NULL, LOGGING_TASK_DEFAULT_PRIORITY, &xLogToFileTaskHandle);
//
//	xTaskCreate(vRadioTask, "radio", 300, NULL, RADIO_TASK_DEFAULT_PRIORITY, &xRadioTaskHandle);
//	vTaskSuspend(xRadioTaskHandle);
	xTaskCreate(vExternalTickleTask, "tickle", 128, NULL, WATCHDOG_TASK_DEFAULT_PRIORITY, &xTickleTaskHandle);

	// TODO: watchdog tickle tasks for internal and external WD. (Separate so we can hard reset ourselves via command, two different ways)
	// TODO: ADC task implemented properly with two sample groups
	// TODO: tasks take in the system state and maybe perform differently (ADC will definitely do this)


// --------------------------- OTHER TESTING STUFF ---------------------------
	// Right when we spin up the main task, get the heap (example of a command we can issue)
	CMD_t test_cmd = {.cmd_id = CMD_GET, .subcmd_id = CMD_GET_HEAP};
	Event_t test_event = {.seconds_from_now = 3, .action = test_cmd};
	addEvent(test_event);

	// Example of scheduling a task
	test_event.seconds_from_now = 1;
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
#endif
