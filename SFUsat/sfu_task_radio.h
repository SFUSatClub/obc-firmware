/*
 * sfu_task_radio.h
 *
 *  Created on: Apr 30, 2017
 *      Author: steven
 */

#ifndef SFUSAT_SFU_TASK_RADIO_H_
#define SFUSAT_SFU_TASK_RADIO_H_

#include "FreeRTOS.h"
#include "rtos_task.h"
#include "rtos_queue.h"

void vRadioTask( void *pvParameters );

BaseType_t initRadio();
static int writeToTxFIFO(const uint8 *src, uint8 size);
static void gio_rf_interrupt_init(void);

void rf_interrupt_init(void); // standalone initializer for tasks and semaphore
void gio_notification_RF(gioPORT_t *port, uint32 bit); // called in gionotification, raises semaphore to start the task
void vRFInterruptTask(void *pvParameters); // Task that runs when the interrupt fires


extern QueueHandle_t xRadioTXQueue;
extern QueueHandle_t xRadioRXQueue;

uint8 hello;

#endif /* SFUSAT_SFU_TASK_RADIO_H_ */
