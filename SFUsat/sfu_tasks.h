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

#include "sfu_task_radio.h"

extern QueueHandle_t xQueue;
//xTaskHandle vSenderHandle; // task handle for the sender which we can use to delete it

void hundredBlinky(void *pvParameters);
void vTask2(void *pvParameters);

// RTOS queue example
void vSenderTask( void *pvParameters );
void vReceiverTask( void *pvParameters );
void periodicSenderTask( void *pvParameters );

extern QueueHandle_t xSerialTXQueue;
extern QueueHandle_t xSerialRXQueue;
void vSerialTask(void *pvParameters);
void vSerialSenderTask(void *pvParameters);

#endif /* SFUSAT_SFU_TASKS_H_ */
