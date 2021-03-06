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

#ifndef SFUSAT_OBC_GPS_H_
#define SFUSAT_OBC_GPS_H_

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "obc_tasks.h"
#include "sci.h" // by HALcoGen
#include "printf.h"

extern unsigned char currCharGPS;
typedef struct GPSdata {
   char  longa[30];
   char  lat[30];
   char  alt[30];
   uint8_t flag;
} GPSdata_t;

#define GPS_CHAR_TIMEOUT 30000

void serialGPSInit();
bool serialGPSSendln(const char*);
void restartGPS();
void readGPS();
//void parseGPS(char *);
void parseGPS(char *gpsBuff,GPSdata_t *data);
struct GPSData;

BaseType_t serialGPSSendQ(const char * toSend);
BaseType_t serialGPSSendQFromISR(char * toSend);

extern QueueHandle_t xSerialRXQueue;
extern QueueHandle_t xSerialGpsRXQueue;

#endif /* SFUSAT_OBC_GPS_H_ */
