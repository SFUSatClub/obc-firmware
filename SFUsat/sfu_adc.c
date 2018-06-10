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
	adcUpdate();
	return adc_to_mA(adc_data[OBC_CURRENT_ADC].value);
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

void vADCRead(void *pvParameters) {
	// TODO: this task should start a conversion of group 1 (will read ADC channel 2 for starters)
	// TODO: instead of waiting for the conversion to complete, add a semaphore from the ISR callback (adc1Group1Interrupt)
	// this task would start conversion, suspend and wait for semaphore (xsemaphoretake). When the semaphore is raised (given) in adc1Group1Interrupt,
	// it means the conversion is done. This task will take the semaphore back, then grab the data as we do now.
	// With the data, create a new task and get the data into the task. - task notification, task param, queue, etc.
	// In the new task, print out the data received.
	// This is a lot of stuff, but it mimics what we will eventually need to do - call tasks to write data out to flash and save it.
	// Chapter 6: https://www.freertos.org/Documentation/161204_Mastering_the_FreeRTOS_Real_Time_Kernel-A_Hands-On_Tutorial_Guide.pdf
	// Note in this example we just run the function from the test. Normally we'd have the code in here, not call a far away function


	while (1) {
	    adcData_t adc_data[24]; // there are 24 channels
	    char sendBuf[20];
		// start conversion (it's inside test_adc)
		// take a semaphore here

		    adcStartConversion(adcREG1,adcGROUP1); // sample all channels on ADC1
		    while((adcIsConversionComplete(adcREG1,adcGROUP1))==0); // wait for conversion to complete.
		    adcGetData(adcREG1, adcGROUP1,&adc_data[0]); //

		// if we get here, semaphore is taken, so we have data and can now print/send to other tasks

		    snprintf(sendBuf, 20,"Current (mA): %d",adc_data[2].value);
		  serialSendQ(sendBuf);
		vTaskDelay(pdMS_TO_TICKS(2000)); // check every 2s
	}
}
