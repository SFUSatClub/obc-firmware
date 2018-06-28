/*
 * sfu_gps.h
 *
 *  Created on: Jun 6, 2018
 *      Author: Victor Leon
 *
 *      this header has the SCI2 communication drivers
 *
 *      This requires interrupt channel 13 to be set (SCI/LIN) RX interrupt
 */

#ifndef SFUSAT_SFU_GPS_H_
#define SFUSAT_SFU_GPS_H_

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "sci.h" // by HALcoGen
#include "printf.h"

#include "sfu_tasks.h"
extern unsigned char currCharGPS;

#define GPS_CHAR_TIMEOUT 10000

void serialGPSInit();
bool serialGPSSendln(const char*);
void restartGPS();
void readGPS();
void parseGPS(char *);
struct GPSData;

BaseType_t serialGPSSendQ(const char * toSend);
BaseType_t serialGPSSendQFromISR(char * toSend);

extern QueueHandle_t xSerialRXQueue;
extern QueueHandle_t xSerialGpsRXQueue;

#endif /* SFUSAT_SFU_GPS_H_ */
