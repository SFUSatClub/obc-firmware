/*
 * sfusat_examples.h
 *
 *  Created on: Feb 19, 2018
 *      Author: Richard
 */

#ifndef SFUSAT_EXAMPLES_OBCSAT_EXAMPLES_H_
#define SFUSAT_EXAMPLES_OBCSAT_EXAMPLES_H_
#include "FreeRTOS.h"
#include "rtos_task.h"
#include "rtos_queue.h"
#include "rtos_semphr.h"
#include "rtos_portmacro.h"
#include "gio.h"


// GPIO Interrupt Example
void gio_interrupt_example_rtos_init(void); // standalone initializer for tasks and semaphore
void gio_notification_example(gioPORT_t *port, uint32 bit); // called in gionotification, raises semaphore to start the task
void vGioInterruptTask(void *pvParameters); // Task that runs when the interrupt fires
extern SemaphoreHandle_t gioExampleSem;
extern TaskHandle_t xGioInterruptTaskHandle;




#endif /* SFUSAT_EXAMPLES_OBCSAT_EXAMPLES_H_ */
