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
	portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
	return xStatus;
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
    sciReceive(scilinREG, 1, &currChar); // go back into receive mode
}


int cmdHelp(int args, char **argv) {
	serialSendQ("Help");
	int i;
	for(i = 0; i < args; i++) {
		serialSendQ(argv[i]);
	}
	return 0;
}

int cmdTest(int args, char **argv) {
	serialSendQ("Test");
	int i;
	for(i = 0; i < args; i++) {
		serialSendQ(argv[i]);
	}
	return 0;
}

#define CMD_NAME_SELECTOR(a, b) \
	a,

#define CMD_FUNC_SELECTOR(a, b) \
	b,

#define CMD_DATA(_) \
	_("help", cmdHelp) \
	_("test", cmdTest)

char *CMD_NAMES[] = {
	CMD_DATA(CMD_NAME_SELECTOR)
};

int (*CMD_FUNCS[])(int args, char **argv) = {
	CMD_DATA(CMD_FUNC_SELECTOR)
};

BaseType_t checkAndRunCommand(char *cmd) {
	const char delim[] = " ";
	char *intendedCmd = strtok(cmd, delim);
	// if we could not get the first token
	if (intendedCmd == NULL) return 0;

	int intendedCmdIdx = -1;
	size_t i;
	for (i = 0; i < sizeof(CMD_NAMES) / sizeof(char*); i++) {
		char *currCmd = CMD_NAMES[i];
		if(strcmp(intendedCmd, currCmd) == 0) {
			intendedCmdIdx = i;
			break;
		}
	}
	// if the command does not exist
	if (intendedCmdIdx == -1) return 0;

	int argsIdx = 0;
	char *args[10] = {NULL};
	while (intendedCmd != NULL) {
		intendedCmd = strtok(NULL, delim);
		if (intendedCmd == NULL) break;
		args[argsIdx] = intendedCmd;
		argsIdx++;
	}

	(*CMD_FUNCS[intendedCmdIdx])(argsIdx, args);

	return 1;
}



