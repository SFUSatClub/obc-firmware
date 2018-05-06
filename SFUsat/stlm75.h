/*
 * stlm75.h
 *
 *  Created on: Apr 18, 2018
 *      Author: Richard
 *
 *      Usage: if temperature read functions return a large negative number (ex: -30077), there was an error
 *      		- calling function should trigger an entry into safe mode
 */

#ifndef SFUSAT_STLM75_H_
#define SFUSAT_STLM75_H_

// Chip addresses for all sensors in the satellite
#define OBC_TEMP 0x48
#define RF1_TEMP 0x49
#define RF2_TEMP 0x4A

#define TEMP_READ_ERROR -999

int16_t obc_temp_test_no_rtos();
int16_t read_temp(uint8_t addr);			// suitable for RTOS use (includes mutex)
int16_t read_temp_raw(uint8_t addr);		// temp read without mutex

#endif /* SFUSAT_STLM75_H_ */
