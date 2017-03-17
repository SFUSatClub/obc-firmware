/*
 * SFU_Serial.cpp
 *
 *  Created on: Feb 8, 2017
 *      Author: Richard Arthurs
 *      This is a set of functions to test out the UART. They are interrupt driven but still fairly simple.
 */



#include "SFU_Serial.h"
#include <string.h>
#include <stdlib.h>

void serialInit(){
    sciInit(); //initialize the SCI driver
    bufferIndex = 0;
    sciReceive(scilinREG, 1, (unsigned char *)&myCommand); // place into receive mode
}

void serialSend(unsigned char* myStr){ // simple, just sends what's given to it with no /r/n
    sciSend(scilinREG, sizeof(myStr) + 1, myStr);
}

void serialSendln(char* stringToSend){
    const char append[3] = "\r\n";

    char* extended;
    extended = malloc(strlen(stringToSend)+1+2); // enough to hold everything. 1 captures the null string terminator
    strcpy(extended, stringToSend);
    strcat(extended, append);
    int stringLength = strlen(extended);
    sciSend(scilinREG, stringLength, (unsigned char *) extended); // does not like strlen + 1 to be inlined, but works fine when broken out to another var

    free(extended);

    sciReceive(scilinREG, 1, (unsigned char *)&myCommand);
}



void sciNotification(sciBASE_t *sci, unsigned flags){ // this is the interrupt handler callback
    inputBuffer[bufferIndex] = myCommand;

    //	sciSend(scilinREG, 1, (unsigned char *)&inputBuffer[bufferIndex]); // echo received character
    bufferIndex ++;

    if (bufferIndex > 9){ // reset the buffer
        echoInputBuffer();
        bufferIndex = 0; // reset our index
        serialSendln("\r\nBuffer full");
    }
    sciReceive(scilinREG, 1, (unsigned char *)&myCommand); // go back into receive mode
}

void echoInputBuffer(void){ 	// prints out the entire input buffer so we can see what's in it
    int i;
    char sendChar;
    for (i = 0; i < sizeof(inputBuffer); i++){
        sendChar = inputBuffer[i];
        serialSendln(&sendChar);
    }
}
