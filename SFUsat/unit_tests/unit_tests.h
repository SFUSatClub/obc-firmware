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
void init_adc_test();
uint32_t test_adc(uint8_t channel);

// Flash test
uint32_t test_flash(void);
bool check16(uint32_t address, uint8_t *matchBuf);
bool checkArbitrary(uint32_t start_address, uint32_t size, uint8_t *matchBuf);



#endif /* SFUSAT_UNIT_TESTS_UNIT_TESTS_H_ */
