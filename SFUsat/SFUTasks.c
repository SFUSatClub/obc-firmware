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


void periodicSenderTask(void *pvParameters){ // uses the task parameter to delay itself at a different frequency. Creates UART sender tasks to send whether it was a frequent or infrequent call.
    while(1){
        uint32_t delayInput;
        delayInput = (uint32_t) pvParameters;
        if(delayInput > 4000){
            xTaskCreate( vSenderTask, "Infreq", configMINIMAL_STACK_SIZE, ( void * )  "INFREQUENT Task", 1,  NULL);
        }
        else{
            xTaskCreate( vSenderTask, "Sender1", configMINIMAL_STACK_SIZE, ( void * )  "Sender Task", 1,  NULL);
        }
        vTaskDelay(pdMS_TO_TICKS( delayInput)); // delay a certain time. Use the macro
    }
}

void vSenderTask( void *pvParameters ) // sends stuff to the UART
{
    char * toSend;
    BaseType_t xStatus;
    /* Two instances of this task are created so the value that is sent to the
 queue is passed in via the task parameter - this way each instance can use
 a different value. The queue was created to hold values of type int32_t,
 so cast the parameter to the required type. */
    toSend = (char*)pvParameters;
    /* As per most tasks, this task is implemented within an infinite loop. */
    for( ;; )
    {
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
        }
        vTaskDelete( NULL );  // once complete, delete the current instance of the task.
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
