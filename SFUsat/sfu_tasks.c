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

void vADCRead(void *pvParameters) {
	// TODO: this task should start a conversion of group 1 (will read ADC channel 2 for starters)
	// TODO: instead of waiting for the conversion to complete, add a semaphore from the ISR callback (adc1Group1Interrupt)
	// this task would start conversion, suspend and wait for semaphore (xsemaphoretake). When the semaphore is raised (given) in adc1Group1Interrupt,
	// it means the conversion is done. This task will take the semaphore back, then grab the data as we do now.
	// With the data, create a new task and get the data into the task. - task notification, task param, queue, etc.
	// In the new task, print out the data received.
	// This is a lot of stuff, but it mimics what we will eventually need to do - call tasks to write data out to flash and save it.
	// Chapter 6: https://www.freertos.org/Documentation/161204_Mastering_the_FreeRTOS_Real_Time_Kernel-A_Hands-On_Tutorial_Guide.pdf
	// Note in this example we just run the function from the test. Normally we'd have the code in here, not call a far away function


	while (1) {
	    adcData_t adc_data[24]; // there are 24 channels
	    char sendBuf[20];
		// start conversion (it's inside test_adc)
		// take a semaphore here

		    adcStartConversion(adcREG1,adcGROUP1); // sample all channels on ADC1
		    while((adcIsConversionComplete(adcREG1,adcGROUP1))==0); // wait for conversion to complete.
		    adcGetData(adcREG1, adcGROUP1,&adc_data[0]); //

		// if we get here, semaphore is taken, so we have data and can now print/send to other tasks

		    snprintf(sendBuf, 20,"Current (mA): %d",adc_data[2].value);
		  serialSendQ(sendBuf);
		vTaskDelay(pdMS_TO_TICKS(2000)); // check every 2s
	}
}


void vStateTask(void *pvParameters) {
	while (1){
		cur_state = runState( cur_state, &state_persistent_data ); // update state machine
		vTaskDelay(pdMS_TO_TICKS(2000)); // check state every 2s
	}
}

/**
 * STDTelem
 * - this is responsible for collecting and sending out our regular telemetry packets
 * - these have defined content including:
 * - this is what we'll be sending out most of the time, unless we decide to issue commands
 *   to downlink other data if we decide we want it.
 * 		- state, time we enter state
 * 		- temperatures
 * 		- battery
 * 		- filesystem size, file prefix
 * 		- epoch at send
 */
void vStdTelemTask(void *pvParameters){
	char buf[50] = {'\0'};
	int16_t OBC_temp;
	while(1){
		OBC_temp = read_temp(OBC_TEMP);
		snprintf(buf, 50, "STD: %i, %i, %i, %i, %i, %c, %i",
				getCurrentTime(), 					// epoch at send
				cur_state, 							// state
				stateEntryTime(), 					// time we entered state
				xPortGetMinimumEverFreeHeapSize(), 	// min heap
				fs.free_blocks,						// filesys free blocks
				*(char *) fs.user_data,				// filesys prefix
				OBC_temp
		);
		serialSendQ(buf);
		vTaskDelay(pdMS_TO_TICKS(5000)); // frequency to send out stdtelem
		// TODO: add temps, voltages, currents
		// TODO: send out from radio
		/* for large, time consuming things like voltages and currents, we can have the normal read tasks
		 * send their info to a queue with depth 1. This task will pick up that (most recent) data whenever it runs,
		 * allowing these values to be updated in the background by their respective tasks.
		 */
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


