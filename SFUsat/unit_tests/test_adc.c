/*
 * test_adc.c
 *
 *  Created on: Jan 29, 2018
 *      Author: Richard
 */
#include "adc.h"
#include "unit_tests.h"

/* ------- HALCOGEN SETUP ------------
 * ADCs can be grouped, then groups can be sampled and queried. We will use group 1.
 * ADC1 is 24 channel. ADC2 is a 16 channel duplicate of ADC1. So you can sample the same signal with ADC1 and ADC2 and compare the values for safety (we won't do this)
 *
 * TMS570LS0714PGE > Driver Enable > Enable ADC Drivers (checked)
 * VIM Channel 0-31 >  15: ADC1 Group 1
 * ADC1 > ADC1 Group 1 Channel Selection > Enable Pin x
 */

void init_adc_test(){
	adcInit();
}

uint32_t test_adc(uint8_t channel){
	/* Expected output: some sort of value.
	 * An easy way to test is to query channel 2, which is the current drawn by the board (on OBC v0.3 and 0.4)
	 * Grounding an ADC pin on the 104 connector should give a 0
	 *
	 * --------------- USAGE ------------------------
	 * 	init_adc_test();
		uint32_t result;
		result = test_adc(2);
	 */
#pragma diag_push
#pragma diag_suppress 552
	// Example - we don't need these usually
    uint32 ch_count;
    uint32 id;
    uint32 value;
#pragma diag_pop

    adcData_t adc_data[24]; // there are 24 channels
    adcStartConversion(adcREG1,adcGROUP1); // sample all channels on ADC1
    while((adcIsConversionComplete(adcREG1,adcGROUP1))==0); // wait for conversion to complete.
    ch_count = adcGetData(adcREG1, adcGROUP1,&adc_data[0]); //

    /* adc_data[0] -> should have conversions for Group1 channel0 */
    /* adc_data[1] -> should have conversions for Group1 channel1 */
    id    = adc_data[0].id;
    value = adc_data[0].value;

    return adc_data[channel].value;
}
