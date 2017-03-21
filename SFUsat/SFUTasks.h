/*
 * SFUTasks.h
 *
 *  Created on: Feb 17, 2017
 *      Author: Richard
 */

#ifndef SFUSAT_SFUTASKS_H_
#define SFUSAT_SFUTASKS_H_
#include "FreeRTOS.h"
#include "rtos_task.h"
#include "rtos_queue.h"
#include "gio.h"
#include "sys_common.h"
#include "SFU_Serial.h"

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
BaseType_t serialSendQ(char * toSend);
BaseType_t serialSendQFromISR(char * toSend);


#endif /* SFUSAT_SFUTASKS_H_ */
