/*
 * sfu_task_radio.h
 *
 *  Created on: Apr 30, 2017
 *      Author: steven
 */

#ifndef SFUSAT_SFU_TASK_MAIN_H_
#define SFUSAT_SFU_TASK_MAIN_H_

#include "FreeRTOS.h"
#include "rtos_task.h"
#include "rtos_queue.h"

void vMainTask(void *pvParameters);

extern TaskHandle_t xSerialTaskHandle;
extern TaskHandle_t xRadioTaskHandle;

extern TaskHandle_t xBlinkyTaskHandle;

#endif /* SFUSAT_SFU_TASK_MAIN_H_ */