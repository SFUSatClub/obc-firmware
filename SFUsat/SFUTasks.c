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



/* configSUPPORT_STATIC_ALLOCATION is set to 1, so the application must provide an
implementation of vApplicationGetIdleTaskMemory() to provide the memory that is
used by the Idle task. */
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer,
                                    StackType_t **ppxIdleTaskStackBuffer,
                                    uint32_t *pulIdleTaskStackSize )
{
/* If the buffers to be provided to the Idle task are declared inside this
function then they must be declared static - otherwise they will be allocated on
the stack and so not exists after this function exits. */
static StaticTask_t xIdleTaskTCB;
static StackType_t uxIdleTaskStack[ configMINIMAL_STACK_SIZE ];

    /* Pass out a pointer to the StaticTask_t structure in which the Idle task's
    state will be stored. */
    *ppxIdleTaskTCBBuffer = &xIdleTaskTCB;

    /* Pass out the array that will be used as the Idle task's stack. */
    *ppxIdleTaskStackBuffer = uxIdleTaskStack;

    /* Pass out the size of the array pointed to by *ppxIdleTaskStackBuffer.
    Note that, as the array is necessarily of type StackType_t,
    configMINIMAL_STACK_SIZE is specified in words, not bytes. */
    *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
}

#define STACK_SIZE 200
StaticTask_t xTaskBuffer;
StackType_t xStack[ STACK_SIZE ];
void vTaskCode( void * pvParameters )
{
    /* The parameter value is expected to be 1 as 1 is passed in the
    pvParameters value in the call to xTaskCreateStatic(). */
    configASSERT( ( uint32_t ) pvParameters == 1UL );

    for( ;; )
    {
        /* Task code goes here. */
    }
}
void periodicSenderTask(void *pvParameters){ // uses the task parameter to delay itself at a different frequency. Creates UART sender tasks to send whether it was a frequent or infrequent call.
    while(1){
        serialSendln( "periodic dbg" );
        uint32_t delayInput;
        delayInput = 1000;
        if(delayInput > 4000){
            //xTaskCreate( vSenderTask, "Infreq", configMINIMAL_STACK_SIZE, ( void * )  "INFREQUENT Task", 1,  NULL);
        }
        else{
            TaskHandle_t xHandle = NULL;

            /* Create the task without using any dynamic memory allocation. */
            xHandle = xTaskCreateStatic(
                          vTaskCode,       /* Function that implements the task. */
                          "NAME",          /* Text name for the task. */
                          STACK_SIZE,      /* Number of indexes in the xStack array. */
                          ( void * ) 1,    /* Parameter passed into the task. */
                          tskIDLE_PRIORITY,/* Priority at which the task is created. */
                          xStack,          /* Array to use as the task's stack. */
                          &xTaskBuffer );  /* Variable to hold the task's data structure. */

            int b = 2 + 2;
            BaseType_t ret = xTaskCreate( vSenderTask, "Sender1", configMINIMAL_STACK_SIZE, NULL  , 2,  NULL);
            int a = 1 + 1;
        }
        vTaskDelay(pdMS_TO_TICKS(delayInput)); // delay a certain time. Use the macro
    }
}

void vSenderTask( void *pvParameters ) // sends stuff to the UART
{
    /* As per most tasks, this task is implemented within an infinite loop. */
    while(1) {
        serialSendln( "sender dbg" );
        vTaskDelay(pdMS_TO_TICKS(3000));
        //vTaskDelete( NULL );  // once complete, delete the current instance of the task.
    }
}


void vReceiverTask( void *pvParameters ) // gets called whenever a new value is placed in the queue (the transmit queue for the UART). Uses existing non-RTOS save UART driver to send queue values out.
{
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
            serialSendln( (char *)receivedVal );
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
