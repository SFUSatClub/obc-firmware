/** @file sys_main.c 
 *   @brief Application main file
 *   @date 07-July-2017
 *   @version 04.07.00
 *
 *   This file contains an empty main function,
 *   which can be used for the application.
 */

/* 
 * Copyright (C) 2009-2016 Texas Instruments Incorporated - www.ti.com
 *
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *    Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 *    Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the
 *    distribution.
 *
 *    Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

/* USER CODE BEGIN (0) */
//  ---------- TI/External System ----------
#include "sys_core.h"
#include "FreeRTOS.h"
#include "rtos_task.h"
#include "rtos_queue.h"
#if (configGENERATE_RUN_TIME_STATS == 1)
#include "sys_pmu.h"
#endif

// ---------- TI/External Hardware ----------
#include "adc.h"
#include "gio.h"
#include "mibspi.h"

//  ---------- SFUSat Hardware ----------
#include "sfu_spi.h"
#include "sfu_uart.h"
#include "flash_mibspi.h"

//  ---------- SFUSat System ----------
#include "sfu_startup.h"
#include "sfu_tasks.h"
#include "sfu_state.h"
#include "sfu_utils.h"
#include "sfu_rtc.h"

//  ---------- SFUSat Tests (optional) ----------
#include "unit_tests/unit_tests.h"
#include "sfusat_spiffs.h"
#include "printf.h"

/* USER CODE END */

/* Include Files */

#include "sys_common.h"

/* USER CODE BEGIN (1) */

void vApplicationStackOverflowHook(TaskHandle_t pxTask, signed char *pcTaskName) {
	(void) pcTaskName;
	(void) pxTask;
	serialSend((char *) pcTaskName);
	serialSendln(" has overflowed");

	/* Run time stack overflow checking is performed if
	 configCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2.  This hook
	 function is called if a stack overflow is detected. */
	taskDISABLE_INTERRUPTS();
	for (;;)
		;
}

/* USER CODE END */

/** @fn void main(void)
 *   @brief Application main function
 *   @note This function is empty by default.
 *
 *   This function is called after startup.
 *   The user can use this function to implement the application.
 */

/* USER CODE BEGIN (2) */

/* USER CODE END */

int main(void) {
	/* USER CODE BEGIN (3) */
// ---------- SETUP/INIT HARDWARE ----------
	_enable_IRQ(); // global interrupt enable
	_enable_interrupt_();

	serialInit();
	gioInit();
	adcInit();
	spiInit();
	flash_mibspi_init();

// ---------- SFUSat INIT ----------
	rtcInit();
	stateMachineInit(); // we start in SAFE mode

// ---------- BRINGUP/PRELIMINARY PHASE ----------
	printf("SFUSat Started!");

//	watchdog_busywait(3000); // to allow time for serial to connect up to script
	simpleWatchdog(); // do this just to be sure we hit the watchdog before entering RTOS
	printStartupType();

// ---------- INIT TESTS ----------
	// TODO: if tests fail, actually do something
	// Also, we can't actually run some of these tests in the future. They erase the flash, for example
//	test_flash();
	init_adc_test();
//    triumf_init();
	flash_erase_chip();
//    sfusat_spiffs_init();
//
////	my_spiffs_mount();
//   test_spiffs();
//    read_write_example();

// ---------- INIT RTOS FEATURES ----------
	// TODO: encapsulate these
//  xQueue = xQueueCreate(5, sizeof(char *));
	xSerialTXQueue = xQueueCreate(50, sizeof(portCHAR *));
	xSerialRXQueue = xQueueCreate(50, sizeof(portCHAR));
	serialSendQ("created queue");

	xRTCMutex = xSemaphoreCreateMutex();

// ---------- SETUP/START RTOS ----------
	// vMainTask starts up all of the top level tasks. From those, other tasks are spawned as necessary.
	xTaskCreate(vMainTask, "main", 1000, NULL, MAIN_TASK_PRIORITY, NULL);

	vTaskStartScheduler();

	while (1) {
		// keep running the scheduler
	}

	/* USER CODE END */

	return 0;
}

/* USER CODE BEGIN (4) */

#if (configGENERATE_RUN_TIME_STATS == 1)
BaseType_t getRunTimeCounterValue() {
	return _pmuGetEventCount_(pmuCOUNTER0);
}
void configureTimerForRunTimeStats() {
	_pmuInit_();
	_pmuEnableCountersGlobal_();
	_pmuSetCountEvent_(pmuCOUNTER0, PMU_CYCLE_COUNT);
	_pmuSetCountEvent_(pmuCOUNTER1, PMU_CYCLE_COUNT);
	_pmuStartCounters_(pmuCOUNTER0 | pmuCOUNTER1 | pmuCOUNTER2);
}
#endif
/* USER CODE END */
