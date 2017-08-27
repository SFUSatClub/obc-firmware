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

#include "sfu_spi.h"

void vRadioTask( void *pvParameters );
BaseType_t initRadio();

extern QueueHandle_t xRadioTXQueue;
extern QueueHandle_t xRadioRXQueue;

#endif /* SFUSAT_SFU_TASK_RADIO_H_ */
