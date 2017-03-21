#include "SFUTasks.h"

QueueHandle_t xQueue;
QueueHandle_t xSerialTXQueue;
QueueHandle_t xSerialRXQueue;

void hundredBlinky(void *pvParameters) { // this is the sanity checker task, blinks LED at 10Hz
	while (1) {
		gioSetBit(gioPORTA, 2, gioGetBit(gioPORTA, 2) ^ 1);   // Toggles the A2 bit
		//serialSendQ("blinked");
		vTaskDelay(pdMS_TO_TICKS(100)); // delay 100ms. Use the macro
	}
}

void vTask2(void *pvParameters) {
	while (1) {

	}
}


#define MAX_COMMANDS 10
#define MAX_RX_BUFFER 64
void vSerialTask(void *pvParameters) {
	const TickType_t xTicksToWait = pdMS_TO_TICKS(10);
	char *queuedStrToTx;

	//char **commands = (char**) malloc((MAX_COMMANDS)*sizeof(char*));
	char *commands[MAX_COMMANDS];
	int commandsIdx = 0;

	char rxBuffer[MAX_RX_BUFFER];
	int rxBufferIdx = 0;

	char currRcvdCharFromRx;
	char prevRcvdCharFromRx;
	while (1) {
		if (xQueueReceive(xSerialTXQueue, &queuedStrToTx, xTicksToWait) == pdPASS) {
			serialSendln(queuedStrToTx);
			//free(queuedStr);
//            char nm[30];
//            UBaseType_t waiting = uxQueueMessagesWaiting(xSerialQueue);
//            snprintf(nm, 30, "serialQlen=%Lu", waiting);
//            serialSendln(nm);
			if (uxQueueMessagesWaiting(xSerialTXQueue) > 5) {
				serialSendln("WARNING: lots of uart tx");
			}
		}

		if (xQueueReceive(xSerialRXQueue, &currRcvdCharFromRx, xTicksToWait) == pdPASS) {
			rxBuffer[rxBufferIdx] = currRcvdCharFromRx;
		    // check for and accept both CR and CRLF as EOL terminators
		    // exclude both from extracted command
			if (currRcvdCharFromRx == '\n') {
				// reset and free the commands buffer if it is currently full and a new command is being added
				if (commandsIdx >= MAX_COMMANDS) {
					serialSendln("Freeing commands: ");
					while(--commandsIdx > 0) {
						serialSendln(commands[commandsIdx]);
						free(commands[commandsIdx]);
					}
					commandsIdx = 0;
				}
				size_t toAllocate;
				if (prevRcvdCharFromRx == '\r') {
					rxBuffer[rxBufferIdx - 1] = '\0'; // strlen = rxBufferIdx
					toAllocate = rxBufferIdx;
				} else {
					rxBuffer[rxBufferIdx] = '\0'; // strlen = rxBufferIdx + 1
					toAllocate = rxBufferIdx + 1;
				}
				char *commandPtr = malloc(toAllocate);
				strcpy(commandPtr, rxBuffer);
				serialSendln(commandPtr);
				commands[commandsIdx] = commandPtr;
				commandsIdx++;

				rxBufferIdx = 0;
			} else {
				rxBufferIdx++;
				if (rxBufferIdx >= MAX_RX_BUFFER) {
					serialSendln("ERROR: maximum command length exceeded; try again");
					rxBufferIdx = 0;
				}
			}

			prevRcvdCharFromRx = currRcvdCharFromRx;
			serialSendCh(currRcvdCharFromRx);
			if (uxQueueMessagesWaiting(xSerialRXQueue) > 5) {
				serialSendln("WARNING: lots of uart rx");
			}
		}
	}
}
// to send long strings?
void vSerialSenderTask(void *pvParameters) {
	while (1) {

	}
}
BaseType_t serialSendQ(char * toSend) {
	if (xQueueSendToBack(xSerialTXQueue, &toSend, 0) == pdPASS) {
		return pdPASS;
	} else {
		return pdFAIL;
	}
}
BaseType_t serialSendQFromISR(char * toSend) {
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;
	BaseType_t xStatus = xQueueSendToBackFromISR(xSerialTXQueue, &toSend, &xHigherPriorityTaskWoken);
	if(xHigherPriorityTaskWoken == pdTRUE) {
		taskYIELD();
	}
	return xStatus;
}

void periodicSenderTask(void *pvParameters) { // uses the task parameter to delay itself at a different frequency. Creates UART sender tasks to send whether it was a frequent or infrequent call.
	serialSendQ("periodic: started");
	while (1) {
		uint32_t delayInput = (uint32_t) pvParameters;
		if (delayInput > 4000) {
			serialSendQ("periodic: 4");
			xTaskCreate(vSenderTask, "SenderInfreq", 300, (void *) "SenderInfreq", 1, NULL);
		} else {
			serialSendQ("periodic: 1 ");
			xTaskCreate(vSenderTask, "SenderFreq", 300, (void *) "SenderFreq", 1, NULL);
		}
		vTaskDelay(pdMS_TO_TICKS(delayInput)); // delay a certain time. Use the macro
	}
}

const size_t MAX_STR_SIZE = 20;
void vSenderTask(void *pvParameters) // sends stuff to the UART
{
	serialSendQ("sender: started");
	char *toSend = (char *) malloc(MAX_STR_SIZE);
	snprintf(toSend, MAX_STR_SIZE, (char *) pvParameters);

	BaseType_t xStatus;
	for (;;) {
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
		if (xQueueSendToBack( xQueue, &toSend, 0 ) != pdPASS) {
			/* The send operation could not complete because the queue was full -
			 this must be an error as the queue should never contain more than
			 one item! */
			serialSendQ("Could not send to the queue.");
		} else {
			serialSendQ("sender: sent");
		}
		vTaskDelete( NULL); // once complete, delete the current instance of the task.
	}
}

// gets called whenever a new value is placed in the queue (the transmit queue for the UART). Uses existing non-RTOS save UART driver to send queue values out.
void vReceiverTask(void *pvParameters) {
	serialSendQ("receiver: started");
	/* Declare the variable that will hold the values received from the queue. */
	char * receivedVal;
	BaseType_t xStatus;
	const TickType_t xTicksToWait = pdMS_TO_TICKS(100);
	/* This task is also defined within an infinite loop. */
	for (;;) {
		/* This call should always find the queue empty because this task will
		 immediately remove any data that is written to the queue. */
		if (uxQueueMessagesWaiting(xQueue) != 0) {
			serialSendQ("Queue should have been empty!");
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
		xStatus = xQueueReceive(xQueue, &receivedVal, xTicksToWait);
		if (xStatus == pdPASS) {
			/* Data was successfully received from the queue, print out the received value. */
			serialSendQ(receivedVal);
			free(receivedVal);
		}
	}
}
