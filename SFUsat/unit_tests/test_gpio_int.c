/*
 * test_gpio_int.c
 *
 *  Created on: Feb 19, 2018
 *      Author: Richard
 *
 *      This test will demonstrate GPIO interrupt functionality and how to integrate the RTOS with it.
 */

#include "gio.h"
#include "unit_tests.h"
#include "sfu_hardwaredefs.h"
#include "FreeRTOS.h"
#include "rtos_task.h"
#include "rtos_queue.h"
#include "rtos_semphr.h"
#include "sfu_tasks.h"

// TMS570 > Driver Enable > Enable GIO Driver
// Pinmux > Enable the pin
// GIO > Port A > Enable VIM low priority and rising edge
// TMS570 > VIM Channel 0-31 > Enable channel 23

void gio_int_test(void){
	gioEnableNotification(RF_IRQ_PORT, RF_IRQ_PIN);
}

/* USING THIS EXAMPLE
 * - In sfu_notification.c, run gio_notification_example() as the only thing in the function
 * - Enable interrupts
 * - Call gio_interrupt_example_rtos_init()
 * - Start the scheduler
 * - On the rising edge, the task will fire and print something out over the UART
 */

void gio_notification_example(gioPORT_t *port, uint32 bit){
	// This gets called by the ISR callback (notification). Doesn't need to be in another function, but this just keeps things tidy for the example
	// See FreeRTOS tutorial guide pg. 200
	BaseType_t xHigherPriorityTaskWoken;
	xHigherPriorityTaskWoken = pdFALSE;

	xSemaphoreGiveFromISR(gioExampleSem, &xHigherPriorityTaskWoken);
	portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}
void gio_interrupt_example_rtos_init(void){
	gioExampleSem = xSemaphoreCreateBinary();
	xGioInterruptTaskHandle = NULL;

	if(gioExampleSem != NULL){ // setup the task to handle the ISR
		xTaskCreate(vGioInterruptTask, "GIO Interrupt", 200, NULL, 3, xGioInterruptTaskHandle);
	}

	gioEnableNotification(RF_IRQ_PORT, RF_IRQ_PIN); // enable the notification callback for this pin
}

void vGioInterruptTask(void *pvParameters){
	// This task gets invoked by the ISR callback (notification)
	while(1){
		xSemaphoreTake(gioExampleSem, portMAX_DELAY);
		serialSendQ("ISR Fired!");
		gioSetBit(DEBUG_LED_PORT, DEBUG_LED_PIN, gioGetBit(DEBUG_LED_PORT, DEBUG_LED_PIN) ^ 1);   // Toggles the pin

	}
}


