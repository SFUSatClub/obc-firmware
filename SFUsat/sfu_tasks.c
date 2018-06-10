#include "adc.h"
#include "sys_pmu.h"

#include "sfu_tasks.h"
#include "sfu_cmds.h"
#include "sfu_hardwaredefs.h"
#include "flash_mibspi.h"
#include "sfu_rtc.h"
#include "sfu_utils.h"
#include "unit_tests/unit_tests.h"
#include "printf.h"
#include "adc.h"
#include "sfu_state.h"
#include "sfusat_spiffs.h"
#include "stlm75.h"

QueueHandle_t xSerialTXQueue;
QueueHandle_t xSerialRXQueue;

void blinky(void *pvParameters) { // blinks LED at 10Hz
	// You can initialize variables for your task here. Runs once.
	// The loop of the task will run repeatedly until the task is preempted. So delay or suspend the task once you're done processing.
	while (1) {
		gioSetBit(DEBUG_LED_PORT, DEBUG_LED_PIN, gioGetBit(DEBUG_LED_PORT, DEBUG_LED_PIN) ^ 1);   // Toggles the pin
		vTaskDelay(pdMS_TO_TICKS(200)); // delay 200ms. Use the macro
	}
}


void vStateTask(void *pvParameters) {
	while (1){
		cur_state = runState( cur_state, &state_persistent_data ); // update state machine
		vTaskDelay(pdMS_TO_TICKS(2000)); // check state every 2s
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
		if (numTxMsgs > 25) {
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
				serialSend("> ");
				serialSendln(commandPtr);
				commands[commandsIdx] = commandPtr;
				commandsIdx++;

				checkAndRunCommandStr(rxBuffer);
				rxBufferIdx = 0;
			} else {
				rxBufferIdx++;
				if (rxBufferIdx >= MAX_RX_BUFFER) {
					serialSendln("ERROR: maximum command length exceeded; try again");
					rxBufferIdx = 0;
				}
			}

			rxPrevRcvdChar = rxCurrRcvdChar;
			serialSendCh(rxCurrRcvdChar);
			if (uxQueueMessagesWaiting(xSerialRXQueue) > 5) {
				serialSendln("WARNING: lots of uart rx");
			}
		}
//	    sciReceive(UART_PORT, 1, &currChar); // place into receive mode

	}
}

void vExternalTickleTask(void *pvParameters){
	while(1){
		gioSetBit(WATCHDOG_TICKLE_PORT, WATCHDOG_TICKLE_PIN, 1);
		vTaskDelay(1); // delay 1ms
		gioSetBit(WATCHDOG_TICKLE_PORT, WATCHDOG_TICKLE_PIN, 0);
		vTaskDelay(400); // repeat this cycle every 400ms
	}
}


