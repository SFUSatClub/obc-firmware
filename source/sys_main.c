/** @file sys_main.c 
 *   @brief Application main file
 *   @date 05-Oct-2016
 *   @version 04.06.00
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
#include "FreeRTOS.h"
#include "rtos_task.h"
#include "rtos_queue.h"
#include "gio.h"
#include "SFU_Serial.h"
#include "SFUTasks.h"
/* USER CODE END */

/* Include Files */

/* USER CODE BEGIN (1) */


void vApplicationStackOverflowHook( TaskHandle_t pxTask, char *pcTaskName )
{
    ( void ) pcTaskName;
    ( void ) pxTask;
    serialSendln("Stack overflowwww");

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
    _enable_IRQ(); // global interrupt enable
    serialInit(); // SFU Serial

    gioInit();
    serialSendln("Hello!");
    serialSendln("Hello 2!");

    xTaskCreate( hundredBlinky, /* Pointer to the function that implements the task. */
                 "100 Hz Blinky",/* Text name for the task. This is to facilitate debugging only. */
                 configMINIMAL_STACK_SIZE, /* Stack depth - small microcontrollers will use much less stack than this. */
                 NULL, /* This example does not use the task parameter. */
                 1, /* This task will run at priority 1. */
                 NULL ); /* This example does not use the task handle. */
    serialSendln("created hundred blnky");
   xQueue = xQueueCreate( 5, sizeof( char * ) );
   serialSendln("created queue");
//    if( xQueue != NULL )
//    {
//        /* Create two instances of the task that will send to the queue. The task
//    parameter is used to pass the value that the task will write to the queue,
//   In this case, a string (character pointer) will be passed to the queue.
//   */
//
      xTaskCreate(periodicSenderTask, "PST", 600, ( void * )  1000, 1, NULL);
      serialSendln("created pst");
//        xTaskCreate(periodicSenderTask, "INFREQUENT Sending Task", 300, ( void * )  5000, 1, NULL);
//
//        /* Create the task that will read from the queue. The task is created with
//    priority 2, so above the priority of the sender tasks. */
     //xTaskCreate( vReceiverTask, "Receiver", 800, NULL, 2, NULL );
     serialSendln("created rcvr");
//        /* Start the scheduler so the created tasks start executing. */
//        vTaskStartScheduler();
//    }
//    else
//    {
//        /* The queue could not be created. */
//    }
    vTaskStartScheduler();

    for(;;); // keep running the scheduler

    /* USER CODE END */

    //  return 0;
}


/* USER CODE BEGIN (4) */



/* USER CODE END */
