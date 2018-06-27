/*
 * sfu_adc.c
 *
 *  Created on: Jun 4, 2018
 *      Author: Richard
 */

#include "sfu_adc.h"
#include "sys_common.h"
#include "adc.h"
#include "sfu_hardwaredefs.h"
#include "FreeRTOS.h"
#include "rtos_task.h"
#include "rtos_semphr.h"
#include "sfu_utils.h"


adcData_t adc_data[24];
SemaphoreHandle_t xADCMutex;

void sfuADCInit(){
	adcInit();
	xADCMutex = xSemaphoreCreateMutex();
}

/* OBCCUrrent
 * 	- returns OBC measured current in mA
 */
uint16_t OBCCurrent(){
	uint16_t reading_ma;
	adcUpdate();
	reading_ma = adc_data[OBC_CURRENT_ADC].value;//adc_to_mA(adc_data[OBC_CURRENT_ADC].value);
	return reading_ma;
}

/* adcUpdate
 * 	- updates the global ADC result struct with the most recent values
 * 	- upon calling this, you can access the values from the channels you desire
 */
void adcUpdate(){
	if (xSemaphoreTake(xADCMutex, pdMS_TO_TICKS(500)) == pdTRUE) {
		adcStartConversion(adcREG1,adcGROUP1); // sample all channels on ADC1
		while((adcIsConversionComplete(adcREG1,adcGROUP1))==0); // wait for conversion to complete.
		adcGetData(adcREG1, adcGROUP1,&adc_data[0]);
		xSemaphoreGive(xADCMutex);
	}
	else{
		// todo: log error
	}
}

