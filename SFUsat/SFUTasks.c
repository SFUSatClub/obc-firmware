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

/**
 * This task is responsible for the handling of all UART related functions.
 *
 * This task does not know if dequeued strings are dynamically allocated; the freeing of strings remains the
 * responsibility of the queuer. Thus, this task should be created with a priority higher than those
 * of tasks that wish to tx over serial. This makes possible the immediate freeing of malloc'd strings once queued due to
 * preemption.
 *
 * @param pvParameters
 */
#define MAX_COMMANDS 10
#define MAX_RX_BUFFER 64
void vSerialTask(void *pvParameters) {
	const TickType_t xTicksToWait = pdMS_TO_TICKS(10);

	char *commands[MAX_COMMANDS] = {NULL};
	int commandsIdx = 0;

	char *txCurrQueuedStr = NULL;

	char rxBuffer[MAX_RX_BUFFER] = "";
	int rxBufferIdx = 0;
	char rxCurrRcvdChar = '\0';
	char rxPrevRcvdChar = '\0';
	while (1) {
		/*
		 * Dequeue next string to send over UART.
		 */
		const int numTxMsgs = uxQueueMessagesWaiting(xSerialTXQueue);
		if (numTxMsgs > 5) {
			serialSend("WARNING: ");
			char buffer[10];
			snprintf(buffer, 10, "%d", numTxMsgs);
			serialSend(buffer);
			serialSendln(" msgs in tx queue");
		}
		while (xQueueReceive(xSerialTXQueue, &txCurrQueuedStr, xTicksToWait) == pdPASS) {
			serialSendln(txCurrQueuedStr);
		}

		/*
		 * Dequeue next char received from UART.
		 * Buffer parsed commands for later processing.
		 */
		if (xQueueReceive(xSerialRXQueue, &rxCurrRcvdChar, xTicksToWait) == pdPASS) {
			rxBuffer[rxBufferIdx] = rxCurrRcvdChar;
		    // check for and accept both CR and CRLF as EOL terminators
		    // exclude both from extracted command
			if (rxCurrRcvdChar == '\n') {
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
				if (rxPrevRcvdChar == '\r') {
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

				checkAndRunCommand(rxBuffer);
				rxBufferIdx = 0;
			} else {
				rxBufferIdx++;
				if (rxBufferIdx >= MAX_RX_BUFFER) {
					serialSendln("ERROR: maximum command length exceeded; try again");
					rxBufferIdx = 0;
				}
			}

			rxPrevRcvdChar = rxCurrRcvdChar;
			//serialSendCh(rxCurrRcvdChar);
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

void periodicSenderTask(void *pvParameters) { // uses the task parameter to delay itself at a different frequency. Creates UART sender tasks to send whether it was a frequent or infrequent call.
	serialSendQ("periodic: started");
	while (1) {
		uint32_t delayInput = (uint32_t) pvParameters;
		if (delayInput > 4000) {
			xTaskCreate(vSenderTask, "SenderInfreq", 300, (void *) "SenderInfreq", 1, NULL);
		} else {
			xTaskCreate(vSenderTask, "SenderFreq", 300, (void *) "SenderFreq", 1, NULL);
		}
		vTaskDelay(pdMS_TO_TICKS(delayInput)); // delay a certain time. Use the macro
	}
}

const size_t MAX_STR_SIZE = 20;
void vSenderTask(void *pvParameters) {
	char *toSend = (char *) pvParameters;

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
		}
	}
}
