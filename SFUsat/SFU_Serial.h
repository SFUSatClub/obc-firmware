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

#include "sci.h" // by HALcoGen
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

static unsigned char myCommand;
static char inputBuffer[10]; // 10 character input buffer
static int bufferIndex = 0;

void serialInit();
void serialSend(char*);
void serialSendln(char*);
void echoInputBuffer(void);



#endif /* SFUSAT_SFU_SERIAL_H_ */
