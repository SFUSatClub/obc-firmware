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

#ifndef SFUSAT_SFU_UART_H_
#define SFUSAT_SFU_UART_H_


//Joseph
#define DEBUGONLY 0b001
#define FLIGHT	  0b010
#define RF		  0b100
//Joseph

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "sci.h" // by HALcoGen
#include "printf.h"

#include "sfu_tasks.h"
extern unsigned char currChar;

void serialInit();
void serialSendCh(char charToSend);
void serialSend(char*);
void serialSendln(const char*);

BaseType_t serialSendQ(const char * toSend);

BaseType_t serialSendQFromISR(char * toSend);

#endif /* SFUSAT_SFU_UART_H_ */
