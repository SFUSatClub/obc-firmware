/*
 * SFU_Serial.cpp
 *
 *  Created on: Feb 8, 2017
 *      Author: Richard Arthurs
 *      This is a set of functions to test out the UART. They are interrupt driven but still fairly simple.
 */

#include "SFU_Serial.h"

unsigned char currChar = '\0';
unsigned char prevChar = '\0';
char inputBuffer[10] = "";
int bufferIndex = 0;

void serialInit(){
    sciInit(); //initialize the SCI driver
    sciReceive(scilinREG, 1, &currChar); // place into receive mode
}

void serialSendCh(char charToSend) {
	sciSend(scilinREG, 1, (unsigned char *)&charToSend);
	sciReceive(scilinREG, 1, &currChar);
}
void serialSend(char* stringToSend) {
	sciSend(scilinREG, strlen(stringToSend), (unsigned char *)stringToSend);
	sciReceive(scilinREG, 1, &currChar);
}

void serialSendln(char* stringToSend){
    const char append[3] = "\r\n";

    // TODO: benchmark sciSend extended string w/ malloc VS sciSend stringToSend followed by sciSend \r\n
    char* extended;
    extended = malloc(strlen(stringToSend)+1+2); // enough to hold everything. 1 captures the null string terminator
    strcpy(extended, stringToSend);
    strcat(extended, append);
    int stringLength = strlen(extended);
    sciSend(scilinREG, stringLength, (unsigned char *) extended); // does not like strlen + 1 to be inlined, but works fine when broken out to another var

    free(extended);

    sciReceive(scilinREG, 1, &currChar);
}

void sciNotification(sciBASE_t *sci, unsigned flags){ // this is the interrupt handler callback
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;
	xQueueSendToBackFromISR(xSerialRXQueue, &currChar, &xHigherPriorityTaskWoken);
	portYIELD_FROM_ISR(xHigherPriorityTaskWoken);

    // check and accept both CR and CRLF EOL
    // exclude both from extracted command
//    if(currChar == '\n') {
//    	if(prevChar == '\r') {
//    		inputBuffer[bufferIndex - 1] = '\0';
//    	} else {
//    		inputBuffer[bufferIndex] = '\0';
//    	}
//    	bufferIndex = 0;
//
//
//    }
//    bufferIndex++;
//
//    if (bufferIndex > 9){ // reset the buffer
//        echoInputBuffer();
//        bufferIndex = 0; // reset our index
//        serialSendln("\r\nBuffer full");
//    }
//
//    prevChar = currChar;
    sciReceive(scilinREG, 1, &currChar); // go back into receive mode
}
