/*
 * SFU_Serial.cpp
 *
 *  Created on: Feb 8, 2017
 *      Author: Richard Arthurs
 *      This is a set of functions to test out the UART. They are interrupt driven but still fairly simple.
 */

#include "sfu_uart.h"
#include "sfu_task_radio.h"
#include "sfu_hardwaredefs.h"
#include "printf.h"
#include "sfu_state.h"

unsigned char currChar = '\0';

void serialInit(){
    sciInit(); //initialize the SCI driver
    sciReceive(UART_PORT, 1, &currChar); // place into receive mode

//Joseph Starts
uint8_t ModeFlag = DEBUGONLY;
//Joseph Ends

void serialInit() {
	sciInit(); //initialize the SCI driver
	sciEnableNotification(UART_PORT, SCI_RX_INT);
	sciReceive(UART_PORT, 1, &currChar); // place into receive mode
}

//Joseph Changed
BaseType_t serialSendQ(const char * toSend, uint8_t flag) {
	if(ModeFlag == FLIGHT && flag == DEBUGONLY){
	//do nothing
		}
	else{

		//Original code Starts
		if (xQueueSendToBack(xSerialTXQueue, &toSend, 0) == pdPASS) {
			return pdPASS;
		}
		else {
		return pdFAIL;
		}
	}
	//Original Code Ends
}

//Joseph Starts
//void set_flight(void){
//	ModeFlag=FLIGHT;
//}
//void set_debugonly(void){
//	ModeFlag=DEBUG_ONLY;
//}
//
//void set_rf(void){
//	ModeFlag=RF;
//}

void set_mode(uint8_t flag){
	if(flag & 001 == 001){
		ModeFlag = DEBUG_ONLY;
	}
	else if(flag & 010 == 010){
		ModeFlag = FLIGHT;
	}
	else if(flag & 100 == 100){
		ModeFlag = RF;
	}
}
//Joseph Ends

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
