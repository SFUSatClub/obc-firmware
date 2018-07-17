/*
 * sfu_flags.c
 *
 *  Created on: Jul 15, 2018
 *      Author: Richard
 */
#include "sys_common.h"
#include "stdtelem.h"
#include "sfu_flags.h"
#include "sfu_rtc.h"
void initFlagTable(){
	// generate the calls to writeFlag to write the wrapper bytes
	uint32_t timestamp;
	timestamp = getCurrentRTCTime();
	FLAG_TABLE(UPDATE_TIMESTAMP)
	FLAG_TABLE(WRITEFLAG_CALL)
}

void writeFlag(uint32_t offset, uint32_t size, uint8_t * bytes){
	// loop through the bytes and write them at the offset
}
