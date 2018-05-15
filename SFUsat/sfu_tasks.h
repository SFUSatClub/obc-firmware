/*
 * SFUTasks.h
 *
 *  Created on: Feb 17, 2017
 *      Author: Richard
 */

#ifndef SFUSAT_SFU_TASKS_H_
#define SFUSAT_SFU_TASKS_H_

#include "FreeRTOS.h"
#include "rtos_task.h"
#include "rtos_queue.h"

#include "gio.h"
#include "sfu_uart.h"
#include "sys_common.h"

#include "sfu_task_main.h"
#include "sfu_task_radio.h"
#include "sfu_hardwaredefs.h"
#include "sfu_state.h"

// ------------ Task default priorities -------------------
// since we may create tasks all over the place, put default priorities here so it's easy to determine relative priority
#define MAIN_TASK_PRIORITY					2
#define BLINKY_TASK_DEFAULT_PRIORITY		3
#define SERIAL_TASK_DEFAULT_PRIORITY		5
#define RADIO_TASK_DEFAULT_PRIORITY			1
#define WATCHDOG_TASK_DEFAULT_PRIORITY		6
#define STATE_TASK_DEFAULT_PRIORITY			3
#define ADC_TASK_DEFAULT_PRIORITY			1
#define FLASH_TASK_DEFAULT_PRIORITY			4
#define TESTS_PRIORITY						3
#define LOGGING_TASK_DEFAULT_PRIORITY		3

extern QueueHandle_t xQueue;
//xTaskHandle vSenderHandle; // task handle for the sender which we can use to delete it

void blinky(void *pvParameters);
void vStateTask(void *pvParameters); // state checker
void vADCRead(void *pvParameters);


extern QueueHandle_t xSerialTXQueue;
extern QueueHandle_t xSerialRXQueue;
void vSerialTask(void *pvParameters);
void vSerialSenderTask(void *pvParameters);

void vTickleTask(void *pvParameters);
void vMonitorTask(void *pvParameters);

// RTOS queue example
void vSenderTask( void *pvParameters );
void vReceiverTask( void *pvParameters );
void periodicSenderTask( void *pvParameters );
void vLoggingTask( void *pvParameters );

// Testing task
void vTestLoggingTask( void *pvParameters );

#endif /* SFUSAT_SFU_TASKS_H_ */
