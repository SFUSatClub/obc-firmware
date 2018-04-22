/*
 * unit_tests.h
 *
 *  Created on: Jan 29, 2018
 *      Author: Richard
 */

#ifndef SFUSAT_UNIT_TESTS_UNIT_TESTS_H_
#define SFUSAT_UNIT_TESTS_UNIT_TESTS_H_
#include "sys_common.h"
#include "gio.h"

// ADC Tests
void test_adc_init();
uint32_t test_adc(uint8_t channel);

// Flash test
uint32_t test_flash(void);
bool check16(uint32_t address, uint8_t *matchBuf);
bool checkArbitrary(uint32_t start_address, uint32_t size, uint8_t *matchBuf);
void flash_write_sequence(uint32_t address,uint32_t *testCount, uint32_t testDataSize, uint8_t *testData, uint32_t passFail); // writes data then checks it
void flash_check_sequence(uint32_t address,uint32_t *testCount, uint32_t testDataSize, uint8_t *testData, uint32_t passFail); // just checks data

#endif /* SFUSAT_UNIT_TESTS_UNIT_TESTS_H_ */
