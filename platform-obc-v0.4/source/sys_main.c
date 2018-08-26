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
#include "FreeRTOS.h"
#include "rtos_task.h"
#include "sys_core.h"
#if (configGENERATE_RUN_TIME_STATS == 1)
#include "sys_pmu.h"
#endif

//  ---------- SFUSat Hardware ----------
#include <obc_uart.h>

//  ---------- SFUSat System ----------
#include <obc_tasks.h>
#include <obc_utils.h>

/* USER CODE END */

/* Include Files */

#include "sys_common.h"

/* USER CODE BEGIN (1) */

void vApplicationStackOverflowHook( TaskHandle_t pxTask, signed char *pcTaskName )
{
	( void ) pcTaskName;
	( void ) pxTask;
	serialSend((char *)pcTaskName);
	serialSendln(" has overflowed");

	/* Run time stack overflow checking is performed if
    configCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2.  This hook
    function is called if a stack overflow is detected. */
	taskDISABLE_INTERRUPTS();
	for( ;; );
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

int main(void)
{
/* USER CODE BEGIN (3) */

	/*
	 * Enable interrupts.
	 */
	_enable_IRQ();
    _enable_interrupt_();

    /**
     * Task vMainTask is where all of the top level tasks will be created from.
     *
     * vMaintask is created with portPRIVILEGE_BIT set in order for the processor to
     * remain in Privileged mode. The Memory Protection Unit (MPU) will therefore be
     * disabled for this task, allowing it to do things such as, e.g., configuring the
     * system's peripherals.
     *
     * https://www.freertos.org/FreeRTOS-MPU-memory-protection-unit.html
     */
    xTaskCreate(vMainTask, "main", 1000, NULL, portPRIVILEGE_BIT | MAIN_TASK_PRIORITY, NULL);

    /**
     * Start the FreeRTOS scheduler.
     */
	vTaskStartScheduler();

	/*
	 * Keep running the scheduler.
	 */
	while(1) {}

	/**
	 * Push/pop the following pragmas to prevent them from having an effect anywhere else.
	 *
	 * Suppress diag 112 "statement is unreachable" since Halcogen always generates the
	 * return statement below outside of our control (outside of USER CODE segments).
	 */
#pragma diag_push
#pragma diag_suppress 112
	/* USER CODE END */

    return 0;
}


/* USER CODE BEGIN (4) */

/**
 * Restore warnings.
 */
#pragma diag_pop

#if (configGENERATE_RUN_TIME_STATS == 1)
BaseType_t getRunTimeCounterValue() {
	return _pmuGetEventCount_(pmuCOUNTER0);
}
void configureTimerForRunTimeStats() {
	_pmuInit_();
	_pmuEnableCountersGlobal_();
	_pmuSetCountEvent_(pmuCOUNTER0, PMU_CYCLE_COUNT);
	_pmuSetCountEvent_(pmuCOUNTER1, PMU_CYCLE_COUNT);
	_pmuStartCounters_(pmuCOUNTER0|pmuCOUNTER1|pmuCOUNTER2);
}
#endif
/* USER CODE END */
