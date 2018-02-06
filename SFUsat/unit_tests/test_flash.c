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
	 * Should return 7 if all tests have completed successfully. (Update if more tests are added)
	 */
	uint32_t resultCount;
	resultCount = 0;
	uint32_t test_address;
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

	flash_erase_chip(); // erase chip is not waiting (Feb 4, 2018)

// Test a write/read of 16 bytes to flash
	test_address = 64;
    flash_write_arbitrary(test_address, 16, test_bytes_16);

    while(flash_status() != 0){ // wait for the write to complete
    }
    result = checkArbitrary(test_address, 16, test_bytes_16);
    if (result == TRUE){ resultCount++; } // this should pass

    result = check16(test_address, test_bytes_16);
    if (result == TRUE){ resultCount++; } // this should pass

    result = check16(test_address + 16, test_bytes_16); // fail on this check = good since we're reading an empty address
    if (result == FALSE){ resultCount++; }

// Test a write/read of more than 16 bytes of flash
	test_address = 80;
    flash_write_arbitrary(test_address, 19, test_bytes_19);

    while(flash_status() != 0){ // wait for the write to complete
    }

    result = checkArbitrary(test_address, 19, test_bytes_19);
    if (result == TRUE){ resultCount++; } // this should pass

 // Test a write/read of less than 16 bytes
    test_address = 128;
    flash_write_arbitrary(test_address, 4, test_bytes_4);
    while(flash_status() != 0){ // wait for the write to complete
    }
    result = checkArbitrary(test_address, 4, test_bytes_4);
    if (result == TRUE){ resultCount++; } // this should pass

// Test a write/read of many bytes
   test_address = 1600;
   flash_write_arbitrary(test_address, 48, test_bytes_48);
   while(flash_status() != 0){ // wait for the write to complete
   }
   result = checkArbitrary(test_address, 48, test_bytes_48);
   if (result == TRUE){ resultCount++; } // this should pass


// Test read arbitrary
// RA: Manually confirm results - it's too late to write a checker :D
//   uint8_t readBuf[48] = {0xFF}; // since flash default = FF
//   flash_read_arbitrary(test_address, 48, readBuf); // note: we only send in 38 bytes of random data. So get back a bunch of gunk at the end (zeros? - seems like it should be FF's)
//   flash_read_arbitrary(128, 4, readBuf);
//   flash_read_arbitrary(64, 16, readBuf);

    return resultCount;
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
