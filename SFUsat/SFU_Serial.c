/*
 * SFU_Serial.cpp
 *
 *  Created on: Feb 8, 2017
 *      Author: Richard Arthurs
 *      This is a set of functions to test out the UART. They are interrupt driven but still fairly simple.
 */

#include "SFU_Serial.h"

unsigned char currChar = '\0';

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

char buffer[250];
int cmdGet(int args, char **argv) {
	if (args < 1) return -1;
	if (strcmp(argv[0], "tasks") == 0) {
	    serialSend("Task\t\tState\tPrio\tStack\tNum\n");
		vTaskList(buffer);
		serialSend(buffer);
	} else if (strcmp(argv[0], "freeheap") == 0) {
		size_t heapSize = xPortGetFreeHeapSize();
		sprintf(buffer, "%lu bytes\n", heapSize);
		serialSend(buffer);
	} else if (strcmp(argv[0], "minheap") == 0) {
		size_t heapSize = xPortGetMinimumEverFreeHeapSize();
		sprintf(buffer, "%lu bytes\n", heapSize);
		serialSend(buffer);
	}
	return 0;
}

/**
 * The following macros allow us to construct the arrays CMD_NAMES and CMD_FUNCS by only modifying
 * the macro CMD_TABLE. This allows us to reliably use the same index to reference a command's name and
 * function without the maintenance problems of constructing the arrays manually.
 *
 * The CMD_TABLE below should be the only place you'd need to edit to add/remove/change commands.
 */
#define CMD_TABLE(_) \
	_("help", cmdHelp) \
	_("get", cmdGet)

#define CMD_NAME_SELECTOR(a, b) \
	a,
#define CMD_FUNC_SELECTOR(a, b) \
	b,
const char *CMD_NAMES[] = {
	CMD_TABLE(CMD_NAME_SELECTOR)
};
int (*const CMD_FUNCS[])(int args, char **argv) = {
	CMD_TABLE(CMD_FUNC_SELECTOR)
};


/**
 * Checks if a string is a valid command, and if so, invokes it.
 *
 * A command is valid if the first word exists in CMD_NAMES.
 * A command can be invoked with 0 to a maximum of 10 arguments.
 * Each command determines the requirements of their own parameters.
 * Commands are space delimited.
 *
 * @param cmd A line of characters received from UART
 * @return pdPASS if the command is found and invoked, pdFAIL if the command does
 * not exist.
 */
#define MAX_CMD_ARGS 10
BaseType_t checkAndRunCommand(char *cmd) {
	const char delim[] = " ";
	char *intendedCmd = strtok(cmd, delim);
	/**
	 * Exit if we could not get the first token.
	 */
	if (intendedCmd == NULL) return pdFAIL;

	/**
	 * Compare the first word (which is the user's intended command) with all known
	 * commands. If a match is found, store the index so we can reference it later.
	 */
	int intendedCmdIdx = -1;
	size_t i;
	for (i = 0; i < sizeof(CMD_NAMES) / sizeof(char*); i++) {
		const char *currCmd = CMD_NAMES[i];
		if(strcmp(intendedCmd, currCmd) == 0) {
			intendedCmdIdx = i;
			break;
		}
	}
	/**
	 * Exit if the command does not exist.
	 */
	if (intendedCmdIdx == -1) return pdFAIL;

	/**
	 * Parse for and store every argument so commands can process them easily.
	 */
	char *args[MAX_CMD_ARGS] = {NULL};
	int argsIdx = 0;
	while (intendedCmd != NULL) {
		intendedCmd = strtok(NULL, delim);
		if (intendedCmd == NULL || argsIdx > MAX_CMD_ARGS - 1) break;
		args[argsIdx] = intendedCmd;
		argsIdx++;
	}

	/**
	 * Invoke the intended command with the amount of arguments to expect and the
	 * array of arguments itself.
	 */
	(*CMD_FUNCS[intendedCmdIdx])(argsIdx, args);

	return pdPASS;
}



