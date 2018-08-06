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

flag_memory_table_t flag_memory_table;
void * flagPointers[NUM_FLAGS] = {FLAG_TABLE(FLAG_PTR_INIT) };
const uint8_t flagSize[NUM_FLAGS] = {FLAG_TABLE(FLAG_SIZE_CHECK)};

FLAG_TABLE(FLAG_FLASH_WRITE_DEFINE)
FLAG_TABLE(FLAG_FLASH_READ_DEFINE)

void initFlagTable(){
	// generate the calls to writeFlag to write the wrapper bytes
	uint32_t timestamp;
	timestamp = getCurrentRTCTime();
	FLAG_TABLE(INIT_PAYLOAD)			// initialize the payload of each flag
	FLAG_TABLE(UPDATE_TIMESTAMP)		// update the timestamp for each entry
//	FLAG_TABLE(WRITEFLAG_CALL)			// write to flash
//	volatile uint32_t thingy;
//	thingy = FLAG(GEN_TELEM,min);
//	FLAG(GEN_TELEM,min) = 23;
//	writeFlagTest();
//	thingy = 43;
}



bool ptrWriteFlag(uint8_t idx, uint8_t * data, uint8_t size){
	if(size != flagSize[idx]) return 0;	/* Check the size so we won't overwrite data */

	uint32_t i = 0;
	void * pointer = flagPointers[idx];

	/* Write the byte-addressable timestamp */
	union {
		uint32_t timestamp;
		uint8_t	bytes[sizeof(uint32_t)];
	} time_bytes;

	time_bytes.timestamp = getCurrentRTCTime();
	for(i = 0; i < sizeof(time_bytes); i++){
		*((uint8_t *)pointer + i) = time_bytes.bytes[i];
	}

	/* write the remainder of the data */
	for(i = sizeof(time_bytes); i < size; i++){
		*((uint8_t *)pointer + i) = data[i];
	}
	return 1;
}

bool writeFlagTest(){
	/* Create a flag wrapper, assign some values to it, write it, read and check */
	telemConfigWrap_t test;
	test.payload.min = -234;
	test.payload.max = 992;
	test.payload.timestamp = getCurrentRTCTime() + 120;	/* Check that the timestamp is added automatically */
	test.payload.period = 1111;

	if(ptrWriteFlag(2, test.all, sizeof(test.all))){
		if(	FLAG(GEN_TELEM,min) 		== test.payload.min 			&&
			FLAG(GEN_TELEM,max) 		== test.payload.max 			&&
			FLAG(GEN_TELEM,timestamp) 	!= test.payload.timestamp 		&&		/* can't check equality since current time may change */
			FLAG(GEN_TELEM,period) 		== test.payload.period 			){
			return 1;
		}
		else return 0;
	}
	return 0;
}







void writeFlag(uint32_t offset, uint32_t size, uint8_t * bytes){
	// loop through the bytes and write them at the offset
}
