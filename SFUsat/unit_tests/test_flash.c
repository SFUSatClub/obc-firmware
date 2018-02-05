/*
 * test_flash.c
 *
 *  Created on: Feb 4, 2018
 *      Author: Richard
 */

#include "unit_tests.h"
#include "flash_mibspi.h"
#include "sfu_uart.h"

uint32_t test_flash(void){
	/* Pre: flash_mibspi_init();
	 *
	 */
	uint32_t resultCount;
	resultCount = 0;
	uint32_t test_address;
	bool result;
    uint8_t test_bytes_16[16] = {0x01, 0x01, 0x00, 0x07, 0x03, 0x05, 0x0F, 0x04, 0x07, 0x0B, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    uint8_t test_bytes_19[19] = {0x01, 0x01, 0x00, 0x07, 0x03, 0x05, 0x0F, 0x04, 0x07, 0x0B, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0B, 0x07, 0x0B};

// Quickie built in test
	if(flash_test_JEDEC()){
		serialSendln("Passed flash JEDEC test!");
		resultCount++;
	}

	flash_erase_chip(); // erase chip is not waiting

// Test a write/read of 16 bytes to flash
	test_address = 64;
    flash_write_arbitrary(test_address, 16, test_bytes_16);

    while(flash_status() != 0){ // wait for the write to complete
    }

    result = check16(test_address, test_bytes_16);
    if (result == TRUE){ resultCount++; } // this should pass

    result = check16(test_address + 16, test_bytes_16); // fail on this check = good since we're reading an empty address
    if (result == FALSE){ resultCount++; }

// Test a write/read of more than 16 bytes of flash
	test_address = 80;
    flash_write_arbitrary(test_address, 19, test_bytes_19);
    while(flash_status() != 0){ // wait for the write to complete
    }

    result = check16(test_address, test_bytes_16);
    if (result == TRUE){ resultCount++; } // this should pass

    result = check16(test_address+16, test_bytes_16);
    if (result == TRUE){ resultCount++; } // this should pass

  // read them back


//    flash_erase_chip(); // erase chip is not waiting
//    result = rw16_test(test_address);

    return resultCount;
}

bool check16(uint32_t test_address, uint8_t *matchBuf){
	/* Reads back 16 bytes, checks them against an input.
	 * Return 0 if mismatch
	 * Return 1 if match
	 */

// Todo: if size is less than 16, match against 1 since that's the empty value of flash
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
