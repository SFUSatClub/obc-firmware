/*
 * test_flash.c
 *
 *  Created on: Feb 4, 2018
 *      Author: Richard
 *
 *      Runs several tests on flash including:
 *      - various sized writes
 *      - various sized reads
 *      - chip erase and sector erase
 */

#include "unit_tests.h"
#include "flash_mibspi.h"
#include "sfu_uart.h"

uint32_t test_flash(void){
	/* Pre: flash_mibspi_init();
	 * Should return true if all tests have completed successfully.
	 * NOTE: IF MORE TESTS ARE ADDED, UPDATE THE RETURN CHECK AT THE END OF THIS FUNCTION.
	 */

	uint32_t resultCount;
	resultCount = 0;
	bool result;
	uint8_t test_bytes_16[16] = {0x01, 0x01, 0x00, 0x07, 0x03, 0x05, 0x0F, 0x04, 0x07, 0x0B, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
	uint8_t test_bytes_19[19] = {0x01, 0x01, 0x00, 0x07, 0x03, 0x05, 0x0F, 0x04, 0x07, 0x0B, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0B, 0x07, 0x0B};
	uint8_t test_bytes_4[4] = {0xC0, 0xFF, 0xEE, 0x11};
	uint8_t test_bytes_48[48] = {0x01, 0x01, 0x00, 0x07, 0x03, 0x05, 0x0F, 0x04, 0x07, 0x0B, 0xDE, 0xCA, 0xF0, 0xBA, 0xD0, 0x00, 0x0B, 0x07, 0x0B, 0xC0, 0xFF, 0xEE, 0x15, 0xBE, 0x5F, 0x0F, 0x04, 0x07, 0x0B, 0x77, 0xD0, 0x0D, 0x77, 0x45, 0x23, 0x0B, 0x07, 0x0B};

// Quickie built in test
	if(flash_test_JEDEC()){
		serialSendln("Passed flash JEDEC test!");
		resultCount++;
	}

	flash_erase_chip();

// Test a write/read of 16 bytes to flash
	flash_write_sequence(64, &resultCount, 19, test_bytes_19, 1);

	result = check16(64, test_bytes_16);
	if (result == TRUE){ resultCount++; } // this should pass

	result = check16(64 + 16, test_bytes_16); // fail on this check = good since we're reading an empty address
	if (result == FALSE){ resultCount++; }

// Test a write/read of more than 16 bytes of flash
	flash_write_sequence(80, &resultCount, 19, test_bytes_19, 1);

// Test a write/read of less than 16 bytes
	flash_write_sequence(128, &resultCount, 4, test_bytes_4, 1);

// Test a write/read of many bytes
	flash_write_sequence(1600, &resultCount, 48, test_bytes_48, 1);

// Test out sector erase
	flash_erase_chip(); // erase chip is not waiting (Feb 4, 2018)

	flash_write_sequence(0, &resultCount, 48, test_bytes_48, 1); // write bytes into page 0
	flash_write_sequence(4097, &resultCount, 48, test_bytes_48, 1); // write bytes into page 1

	flash_erase_sector(0); // erase sector 0 (addresses 0-4095)

	flash_check_sequence(0, &resultCount, 48, test_bytes_48, 0); // should read 1's since the sector was erased
	flash_check_sequence(4097, &resultCount, 48, test_bytes_48, 1); // should correctly read second set of bytes which are on the next sector

	flash_write_sequence(34959, &resultCount, 48, test_bytes_48, 1);

	flash_write_sequence(2097151-48, &resultCount, 48, test_bytes_48, 1);

	// Test read arbitrary
	// RA: Manually confirm results - it's too late to write a checker :D
	// If tests are passing, this works fine and we don't need to run this code.
	//   uint8_t readBuf[48] = {0xFF}; // since flash default = FF
	//   flash_read_arbitrary(test_address, 48, readBuf); // note: we only send in 38 bytes of random data. So get back a bunch of gunk at the end (zeros? - seems like it should be FF's)
	//   flash_read_arbitrary(128, 4, readBuf);
	//   flash_read_arbitrary(64, 16, readBuf);

	if(resultCount == 13){
		return true; // passed!
	}
	return false;
}

void flash_write_sequence(uint32_t address,uint32_t *testCount, uint32_t testDataSize, uint8_t *testData, uint32_t passFail){
	/* This sequence writes to flash and then checks that the write was successful (or unsuccessful)
	 *
	 * passFail is the marker for whether the comparison should fail.
	 * 	- if we should write data but not be able to read it back because we erased it, passFail = 0
	 * 	- if it is expected that our read data comes back exactly as what's written, passFail = 1
	 */
//	uint32_t result;

	flash_write_arbitrary(address, testDataSize, testData);
	while(flash_status() != 0){ // wait for the write to complete
	}
	if (checkArbitrary(address, testDataSize, testData) == passFail){ (*testCount)++; } // this should pass
}

void flash_check_sequence(uint32_t address,uint32_t *testCount, uint32_t testDataSize, uint8_t *testData, uint32_t passFail){
	while(flash_status() != 0){ // wait for the write to complete
	}
	if (checkArbitrary(address, testDataSize, testData) == passFail){ (*testCount)++; } // this should pass
}

bool check16(uint32_t test_address,uint8_t *matchBuf){
	/* Reads back 16 bytes, checks them against an input.
	 * Return 0 if mismatch
	 * Return 1 if match
	 */

	uint16_t readBuf[16];
	uint32_t i;
	bool result;

	flash_read_16(test_address, readBuf);
	result = true;

	// check that read is the same as written
	for(i = 0; i < 16; i++ ){
		if(readBuf[i] != matchBuf[i]){ // if any don't match, result = false
			result = false;
		}
	}
	return result;
}

bool checkArbitrary(uint32_t check_address, uint32_t size, uint8_t *matchBuf){
	uint32_t counter;
	bool result;
	uint8_t checkBuffer[16] = {0};
	uint32_t framePosition;
	uint32_t i;
	uint16_t remainderBuffer[16] = {0};

	framePosition = 0;
	result = TRUE;

	// loop through the first bytes and the first multiples of 16
	for(counter = 0; counter < size; counter++){
		checkBuffer[framePosition] = matchBuf[counter]; // snag the subset to match
		framePosition++;

		if(framePosition == 16){ // run a check 16 every 16 bytes. Check for 16 not 15 since it's incremented above
			result = check16(check_address, checkBuffer);
			check_address += 16;
			framePosition = 0;
			if(result == 0){
				return false; // immediate return if any matches fail
			}
		}
	}

	if (framePosition != 0){
		// read the remaining bytes
		flash_read_16(check_address, remainderBuffer);

		// check that read is the same as written
		// assume we start at the beginning of an address
		for(i = 0; i < framePosition; i++ ){
			if(checkBuffer[i] != remainderBuffer[i]){
				return false; // immediate return if any match fails
			}
		}
	}
	return true;
}
