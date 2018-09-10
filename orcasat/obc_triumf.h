/*
 * sfu_triumf.h
 *
 *  Created on: Sep 21, 2017
 *      Author: Richard
 *
 * 		Code that deals specifically with the test at TRIUMF.
 *      Primarily, set whether we want to read through the attached flash and start writing in empty space, or just
 *      erase it every time we start up.
 */

#ifndef SFUSAT_OBC_TRIUMF_H_
#define SFUSAT_OBC_TRIUMF_H_

#include "flash_mibspi.h"

#define ERASE_START 1 // configure whether we erase flash upon boot

void test_triumf_init();


#endif /* SFUSAT_OBC_TRIUMF_H_ */
