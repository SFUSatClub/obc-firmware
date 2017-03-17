/*
    FreeRTOS V9.0.0 - Copyright (C) 2016 Real Time Engineers Ltd.
    All rights reserved

    VISIT http://www.FreeRTOS.org TO ENSURE YOU ARE USING THE LATEST VERSION.

    This file is part of the FreeRTOS distribution.

    FreeRTOS is free software; you can redistribute it and/or modify it under
    the terms of the GNU General Public License (version 2) as published by the
    Free Software Foundation >>>> AND MODIFIED BY <<<< the FreeRTOS exception.

    ***************************************************************************
    >>!   NOTE: The modification to the GPL is included to allow you to     !<<
    >>!   distribute a combined work that includes FreeRTOS without being   !<<
    >>!   obliged to provide the source code for proprietary components     !<<
    >>!   outside of the FreeRTOS kernel.                                   !<<
    ***************************************************************************

    FreeRTOS is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  Full license text is available on the following
    link: http://www.freertos.org/a00114.html

    ***************************************************************************
     *                                                                       *
     *    FreeRTOS provides completely free yet professionally developed,    *
     *    robust, strictly quality controlled, supported, and cross          *
     *    platform software that is more than just the market leader, it     *
     *    is the industry's de facto standard.                               *
     *                                                                       *
     *    Help yourself get started quickly while simultaneously helping     *
     *    to support the FreeRTOS project by purchasing a FreeRTOS           *
     *    tutorial book, reference manual, or both:                          *
     *    http://www.FreeRTOS.org/Documentation                              *
     *                                                                       *
    ***************************************************************************

    http://www.FreeRTOS.org/FAQHelp.html - Having a problem?  Start by reading
    the FAQ page "My application does not run, what could be wrong?".  Have you
    defined configASSERT()?

    http://www.FreeRTOS.org/support - In return for receiving this top quality
    embedded software for free we request you assist our global community by
    participating in the support forum.

    http://www.FreeRTOS.org/training - Investing in training allows your team to
    be as productive as possible as early as possible.  Now you can receive
    FreeRTOS training directly from Richard Barry, CEO of Real Time Engineers
    Ltd, and the world's leading authority on the world's leading RTOS.

    http://www.FreeRTOS.org/plus - A selection of FreeRTOS ecosystem products,
    including FreeRTOS+Trace - an indispensable productivity tool, a DOS
    compatible FAT file system, and our tiny thread aware UDP/IP stack.

    http://www.FreeRTOS.org/labs - Where new FreeRTOS products go to incubate.
    Come and try FreeRTOS+TCP, our new open source TCP/IP stack for FreeRTOS.

    http://www.OpenRTOS.com - Real Time Engineers ltd. license FreeRTOS to High
    Integrity Systems ltd. to sell under the OpenRTOS brand.  Low cost OpenRTOS
    licenses offer ticketed support, indemnification and commercial middleware.

    http://www.SafeRTOS.com - High Integrity Systems also provide a safety
    engineered and independently SIL3 certified version for use in safety and
    mission critical applications that require provable dependability.

    1 tab == 4 spaces!
*/

/*
 * Implementation of the wrapper functions used to raise the processor privilege
 * before calling a standard FreeRTOS API function.
 */

/* Defining MPU_WRAPPERS_INCLUDED_FROM_API_FILE prevents task.h from redefining
all the API functions to use the MPU wrappers.  That should only be done when
task.h is included from an application file. */
#define MPU_WRAPPERS_INCLUDED_FROM_API_FILE

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "rtos_task.h"
#include "rtos_queue.h"
#include "rtos_timer.h"
#include "rtos_event_groups.h"
//#include "os_mpu_prototypes.h"

#undef MPU_WRAPPERS_INCLUDED_FROM_API_FILE

#define portRESET_PRIVILEGE( xRunningPrivileged ) if( xRunningPrivileged == 0 ) portSWITCH_TO_USER_MODE() // Richard: put this in from 8.1
#define xQueueGetMutexHolder MPU_xQueueGetMutexHolder // Richard: added this. It's also in os_mpu_wrappers
/*
 * Checks to see if being called from the context of an unprivileged task, and
 * if so raises the privilege level and returns false - otherwise does nothing
 * other than return true.
 */
// Richard: originally called xPortRaisePrivilege
#pragma SWI_ALIAS(prvRaisePrivilege, 1);
extern BaseType_t prvRaisePrivilege( void );

/*-----------------------------------------------------------*/

BaseType_t MPU_xTaskCreateRestricted( const TaskParameters_t * const pxTaskDefinition, TaskHandle_t *pxCreatedTask )
{
BaseType_t xReturn;
BaseType_t xRunningPrivileged = prvRaisePrivilege();

    xReturn = xTaskCreateRestricted( pxTaskDefinition, pxCreatedTask );
    portRESET_PRIVILEGE( xRunningPrivileged );

    // RICHARD:
    // REPLACED vPortResetPrivilege with portRESET_PRIVILEGE on all lines.

    // portRESET_PRIVILEGE( xRunningPrivileged ); // Richard: removed and replaced with above
    return xReturn;
}
/*-----------------------------------------------------------*/

#if( configSUPPORT_DYNAMIC_ALLOCATION == 1 )
    BaseType_t MPU_xTaskCreate( TaskFunction_t pvTaskCode, const char * const pcName, uint16_t usStackDepth, void *pvParameters, UBaseType_t uxPriority, TaskHandle_t *pxCreatedTask )
    {
    BaseType_t xReturn;

    // prvRaisePrivilege();

    BaseType_t xRunningPrivileged = prvRaisePrivilege();

        xReturn = xTaskCreate( pvTaskCode, pcName, usStackDepth, pvParameters, uxPriority, pxCreatedTask );
        portRESET_PRIVILEGE( xRunningPrivileged );

//        portRESET_PRIVILEGE( xRunningPrivileged );
        return xReturn;
    }
#endif /* configSUPPORT_DYNAMIC_ALLOCATION */
/*-----------------------------------------------------------*/

#if( configSUPPORT_STATIC_ALLOCATION == 1 )
    TaskHandle_t MPU_xTaskCreateStatic( TaskFunction_t pxTaskCode, const char * const pcName, const uint32_t ulStackDepth, void * const pvParameters, UBaseType_t uxPriority, StackType_t * const puxStackBuffer, StaticTask_t * const pxTaskBuffer )
    {
    TaskHandle_t xReturn;
    BaseType_t xRunningPrivileged = prvRaisePrivilege();

        xReturn = xTaskCreateStatic( pxTaskCode, pcName, ulStackDepth, pvParameters, uxPriority, puxStackBuffer, pxTaskBuffer );
        portRESET_PRIVILEGE( xRunningPrivileged );
        return xReturn;
    }
#endif /* configSUPPORT_STATIC_ALLOCATION */
/*-----------------------------------------------------------*/

void MPU_vTaskAllocateMPURegions( TaskHandle_t xTask, const MemoryRegion_t * const xRegions )
{
BaseType_t xRunningPrivileged = prvRaisePrivilege();

    vTaskAllocateMPURegions( xTask, xRegions );
    portRESET_PRIVILEGE( xRunningPrivileged );
//    portRESET_PRIVILEGE( xRunningPrivileged );
}
/*-----------------------------------------------------------*/

#if ( INCLUDE_vTaskDelete == 1 )
    void MPU_vTaskDelete( TaskHandle_t pxTaskToDelete )
    {
    BaseType_t xRunningPrivileged = prvRaisePrivilege();

        vTaskDelete( pxTaskToDelete );
        portRESET_PRIVILEGE( xRunningPrivileged );

//        portRESET_PRIVILEGE( xRunningPrivileged );
    }
#endif
/*-----------------------------------------------------------*/

#if ( INCLUDE_vTaskDelayUntil == 1 )
    void MPU_vTaskDelayUntil( TickType_t * const pxPreviousWakeTime, TickType_t xTimeIncrement )
    {
    BaseType_t xRunningPrivileged = prvRaisePrivilege();

        vTaskDelayUntil( pxPreviousWakeTime, xTimeIncrement );
        portRESET_PRIVILEGE( xRunningPrivileged );

//        portRESET_PRIVILEGE( xRunningPrivileged );
    }
#endif
/*-----------------------------------------------------------*/

#if ( INCLUDE_xTaskAbortDelay == 1 )
    BaseType_t MPU_xTaskAbortDelay( TaskHandle_t xTask )
    {
    BaseType_t xReturn;
    BaseType_t xRunningPrivileged = prvRaisePrivilege();

        xReturn = xTaskAbortDelay( xTask );
        portRESET_PRIVILEGE( xRunningPrivileged );
        return xReturn;
    }
#endif
/*-----------------------------------------------------------*/

#if ( INCLUDE_vTaskDelay == 1 )
    void MPU_vTaskDelay( TickType_t xTicksToDelay )
    {
    BaseType_t xRunningPrivileged = prvRaisePrivilege();

        vTaskDelay( xTicksToDelay );
        portRESET_PRIVILEGE( xRunningPrivileged );

//        portRESET_PRIVILEGE( xRunningPrivileged );
    }
#endif
/*-----------------------------------------------------------*/

#if ( INCLUDE_uxTaskPriorityGet == 1 )
    UBaseType_t MPU_uxTaskPriorityGet( TaskHandle_t pxTask )
    {
    UBaseType_t uxReturn;
    BaseType_t xRunningPrivileged = prvRaisePrivilege();

        uxReturn = uxTaskPriorityGet( pxTask );
        portRESET_PRIVILEGE( xRunningPrivileged );

//        portRESET_PRIVILEGE( xRunningPrivileged );
        return uxReturn;
    }
#endif
/*-----------------------------------------------------------*/

#if ( INCLUDE_vTaskPrioritySet == 1 )
    void MPU_vTaskPrioritySet( TaskHandle_t pxTask, UBaseType_t uxNewPriority )
    {
    BaseType_t xRunningPrivileged = prvRaisePrivilege();

        vTaskPrioritySet( pxTask, uxNewPriority );
        portRESET_PRIVILEGE( xRunningPrivileged );
    }
#endif
/*-----------------------------------------------------------*/

#if ( INCLUDE_eTaskGetState == 1 )
    eTaskState MPU_eTaskGetState( TaskHandle_t pxTask )
    {
    BaseType_t xRunningPrivileged = prvRaisePrivilege();
    eTaskState eReturn;

        eReturn = eTaskGetState( pxTask );
        portRESET_PRIVILEGE( xRunningPrivileged );
        return eReturn;
    }
#endif
/*-----------------------------------------------------------*/

#if( configUSE_TRACE_FACILITY == 1 )
    void MPU_vTaskGetInfo( TaskHandle_t xTask, TaskStatus_t *pxTaskStatus, BaseType_t xGetFreeStackSpace, eTaskState eState )
    {
    BaseType_t xRunningPrivileged = prvRaisePrivilege();

        vTaskGetInfo( xTask, pxTaskStatus, xGetFreeStackSpace, eState );
        portRESET_PRIVILEGE( xRunningPrivileged );
    }
#endif
/*-----------------------------------------------------------*/

#if ( INCLUDE_xTaskGetIdleTaskHandle == 1 )
    TaskHandle_t MPU_xTaskGetIdleTaskHandle( void )
    {
    TaskHandle_t xReturn;
    BaseType_t xRunningPrivileged = prvRaisePrivilege();

        xReturn = xTaskGetIdleTaskHandle();
        portRESET_PRIVILEGE( xRunningPrivileged );
        return xReturn;
    }
#endif
/*-----------------------------------------------------------*/

#if ( INCLUDE_vTaskSuspend == 1 )
    void MPU_vTaskSuspend( TaskHandle_t pxTaskToSuspend )
    {
    BaseType_t xRunningPrivileged = prvRaisePrivilege();

        vTaskSuspend( pxTaskToSuspend );
        portRESET_PRIVILEGE( xRunningPrivileged );
    }
#endif
/*-----------------------------------------------------------*/

#if ( INCLUDE_vTaskSuspend == 1 )
    void MPU_vTaskResume( TaskHandle_t pxTaskToResume )
    {
    BaseType_t xRunningPrivileged = prvRaisePrivilege();

        vTaskResume( pxTaskToResume );
        portRESET_PRIVILEGE( xRunningPrivileged );
    }
#endif
/*-----------------------------------------------------------*/

void MPU_vTaskSuspendAll( void )
{
BaseType_t xRunningPrivileged = prvRaisePrivilege();

    vTaskSuspendAll();
    portRESET_PRIVILEGE( xRunningPrivileged );
}
/*-----------------------------------------------------------*/

BaseType_t MPU_xTaskResumeAll( void )
{
BaseType_t xReturn;
BaseType_t xRunningPrivileged = prvRaisePrivilege();

    xReturn = xTaskResumeAll();
    portRESET_PRIVILEGE( xRunningPrivileged );
    return xReturn;
}
/*-----------------------------------------------------------*/

TickType_t MPU_xTaskGetTickCount( void )
{
TickType_t xReturn;
BaseType_t xRunningPrivileged = prvRaisePrivilege();

    xReturn = xTaskGetTickCount();
    portRESET_PRIVILEGE( xRunningPrivileged );
    return xReturn;
}
/*-----------------------------------------------------------*/

UBaseType_t MPU_uxTaskGetNumberOfTasks( void )
{
UBaseType_t uxReturn;
BaseType_t xRunningPrivileged = prvRaisePrivilege();

    uxReturn = uxTaskGetNumberOfTasks();
    portRESET_PRIVILEGE( xRunningPrivileged );
    return uxReturn;
}
/*-----------------------------------------------------------*/

char * MPU_pcTaskGetName( TaskHandle_t xTaskToQuery )
{
char *pcReturn;
BaseType_t xRunningPrivileged = prvRaisePrivilege();

    pcReturn = pcTaskGetName( xTaskToQuery );
    portRESET_PRIVILEGE( xRunningPrivileged );
    return pcReturn;
}
/*-----------------------------------------------------------*/

#if ( INCLUDE_xTaskGetHandle == 1 )
    TaskHandle_t MPU_xTaskGetHandle( const char *pcNameToQuery )
    {
    TaskHandle_t xReturn;
    BaseType_t xRunningPrivileged = prvRaisePrivilege();

        xReturn = xTaskGetHandle( pcNameToQuery );
        portRESET_PRIVILEGE( xRunningPrivileged );
        return xReturn;
    }
#endif
/*-----------------------------------------------------------*/

#if ( ( configUSE_TRACE_FACILITY == 1 ) && ( configUSE_STATS_FORMATTING_FUNCTIONS > 0 ) )
    void MPU_vTaskList( char *pcWriteBuffer )
    {
    BaseType_t xRunningPrivileged = prvRaisePrivilege();

        vTaskList( pcWriteBuffer );
        portRESET_PRIVILEGE( xRunningPrivileged );
    }
#endif
/*-----------------------------------------------------------*/

#if ( ( configGENERATE_RUN_TIME_STATS == 1 ) && ( configUSE_STATS_FORMATTING_FUNCTIONS > 0 ) )
    void MPU_vTaskGetRunTimeStats( char *pcWriteBuffer )
    {
    BaseType_t xRunningPrivileged = prvRaisePrivilege();

        vTaskGetRunTimeStats( pcWriteBuffer );
        portRESET_PRIVILEGE( xRunningPrivileged );
    }
#endif
/*-----------------------------------------------------------*/

#if ( configUSE_APPLICATION_TASK_TAG == 1 )
    void MPU_vTaskSetApplicationTaskTag( TaskHandle_t xTask, TaskHookFunction_t pxTagValue )
    {
    BaseType_t xRunningPrivileged = prvRaisePrivilege();

        vTaskSetApplicationTaskTag( xTask, pxTagValue );
        portRESET_PRIVILEGE( xRunningPrivileged );
    }
#endif
/*-----------------------------------------------------------*/

#if ( configUSE_APPLICATION_TASK_TAG == 1 )
    TaskHookFunction_t MPU_xTaskGetApplicationTaskTag( TaskHandle_t xTask )
    {
    TaskHookFunction_t xReturn;
    BaseType_t xRunningPrivileged = prvRaisePrivilege();

        xReturn = xTaskGetApplicationTaskTag( xTask );
        portRESET_PRIVILEGE( xRunningPrivileged );
        return xReturn;
    }
#endif
/*-----------------------------------------------------------*/

#if ( configNUM_THREAD_LOCAL_STORAGE_POINTERS != 0 )
    void MPU_vTaskSetThreadLocalStoragePointer( TaskHandle_t xTaskToSet, BaseType_t xIndex, void *pvValue )
    {
    BaseType_t xRunningPrivileged = prvRaisePrivilege();

        vTaskSetThreadLocalStoragePointer( xTaskToSet, xIndex, pvValue );
        portRESET_PRIVILEGE( xRunningPrivileged );
    }
#endif
/*-----------------------------------------------------------*/

#if ( configNUM_THREAD_LOCAL_STORAGE_POINTERS != 0 )
    void *MPU_pvTaskGetThreadLocalStoragePointer( TaskHandle_t xTaskToQuery, BaseType_t xIndex )
    {
    void *pvReturn;
    BaseType_t xRunningPrivileged = prvRaisePrivilege();

        pvReturn = pvTaskGetThreadLocalStoragePointer( xTaskToQuery, xIndex );
        portRESET_PRIVILEGE( xRunningPrivileged );
        return pvReturn;
    }
#endif
/*-----------------------------------------------------------*/

#if ( configUSE_APPLICATION_TASK_TAG == 1 )
    BaseType_t MPU_xTaskCallApplicationTaskHook( TaskHandle_t xTask, void *pvParameter )
    {
    BaseType_t xReturn;
    BaseType_t xRunningPrivileged = prvRaisePrivilege();

        xReturn = xTaskCallApplicationTaskHook( xTask, pvParameter );
        portRESET_PRIVILEGE( xRunningPrivileged );
        return xReturn;
    }
#endif
/*-----------------------------------------------------------*/

#if ( configUSE_TRACE_FACILITY == 1 )
    UBaseType_t MPU_uxTaskGetSystemState( TaskStatus_t *pxTaskStatusArray, UBaseType_t uxArraySize, uint32_t *pulTotalRunTime )
    {
    UBaseType_t uxReturn;
    BaseType_t xRunningPrivileged = prvRaisePrivilege();

        uxReturn = uxTaskGetSystemState( pxTaskStatusArray, uxArraySize, pulTotalRunTime );
        portRESET_PRIVILEGE( xRunningPrivileged );
        return uxReturn;
    }
#endif
/*-----------------------------------------------------------*/

#if ( INCLUDE_uxTaskGetStackHighWaterMark == 1 )
    UBaseType_t MPU_uxTaskGetStackHighWaterMark( TaskHandle_t xTask )
    {
    UBaseType_t uxReturn;
    BaseType_t xRunningPrivileged = prvRaisePrivilege();

        uxReturn = uxTaskGetStackHighWaterMark( xTask );
        portRESET_PRIVILEGE( xRunningPrivileged );
        return uxReturn;
    }
#endif
/*-----------------------------------------------------------*/

#if ( INCLUDE_xTaskGetCurrentTaskHandle == 1 )
    TaskHandle_t MPU_xTaskGetCurrentTaskHandle( void )
    {
    TaskHandle_t xReturn;
    BaseType_t xRunningPrivileged = prvRaisePrivilege();

        xReturn = xTaskGetCurrentTaskHandle();
        portRESET_PRIVILEGE( xRunningPrivileged );
        return xReturn;
    }
#endif
/*-----------------------------------------------------------*/

#if ( INCLUDE_xTaskGetSchedulerState == 1 )
    BaseType_t MPU_xTaskGetSchedulerState( void )
    {
    BaseType_t xReturn;
    BaseType_t xRunningPrivileged = prvRaisePrivilege();

        xReturn = xTaskGetSchedulerState();
        portRESET_PRIVILEGE( xRunningPrivileged );
        return xReturn;
    }
#endif
/*-----------------------------------------------------------*/

void MPU_vTaskSetTimeOutState( TimeOut_t * const pxTimeOut )
{
BaseType_t xRunningPrivileged = prvRaisePrivilege();

    vTaskSetTimeOutState( pxTimeOut );
    portRESET_PRIVILEGE( xRunningPrivileged );
}
/*-----------------------------------------------------------*/

BaseType_t MPU_xTaskCheckForTimeOut( TimeOut_t * const pxTimeOut, TickType_t * const pxTicksToWait )
{
BaseType_t xReturn;
BaseType_t xRunningPrivileged = prvRaisePrivilege();

    xReturn = xTaskCheckForTimeOut( pxTimeOut, pxTicksToWait );
    portRESET_PRIVILEGE( xRunningPrivileged );
    return xReturn;
}
/*-----------------------------------------------------------*/

#if( configUSE_TASK_NOTIFICATIONS == 1 )
    BaseType_t MPU_xTaskGenericNotify( TaskHandle_t xTaskToNotify, uint32_t ulValue, eNotifyAction eAction, uint32_t *pulPreviousNotificationValue )
    {
    BaseType_t xReturn;
    BaseType_t xRunningPrivileged = prvRaisePrivilege();

        xReturn = xTaskGenericNotify( xTaskToNotify, ulValue, eAction, pulPreviousNotificationValue );
        portRESET_PRIVILEGE( xRunningPrivileged );
        return xReturn;
    }
#endif
/*-----------------------------------------------------------*/

#if( configUSE_TASK_NOTIFICATIONS == 1 )
    BaseType_t MPU_xTaskNotifyWait( uint32_t ulBitsToClearOnEntry, uint32_t ulBitsToClearOnExit, uint32_t *pulNotificationValue, TickType_t xTicksToWait )
    {
    BaseType_t xReturn;
    BaseType_t xRunningPrivileged = prvRaisePrivilege();

        xReturn = xTaskNotifyWait( ulBitsToClearOnEntry, ulBitsToClearOnExit, pulNotificationValue, xTicksToWait );
        portRESET_PRIVILEGE( xRunningPrivileged );
        return xReturn;
    }
#endif
/*-----------------------------------------------------------*/

#if( configUSE_TASK_NOTIFICATIONS == 1 )
    uint32_t MPU_ulTaskNotifyTake( BaseType_t xClearCountOnExit, TickType_t xTicksToWait )
    {
    uint32_t ulReturn;
    BaseType_t xRunningPrivileged = prvRaisePrivilege();

        ulReturn = ulTaskNotifyTake( xClearCountOnExit, xTicksToWait );
        portRESET_PRIVILEGE( xRunningPrivileged );
        return ulReturn;
    }
#endif
/*-----------------------------------------------------------*/

#if( configUSE_TASK_NOTIFICATIONS == 1 )
    BaseType_t MPU_xTaskNotifyStateClear( TaskHandle_t xTask )
    {
    BaseType_t xReturn;
    BaseType_t xRunningPrivileged = prvRaisePrivilege();

        xReturn = xTaskNotifyStateClear( xTask );
        portRESET_PRIVILEGE( xRunningPrivileged );
        return xReturn;
    }
#endif
/*-----------------------------------------------------------*/

#if( configSUPPORT_DYNAMIC_ALLOCATION == 1 )
    QueueHandle_t MPU_xQueueGenericCreate( UBaseType_t uxQueueLength, UBaseType_t uxItemSize, uint8_t ucQueueType )
    {
    QueueHandle_t xReturn;
    BaseType_t xRunningPrivileged = prvRaisePrivilege();

        xReturn = xQueueGenericCreate( uxQueueLength, uxItemSize, ucQueueType );
        portRESET_PRIVILEGE( xRunningPrivileged );
        return xReturn;
    }
#endif
/*-----------------------------------------------------------*/

#if( configSUPPORT_STATIC_ALLOCATION == 1 )
    QueueHandle_t MPU_xQueueGenericCreateStatic( const UBaseType_t uxQueueLength, const UBaseType_t uxItemSize, uint8_t *pucQueueStorage, StaticQueue_t *pxStaticQueue, const uint8_t ucQueueType )
    {
    QueueHandle_t xReturn;
    BaseType_t xRunningPrivileged = prvRaisePrivilege();

        xReturn = xQueueGenericCreateStatic( uxQueueLength, uxItemSize, pucQueueStorage, pxStaticQueue, ucQueueType );
        portRESET_PRIVILEGE( xRunningPrivileged );
        return xReturn;
    }
#endif
/*-----------------------------------------------------------*/

BaseType_t MPU_xQueueGenericReset( QueueHandle_t pxQueue, BaseType_t xNewQueue )
{
BaseType_t xReturn;
BaseType_t xRunningPrivileged = prvRaisePrivilege();

    xReturn = xQueueGenericReset( pxQueue, xNewQueue );
    portRESET_PRIVILEGE( xRunningPrivileged );
    return xReturn;
}
/*-----------------------------------------------------------*/

BaseType_t MPU_xQueueGenericSend( QueueHandle_t xQueue, const void * const pvItemToQueue, TickType_t xTicksToWait, BaseType_t xCopyPosition )
{
BaseType_t xReturn;
BaseType_t xRunningPrivileged = prvRaisePrivilege();

    xReturn = xQueueGenericSend( xQueue, pvItemToQueue, xTicksToWait, xCopyPosition );
    portRESET_PRIVILEGE( xRunningPrivileged );
    return xReturn;
}
/*-----------------------------------------------------------*/

UBaseType_t MPU_uxQueueMessagesWaiting( const QueueHandle_t pxQueue )
{
BaseType_t xRunningPrivileged = prvRaisePrivilege();
UBaseType_t uxReturn;

    uxReturn = uxQueueMessagesWaiting( pxQueue );
    portRESET_PRIVILEGE( xRunningPrivileged );
    return uxReturn;
}
/*-----------------------------------------------------------*/

UBaseType_t MPU_uxQueueSpacesAvailable( const QueueHandle_t xQueue )
{
BaseType_t xRunningPrivileged = prvRaisePrivilege();
UBaseType_t uxReturn;

    uxReturn = uxQueueSpacesAvailable( xQueue );
    portRESET_PRIVILEGE( xRunningPrivileged );
    return uxReturn;
}
/*-----------------------------------------------------------*/

BaseType_t MPU_xQueueGenericReceive( QueueHandle_t pxQueue, void * const pvBuffer, TickType_t xTicksToWait, BaseType_t xJustPeeking )
{
BaseType_t xRunningPrivileged = prvRaisePrivilege();
BaseType_t xReturn;

    xReturn = xQueueGenericReceive( pxQueue, pvBuffer, xTicksToWait, xJustPeeking );
    portRESET_PRIVILEGE( xRunningPrivileged );
    return xReturn;
}
/*-----------------------------------------------------------*/

BaseType_t MPU_xQueuePeekFromISR( QueueHandle_t pxQueue, void * const pvBuffer )
{
BaseType_t xRunningPrivileged = prvRaisePrivilege();
BaseType_t xReturn;

    xReturn = xQueuePeekFromISR( pxQueue, pvBuffer );
    portRESET_PRIVILEGE( xRunningPrivileged );
    return xReturn;
}
/*-----------------------------------------------------------*/

void* MPU_xQueueGetMutexHolder( QueueHandle_t xSemaphore )
{
BaseType_t xRunningPrivileged = prvRaisePrivilege();
void * xReturn;

    xReturn = ( void * ) xQueueGetMutexHolder( xSemaphore );
    portRESET_PRIVILEGE( xRunningPrivileged );
    return xReturn;
}
/*-----------------------------------------------------------*/

#if( ( configUSE_MUTEXES == 1 ) && ( configSUPPORT_DYNAMIC_ALLOCATION == 1 ) )
    QueueHandle_t MPU_xQueueCreateMutex( const uint8_t ucQueueType )
    {
    QueueHandle_t xReturn;
    BaseType_t xRunningPrivileged = prvRaisePrivilege();

        xReturn = xQueueCreateMutex( ucQueueType );
        portRESET_PRIVILEGE( xRunningPrivileged );
        return xReturn;
    }
#endif
/*-----------------------------------------------------------*/

#if( ( configUSE_MUTEXES == 1 ) && ( configSUPPORT_STATIC_ALLOCATION == 1 ) )
    QueueHandle_t MPU_xQueueCreateMutexStatic( const uint8_t ucQueueType, StaticQueue_t *pxStaticQueue )
    {
    QueueHandle_t xReturn;
    BaseType_t xRunningPrivileged = prvRaisePrivilege();

        xReturn = xQueueCreateMutexStatic( ucQueueType, pxStaticQueue );
        portRESET_PRIVILEGE( xRunningPrivileged );
        return xReturn;
    }
#endif
/*-----------------------------------------------------------*/

#if( ( configUSE_COUNTING_SEMAPHORES == 1 ) && ( configSUPPORT_DYNAMIC_ALLOCATION == 1 ) )
    QueueHandle_t MPU_xQueueCreateCountingSemaphore( UBaseType_t uxCountValue, UBaseType_t uxInitialCount )
    {
    QueueHandle_t xReturn;
    BaseType_t xRunningPrivileged = prvRaisePrivilege();

        xReturn = xQueueCreateCountingSemaphore( uxCountValue, uxInitialCount );
        portRESET_PRIVILEGE( xRunningPrivileged );
        return xReturn;
    }
#endif
/*-----------------------------------------------------------*/

#if( ( configUSE_COUNTING_SEMAPHORES == 1 ) && ( configSUPPORT_STATIC_ALLOCATION == 1 ) )

    QueueHandle_t MPU_xQueueCreateCountingSemaphoreStatic( const UBaseType_t uxMaxCount, const UBaseType_t uxInitialCount, StaticQueue_t *pxStaticQueue )
    {
    QueueHandle_t xReturn;
    BaseType_t xRunningPrivileged = prvRaisePrivilege();

        xReturn = xQueueCreateCountingSemaphoreStatic( uxMaxCount, uxInitialCount, pxStaticQueue );
        portRESET_PRIVILEGE( xRunningPrivileged );
        return xReturn;
    }
#endif
/*-----------------------------------------------------------*/

#if ( configUSE_MUTEXES == 1 )
    BaseType_t MPU_xQueueTakeMutexRecursive( QueueHandle_t xMutex, TickType_t xBlockTime )
    {
    BaseType_t xReturn;
    BaseType_t xRunningPrivileged = prvRaisePrivilege();

        xReturn = xQueueTakeMutexRecursive( xMutex, xBlockTime );
        portRESET_PRIVILEGE( xRunningPrivileged );
        return xReturn;
    }
#endif
/*-----------------------------------------------------------*/

#if ( configUSE_MUTEXES == 1 )
    BaseType_t MPU_xQueueGiveMutexRecursive( QueueHandle_t xMutex )
    {
    BaseType_t xReturn;
    BaseType_t xRunningPrivileged = prvRaisePrivilege();

        xReturn = xQueueGiveMutexRecursive( xMutex );
        portRESET_PRIVILEGE( xRunningPrivileged );
        return xReturn;
    }
#endif
/*-----------------------------------------------------------*/

#if ( configUSE_QUEUE_SETS == 1 )
    QueueSetHandle_t MPU_xQueueCreateSet( UBaseType_t uxEventQueueLength )
    {
    QueueSetHandle_t xReturn;
    BaseType_t xRunningPrivileged = prvRaisePrivilege();

        xReturn = xQueueCreateSet( uxEventQueueLength );
        portRESET_PRIVILEGE( xRunningPrivileged );
        return xReturn;
    }
#endif
/*-----------------------------------------------------------*/

#if ( configUSE_QUEUE_SETS == 1 )
    QueueSetMemberHandle_t MPU_xQueueSelectFromSet( QueueSetHandle_t xQueueSet, TickType_t xBlockTimeTicks )
    {
    QueueSetMemberHandle_t xReturn;
    BaseType_t xRunningPrivileged = prvRaisePrivilege();

        xReturn = xQueueSelectFromSet( xQueueSet, xBlockTimeTicks );
        portRESET_PRIVILEGE( xRunningPrivileged );
        return xReturn;
    }
#endif
/*-----------------------------------------------------------*/

#if ( configUSE_QUEUE_SETS == 1 )
    BaseType_t MPU_xQueueAddToSet( QueueSetMemberHandle_t xQueueOrSemaphore, QueueSetHandle_t xQueueSet )
    {
    BaseType_t xReturn;
    BaseType_t xRunningPrivileged = prvRaisePrivilege();

        xReturn = xQueueAddToSet( xQueueOrSemaphore, xQueueSet );
        portRESET_PRIVILEGE( xRunningPrivileged );
        return xReturn;
    }
#endif
/*-----------------------------------------------------------*/

#if ( configUSE_QUEUE_SETS == 1 )
    BaseType_t MPU_xQueueRemoveFromSet( QueueSetMemberHandle_t xQueueOrSemaphore, QueueSetHandle_t xQueueSet )
    {
    BaseType_t xReturn;
    BaseType_t xRunningPrivileged = prvRaisePrivilege();

        xReturn = xQueueRemoveFromSet( xQueueOrSemaphore, xQueueSet );
        portRESET_PRIVILEGE( xRunningPrivileged );
        return xReturn;
    }
#endif
/*-----------------------------------------------------------*/

#if configQUEUE_REGISTRY_SIZE > 0
    void MPU_vQueueAddToRegistry( QueueHandle_t xQueue, const char *pcName )
    {
    BaseType_t xRunningPrivileged = prvRaisePrivilege();

        vQueueAddToRegistry( xQueue, pcName );

        portRESET_PRIVILEGE( xRunningPrivileged );
    }
#endif
/*-----------------------------------------------------------*/

#if configQUEUE_REGISTRY_SIZE > 0
    void MPU_vQueueUnregisterQueue( QueueHandle_t xQueue )
    {
    BaseType_t xRunningPrivileged = prvRaisePrivilege();

        vQueueUnregisterQueue( xQueue );

        portRESET_PRIVILEGE( xRunningPrivileged );
    }
#endif
/*-----------------------------------------------------------*/

#if configQUEUE_REGISTRY_SIZE > 0
    const char *MPU_pcQueueGetName( QueueHandle_t xQueue )
    {
    BaseType_t xRunningPrivileged = prvRaisePrivilege();
    const char *pcReturn;

        pcReturn = pcQueueGetName( xQueue );

        portRESET_PRIVILEGE( xRunningPrivileged );
        return pcReturn;
    }
#endif
/*-----------------------------------------------------------*/

void MPU_vQueueDelete( QueueHandle_t xQueue )
{
BaseType_t xRunningPrivileged = prvRaisePrivilege();

    vQueueDelete( xQueue );

    portRESET_PRIVILEGE( xRunningPrivileged );
}
/*-----------------------------------------------------------*/

#if( configSUPPORT_DYNAMIC_ALLOCATION == 1 )

    void *MPU_pvPortMalloc( size_t xSize )
    {
    void *pvReturn;
    BaseType_t xRunningPrivileged = prvRaisePrivilege();

        pvReturn = pvPortMalloc( xSize );

        portRESET_PRIVILEGE( xRunningPrivileged );

        return pvReturn;
    }

#endif /* configSUPPORT_DYNAMIC_ALLOCATION */
/*-----------------------------------------------------------*/

#if( configSUPPORT_DYNAMIC_ALLOCATION == 1 )

    void MPU_vPortFree( void *pv )
    {
    BaseType_t xRunningPrivileged = prvRaisePrivilege();

        vPortFree( pv );

        portRESET_PRIVILEGE( xRunningPrivileged );
    }

#endif /* configSUPPORT_DYNAMIC_ALLOCATION */
/*-----------------------------------------------------------*/

void MPU_vPortInitialiseBlocks( void )
{
BaseType_t xRunningPrivileged = prvRaisePrivilege();

    vPortInitialiseBlocks();

    portRESET_PRIVILEGE( xRunningPrivileged );
}
/*-----------------------------------------------------------*/

size_t MPU_xPortGetFreeHeapSize( void )
{
size_t xReturn;
BaseType_t xRunningPrivileged = prvRaisePrivilege();

    xReturn = xPortGetFreeHeapSize();

    portRESET_PRIVILEGE( xRunningPrivileged );

    return xReturn;
}
/*-----------------------------------------------------------*/

#if( ( configSUPPORT_DYNAMIC_ALLOCATION == 1 ) && ( configUSE_TIMERS == 1 ) )
    TimerHandle_t MPU_xTimerCreate( const char * const pcTimerName, const TickType_t xTimerPeriodInTicks, const UBaseType_t uxAutoReload, void * const pvTimerID, TimerCallbackFunction_t pxCallbackFunction )
    {
    TimerHandle_t xReturn;
    BaseType_t xRunningPrivileged = prvRaisePrivilege();

        xReturn = xTimerCreate( pcTimerName, xTimerPeriodInTicks, uxAutoReload, pvTimerID, pxCallbackFunction );
        portRESET_PRIVILEGE( xRunningPrivileged );

        return xReturn;
    }
#endif
/*-----------------------------------------------------------*/

#if( ( configSUPPORT_STATIC_ALLOCATION == 1 ) && ( configUSE_TIMERS == 1 ) )
    TimerHandle_t MPU_xTimerCreateStatic( const char * const pcTimerName, const TickType_t xTimerPeriodInTicks, const UBaseType_t uxAutoReload, void * const pvTimerID, TimerCallbackFunction_t pxCallbackFunction, StaticTimer_t *pxTimerBuffer )
    {
    TimerHandle_t xReturn;
    BaseType_t xRunningPrivileged = prvRaisePrivilege();

        xReturn = xTimerCreateStatic( pcTimerName, xTimerPeriodInTicks, uxAutoReload, pvTimerID, pxCallbackFunction, pxTimerBuffer );
        portRESET_PRIVILEGE( xRunningPrivileged );

        return xReturn;
    }
#endif
/*-----------------------------------------------------------*/

#if( configUSE_TIMERS == 1 )
    void *MPU_pvTimerGetTimerID( const TimerHandle_t xTimer )
    {
    void * pvReturn;
    BaseType_t xRunningPrivileged = prvRaisePrivilege();

        pvReturn = pvTimerGetTimerID( xTimer );
        portRESET_PRIVILEGE( xRunningPrivileged );

        return pvReturn;
    }
#endif
/*-----------------------------------------------------------*/

#if( configUSE_TIMERS == 1 )
    void MPU_vTimerSetTimerID( TimerHandle_t xTimer, void *pvNewID )
    {
    BaseType_t xRunningPrivileged = prvRaisePrivilege();

        vTimerSetTimerID( xTimer, pvNewID );
        portRESET_PRIVILEGE( xRunningPrivileged );
    }
#endif
/*-----------------------------------------------------------*/

#if( configUSE_TIMERS == 1 )
    BaseType_t MPU_xTimerIsTimerActive( TimerHandle_t xTimer )
    {
    BaseType_t xReturn;
    BaseType_t xRunningPrivileged = prvRaisePrivilege();

        xReturn = xTimerIsTimerActive( xTimer );
        portRESET_PRIVILEGE( xRunningPrivileged );

        return xReturn;
    }
#endif
/*-----------------------------------------------------------*/

#if( configUSE_TIMERS == 1 )
    TaskHandle_t MPU_xTimerGetTimerDaemonTaskHandle( void )
    {
    TaskHandle_t xReturn;
    BaseType_t xRunningPrivileged = prvRaisePrivilege();

        xReturn = xTimerGetTimerDaemonTaskHandle();
        portRESET_PRIVILEGE( xRunningPrivileged );

        return xReturn;
    }
#endif
/*-----------------------------------------------------------*/

#if( ( INCLUDE_xTimerPendFunctionCall == 1 ) && ( configUSE_TIMERS == 1 ) )
    BaseType_t MPU_xTimerPendFunctionCall( PendedFunction_t xFunctionToPend, void *pvParameter1, uint32_t ulParameter2, TickType_t xTicksToWait )
    {
    BaseType_t xReturn;
    BaseType_t xRunningPrivileged = prvRaisePrivilege();

        xReturn = xTimerPendFunctionCall( xFunctionToPend, pvParameter1, ulParameter2, xTicksToWait );
        portRESET_PRIVILEGE( xRunningPrivileged );

        return xReturn;
    }
#endif
/*-----------------------------------------------------------*/

#if( configUSE_TIMERS == 1 )
    const char * MPU_pcTimerGetName( TimerHandle_t xTimer )
    {
    const char * pcReturn;
    BaseType_t xRunningPrivileged = prvRaisePrivilege();

        pcReturn = pcTimerGetName( xTimer );
        portRESET_PRIVILEGE( xRunningPrivileged );

        return pcReturn;
    }
#endif
/*-----------------------------------------------------------*/

#if( configUSE_TIMERS == 1 )
    TickType_t MPU_xTimerGetPeriod( TimerHandle_t xTimer )
    {
    TickType_t xReturn;
    BaseType_t xRunningPrivileged = prvRaisePrivilege();

        xReturn = xTimerGetPeriod( xTimer );
        portRESET_PRIVILEGE( xRunningPrivileged );

        return xReturn;
    }
#endif
/*-----------------------------------------------------------*/

#if( configUSE_TIMERS == 1 )
    TickType_t MPU_xTimerGetExpiryTime( TimerHandle_t xTimer )
    {
    TickType_t xReturn;
    BaseType_t xRunningPrivileged = prvRaisePrivilege();

        xReturn = xTimerGetExpiryTime( xTimer );
        portRESET_PRIVILEGE( xRunningPrivileged );

        return xReturn;
    }
#endif
/*-----------------------------------------------------------*/

#if( configUSE_TIMERS == 1 )
    BaseType_t MPU_xTimerGenericCommand( TimerHandle_t xTimer, const BaseType_t xCommandID, const TickType_t xOptionalValue, BaseType_t * const pxHigherPriorityTaskWoken, const TickType_t xTicksToWait )
    {
    BaseType_t xReturn;
    BaseType_t xRunningPrivileged = prvRaisePrivilege();

        xReturn = xTimerGenericCommand( xTimer, xCommandID, xOptionalValue, pxHigherPriorityTaskWoken, xTicksToWait );
        portRESET_PRIVILEGE( xRunningPrivileged );

        return xReturn;
    }
#endif
/*-----------------------------------------------------------*/

#if( configSUPPORT_DYNAMIC_ALLOCATION == 1 )
    EventGroupHandle_t MPU_xEventGroupCreate( void )
    {
    EventGroupHandle_t xReturn;
    BaseType_t xRunningPrivileged = prvRaisePrivilege();

        xReturn = xEventGroupCreate();
        portRESET_PRIVILEGE( xRunningPrivileged );

        return xReturn;
    }
#endif
/*-----------------------------------------------------------*/

#if( configSUPPORT_STATIC_ALLOCATION == 1 )
    EventGroupHandle_t MPU_xEventGroupCreateStatic( StaticEventGroup_t *pxEventGroupBuffer )
    {
    EventGroupHandle_t xReturn;
    BaseType_t xRunningPrivileged = prvRaisePrivilege();

        xReturn = xEventGroupCreateStatic( pxEventGroupBuffer );
        portRESET_PRIVILEGE( xRunningPrivileged );

        return xReturn;
    }
#endif
/*-----------------------------------------------------------*/

EventBits_t MPU_xEventGroupWaitBits( EventGroupHandle_t xEventGroup, const EventBits_t uxBitsToWaitFor, const BaseType_t xClearOnExit, const BaseType_t xWaitForAllBits, TickType_t xTicksToWait )
{
EventBits_t xReturn;
BaseType_t xRunningPrivileged = prvRaisePrivilege();

    xReturn = xEventGroupWaitBits( xEventGroup, uxBitsToWaitFor, xClearOnExit, xWaitForAllBits, xTicksToWait );
    portRESET_PRIVILEGE( xRunningPrivileged );

    return xReturn;
}
/*-----------------------------------------------------------*/

EventBits_t MPU_xEventGroupClearBits( EventGroupHandle_t xEventGroup, const EventBits_t uxBitsToClear )
{
EventBits_t xReturn;
BaseType_t xRunningPrivileged = prvRaisePrivilege();

    xReturn = xEventGroupClearBits( xEventGroup, uxBitsToClear );
    portRESET_PRIVILEGE( xRunningPrivileged );

    return xReturn;
}
/*-----------------------------------------------------------*/

EventBits_t MPU_xEventGroupSetBits( EventGroupHandle_t xEventGroup, const EventBits_t uxBitsToSet )
{
EventBits_t xReturn;
BaseType_t xRunningPrivileged = prvRaisePrivilege();

    xReturn = xEventGroupSetBits( xEventGroup, uxBitsToSet );
    portRESET_PRIVILEGE( xRunningPrivileged );

    return xReturn;
}
/*-----------------------------------------------------------*/

EventBits_t MPU_xEventGroupSync( EventGroupHandle_t xEventGroup, const EventBits_t uxBitsToSet, const EventBits_t uxBitsToWaitFor, TickType_t xTicksToWait )
{
EventBits_t xReturn;
BaseType_t xRunningPrivileged = prvRaisePrivilege();

    xReturn = xEventGroupSync( xEventGroup, uxBitsToSet, uxBitsToWaitFor, xTicksToWait );
    portRESET_PRIVILEGE( xRunningPrivileged );

    return xReturn;
}
/*-----------------------------------------------------------*/

void MPU_vEventGroupDelete( EventGroupHandle_t xEventGroup )
{
BaseType_t xRunningPrivileged = prvRaisePrivilege();

    vEventGroupDelete( xEventGroup );
    portRESET_PRIVILEGE( xRunningPrivileged );
}
/*-----------------------------------------------------------*/





/* Functions that the application writer wants to execute in privileged mode
can be defined in application_defined_privileged_functions.h.  The functions
must take the same format as those above whereby the privilege state on exit
equals the privilege state on entry.  For example:

void MPU_FunctionName( [parameters ] )
{
BaseType_t xRunningPrivileged = prvRaisePrivilege();

    FunctionName( [parameters ] );

    portRESET_PRIVILEGE( xRunningPrivileged );
}
*/

#if configINCLUDE_APPLICATION_DEFINED_PRIVILEGED_FUNCTIONS == 1
    #include "application_defined_privileged_functions.h"
#endif
