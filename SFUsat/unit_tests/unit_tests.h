/*
 * unit_tests.h
 *
 *  Created on: Jan 29, 2018
 *      Author: Richard
 */

#ifndef SFUSAT_UNIT_TESTS_UNIT_TESTS_H_
#define SFUSAT_UNIT_TESTS_UNIT_TESTS_H_
#include "sys_common.h"
#include "freeRTOS.h"
#include "rtos_task.h"
#include "rtos_queue.h"
#include "rtos_semphr.h"
#include "rtos_portmacro.h"
#include "gio.h"

// ADC Tests
void init_adc_test();
uint32_t test_adc(uint8_t channel);

// Flash test
uint32_t test_flash(void);
bool check16(uint32_t address, uint8_t *matchBuf);
bool checkArbitrary(uint32_t start_address, uint32_t size, uint8_t *matchBuf);

// GPIO Interrupt Example
void gio_int_test(void);
void gio_interrupt_example_rtos_init(void);
void gio_notification_example(gioPORT_t *port, uint32 bit);
TaskHandle_t xGioInterruptTaskHandle;
void vGioInterruptTask(void *pvParameters);
SemaphoreHandle_t gioExampleSem;






#endif /* SFUSAT_UNIT_TESTS_UNIT_TESTS_H_ */
