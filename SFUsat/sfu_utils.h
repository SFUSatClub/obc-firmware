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

#define BUSY_SECOND 6600000 // this is about a second for busy waiting at 80 MHz
void busyWait(uint32_t ticksToWait); // DON'T USE THIS IN REAL CODE! - For quick n' dirty testing ONLY :)
void simpleWatchdog(); // tickles watchdog using a busy wait. Appropriate for making sure it doesn't go off during init routines.
void watchdog_busywait(int32_t numtickles);
char* itoa2(int num, char *buffer, int base, int itr); // http://code.geeksforgeeks.org/lDrTiv
#endif /* SFUSAT_SFU_UTILS_H */
