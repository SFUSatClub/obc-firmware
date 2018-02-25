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

extern QueueHandle_t xRadioTXQueue;
extern QueueHandle_t xRadioRXQueue;

uint8 hello;

//typedef struct RadioDAT RadioDAT_1;

extern struct RadioDAT{
	uint8 srcsz;
	uint8 srcdat[100];
};

#endif /* SFUSAT_SFU_TASK_RADIO_H_ */
