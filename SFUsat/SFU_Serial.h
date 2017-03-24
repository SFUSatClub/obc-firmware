/*
 * SFU_Serial.h
 *
 *  Created on: Feb 8, 2017
 *      Author: Richard Arthurs
 *
 *      this header has the SCI communication drivers for the TMS570
 *
 *      This requires interrupt channel 13 to be set (SCI/LIN) RX interrupt
 */

#ifndef SFUSAT_SFU_SERIAL_H_
#define SFUSAT_SFU_SERIAL_H_

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "sci.h" // by HALcoGen

#include "SFUTasks.h"

void serialInit();
void serialSendCh(char charToSend);
void serialSend(char*);
void serialSendln(char*);
BaseType_t serialSendQ(char * toSend);
BaseType_t serialSendQFromISR(char * toSend);

extern char *CMD_NAMES[];
extern int (*CMD_FUNCS[])(int args, char **argv);
BaseType_t checkAndRunCommand(char *cmd);

#endif /* SFUSAT_SFU_SERIAL_H_ */
