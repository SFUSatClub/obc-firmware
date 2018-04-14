/*
 * sfu_utils.h
 *
 *  Created on: April 3, 2017
 *      Author: steven
 */

#ifndef SFUSAT_SFU_UTILS_H
#define SFUSAT_SFU_UTILS_H

#include <string.h>
#include <stdlib.h>
#include "sys_common.h"

#define GENERATE_ENUM(ENUM) ENUM,
#define GENERATE_STRING(STRING) #STRING,

/**
 * To work with arrays easier.
 */
#define LEN(array) (sizeof((array))/sizeof((array)[0]))
#define FOR_EACH(it, array)				for (it = array; it < &array[LEN(array)]; it++)
#define IS_OUT_OF_BOUNDS(it, array) 	(it >= &array[LEN(array)])

/*
 * Generates a hash in compile-time (constant expression)
 * Usage: HASH("some_string")
 * Source: http://lolengine.net/blog/2011/12/20/cpp-constant-string-hash
 */
#define H1(s,i,x)   (x*65599u+(uint8_t)s[(i)<strlen(s)?strlen(s)-1-(i):strlen(s)])
#define H4(s,i,x)   H1(s,i,H1(s,i+1,H1(s,i+2,H1(s,i+3,x))))
#define H16(s,i,x)  H4(s,i,H4(s,i+4,H4(s,i+8,H4(s,i+12,x))))
#define H64(s,i,x)  H16(s,i,H16(s,i+16,H16(s,i+32,H16(s,i+48,x))))
#define H256(s,i,x) H64(s,i,H64(s,i+64,H64(s,i+128,H64(s,i+192,x))))
#define HASH(s)    ((uint32_t)(H256(s,0,0)^(H256(s,0,0)>>16)))

/**
 * XOR and keep only the lowest byte
 * TODO: this could probably be made faster by using the HX's above directly
 */
#define GENERATE_ID(s) ( (uint8_t)( HASH(s) ^ (HASH(s)>>24) ^ (HASH(s)>>16) ^ (HASH(s)>>8) ) )


#define BUSY_SECOND 6600000 // this is about a second for busy waiting at 80 MHz
void busyWait(uint32_t ticksToWait); // DON'T USE THIS IN REAL CODE! - For quick n' dirty testing ONLY :)
void simpleWatchdog(); // tickles watchdog using a busy wait. Appropriate for making sure it doesn't go off during init routines.
void watchdog_busywait(int32_t numtickles);
char* itoa2(int num, char *buffer, int base, int itr); // http://code.geeksforgeeks.org/lDrTiv

char* utoa2(uint32_t num, char *buffer, int base, int itr);
void clearBuf(char *buf,uint32_t length);
uint32_t adc_to_mA(uint32_t adcval); // based on some rough calibration, convert an ADC reading of the INA301 current output to an actual current draw

#endif /* SFUSAT_SFU_UTILS_H */
