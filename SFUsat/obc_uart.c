/*
 * SFU_Serial.cpp
 *
 *  Created on: Feb 8, 2017
 *      Author: Richard Arthurs
 *      This is a set of functions to test out the UART. They are interrupt driven but still fairly simple.
 */

#include <obc_hardwaredefs.h>
#include <obc_state.h>
#include <obc_task_radio.h>
#include <obc_uart.h>
#include "printf.h"

unsigned char currChar = '\0';

void serialInit() {
	sciInit(); //initialize the SCI driver
	sciEnableNotification(UART_PORT, SCI_RX_INT);
	sciReceive(UART_PORT, 1, &currChar); // place into receive mode
}

BaseType_t serialSendQ(const char * toSend) {
	if (xQueueSendToBack(xSerialTXQueue, &toSend, 0) == pdPASS) {
		return pdPASS;
	} else {
		return pdFAIL;
	}
}

BaseType_t serialSendQFromISR(char * toSend) {
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;
	BaseType_t xStatus = xQueueSendToBackFromISR(xSerialTXQueue, &toSend, &xHigherPriorityTaskWoken);
	portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
	sciReceive(UART_PORT, 1, &currChar);

	return xStatus;
}

void serialSendCh(char charToSend) {
	sciSend(UART_PORT, 1, (unsigned char *) &charToSend);
	sciReceive(UART_PORT, 1, &currChar);
}
void serialSend(char* stringToSend) {
	sciSend(UART_PORT, strlen(stringToSend), (unsigned char *) stringToSend);
	sciReceive(UART_PORT, 1, &currChar);
}

void serialSendln(const char* stringToSend) {
	sciSend(UART_PORT, strlen(stringToSend), (unsigned char *) stringToSend);
	sciSend(UART_PORT, 2, "\r\n");

	sciReceive(UART_PORT, 1, &currChar);
}

void sciNotification(sciBASE_t *sci, unsigned flags) { // this is the interrupt handler callback
	if ((flags & SCI_RX_INT) == SCI_RX_INT && (state_persistent_data.in_RTOS == 1)) {
		BaseType_t xHigherPriorityTaskWoken = pdFALSE;
		xQueueSendToBackFromISR(xSerialRXQueue, &currChar, &xHigherPriorityTaskWoken);
		portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
	}

	sciReceive(UART_PORT, 1, &currChar); // go back into receive mode
}
void sfu_putchar(char character) { // wrapper for printf
	serialSend(&character);
}
