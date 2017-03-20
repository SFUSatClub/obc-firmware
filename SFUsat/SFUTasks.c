#include "SFUTasks.h"


void hundredBlinky(void *pvParameters){ // this is the sanity checker task, blinks LED at 10Hz
    while(1){
        gioSetBit(gioPORTA, 2, gioGetBit(gioPORTA, 2) ^ 1);   // Toggles the A2 bit
        vTaskDelay(pdMS_TO_TICKS( 100 )); // delay 100ms. Use the macro
    }
}

void vTask2(void *pvParameters){
    while(1){

    }
}


//
///* configSUPPORT_STATIC_ALLOCATION is set to 1, so the application must provide an
//implementation of vApplicationGetIdleTaskMemory() to provide the memory that is
//used by the Idle task. */
//void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer,
//                                    StackType_t **ppxIdleTaskStackBuffer,
//                                    uint32_t *pulIdleTaskStackSize )
//{
///* If the buffers to be provided to the Idle task are declared inside this
//function then they must be declared static - otherwise they will be allocated on
//the stack and so not exists after this function exits. */
//static StaticTask_t xIdleTaskTCB;
//static StackType_t uxIdleTaskStack[ configMINIMAL_STACK_SIZE ];
//
//    /* Pass out a pointer to the StaticTask_t structure in which the Idle task's
//    state will be stored. */
//    *ppxIdleTaskTCBBuffer = &xIdleTaskTCB;
//
//    /* Pass out the array that will be used as the Idle task's stack. */
//    *ppxIdleTaskStackBuffer = uxIdleTaskStack;
//
//    /* Pass out the size of the array pointed to by *ppxIdleTaskStackBuffer.
//    Note that, as the array is necessarily of type StackType_t,
//    configMINIMAL_STACK_SIZE is specified in words, not bytes. */
//    *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
//}
//
//#define STACK_SIZE 200
//StaticTask_t xTaskBuffer;
//StackType_t xStack[ STACK_SIZE ];
//void vTaskCode( void * pvParameters )
//{
//    /* The parameter value is expected to be 1 as 1 is passed in the
//    pvParameters value in the call to xTaskCreateStatic(). */
//    configASSERT( ( uint32_t ) pvParameters == 1UL );
//
//    for( ;; )
//    {
//        /* Task code goes here. */
//    }
//}

void periodicSenderTask(void *pvParameters){ // uses the task parameter to delay itself at a different frequency. Creates UART sender tasks to send whether it was a frequent or infrequent call.
    serialSendln( "p" );
    while(1){
        uint32_t delayInput = (uint32_t) pvParameters;
        if(delayInput > 4000){
            serialSendln( "4" );
            xTaskCreate(vSenderTask, "SenderInfreq", 300, (void *) "SenderInfreq", 1, NULL);
        }
        else{
            serialSendln( "1" );
            TaskHandle_t xHandle = NULL;

            /* Create the task without using any dynamic memory allocation. */
//            xHandle = xTaskCreateStatic(
//                          vTaskCode,       /* Function that implements the task. */
//                          "NAME",          /* Text name for the task. */
//                          STACK_SIZE,      /* Number of indexes in the xStack array. */
//                          ( void * ) 1,    /* Parameter passed into the task. */
//                          tskIDLE_PRIORITY,/* Priority at which the task is created. */
//                          xStack,          /* Array to use as the task's stack. */
//                          &xTaskBuffer );  /* Variable to hold the task's data structure. */

            xTaskCreate(vSenderTask, "SenderFreq", 300, (void *) "SenderFreq", 1, NULL);
        }
        vTaskDelay(pdMS_TO_TICKS(delayInput)); // delay a certain time. Use the macro
    }
}
const size_t MAX_STR_SIZE = 20;
void vSenderTask( void *pvParameters ) // sends stuff to the UART
{
    serialSendln( "s" );
    char * toSend = (char *)malloc(MAX_STR_SIZE);
    snprintf(toSend, MAX_STR_SIZE, (char *)pvParameters);

    BaseType_t xStatus;
    for( ;; )
    {
        serialSendln( "w" );
        /* Send the value to the queue.
 The first parameter is the queue to which data is being sent. The
 queue was created before the scheduler was started, so before this task
 started to execute.
 The second parameter is the address of the data to be sent, in this case
 the address of lValueToSend.
 The third parameter is the Block time – the time the task should be kept
 in the Blocked state to wait for space to become available on the queue
 should the queue already be full. In this case a block time is not
 specified because the queue should never contain more than one item, and
 therefore never be full. */
      //  xStatus = xQueueSendToBack( xQueue, &toSend, 0 );
        if( xQueueSendToBack( xQueue, &toSend, 0 ) != pdPASS )
        {
            /* The send operation could not complete because the queue was full -
 this must be an error as the queue should never contain more than
 one item! */
            serialSendln( "Could not send to the queue." );
        } else {
            serialSendln( "q" );
        }
        vTaskDelete( NULL );  // once complete, delete the current instance of the task.
    }
}

void vReceiverTask( void *pvParameters ) // gets called whenever a new value is placed in the queue (the transmit queue for the UART). Uses existing non-RTOS save UART driver to send queue values out.
{
    serialSendln( "r" );
    /* Declare the variable that will hold the values received from the queue. */
    char * receivedVal;
    BaseType_t xStatus;
    const TickType_t xTicksToWait = pdMS_TO_TICKS( 100 );
    /* This task is also defined within an infinite loop. */
    for( ;; )
    {
        /* This call should always find the queue empty because this task will
 immediately remove any data that is written to the queue. */
        if( uxQueueMessagesWaiting( xQueue ) != 0 )
        {
            serialSendln( "Queue should have been empty!" );
        }
        /* Receive data from the queue.
 The first parameter is the queue from which data is to be received. The
 queue is created before the scheduler is started, and therefore before this
 task runs for the first time.
 The second parameter is the buffer into which the received data will be
 placed. In this case the buffer is simply the address of a variable that
 has the required size to hold the received data.
 The last parameter is the block time – the maximum amount of time that the
 task will remain in the Blocked state to wait for data to be available
 should the queue already be empty. */
        xStatus = xQueueReceive( xQueue, &receivedVal, xTicksToWait );
        if( xStatus == pdPASS )
        {
            /* Data was successfully received from the queue, print out the received
 value. */
            //serialSendln( (char *)receivedVal );
            serialSendln( receivedVal );
            free( receivedVal );
        }
//        else // CAN REMOVE THIS BECAUSE THE QUEUE WILL FREQUENTLY BE EMPTY SINCE IT ONLY GETS SENT EVERY SECOND
//        {
//            /* Data was not received from the queue even after waiting for 100ms.
// This must be an error as the sending tasks are free running and will be
// continuously writing to the queue. */
//            serialSendln( "Could not receive from the queue." );
//        }
    }
}
