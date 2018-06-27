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

void vRadioRX(void *pvParameters);
void vRadioTX(void *pvParameters);
void vRadioCHIME(void *pvParameters);

BaseType_t initRadio();
static int writeToTxFIFO(const uint8 *src, uint8 size);
static void gio_rf_interrupt_init(void);

void rf_interrupt_init(void); // standalone initializer for tasks and semaphore
void gio_notification_RF(gioPORT_t *port, uint32 bit); // called in gionotification, raises semaphore to start the task
void vRFInterruptTask(void *pvParameters); // Task that runs when the interrupt fires
void rfTestSequence();
typedef struct RadioDAT {
	uint8_t unused;
	uint8_t size;
	uint8_t data[192];
} RadioDAT_t;

extern QueueHandle_t xRadioTXQueue;
extern QueueHandle_t xRadioRXQueue;

uint8 hello;


#define RF_NOTIF_TX			(0x101)
#define RF_NOTIF_RESET		(0xDEADDDDD)

#endif /* SFUSAT_SFU_TASK_RADIO_H_ */
