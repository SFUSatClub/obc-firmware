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


#define second 6600000 // this is about a second for busy waiting at 80 MHz
void busyWait(uint32_t ticksToWait); // DON'T USE THIS IN REAL CODE! - For quick n' dirty testing ONLY :)

#endif /* SFUSAT_SFU_UTILS_H */
