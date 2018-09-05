/*
 * stdtelem.c
 *
 *  Created on: May 22, 2018
 *      Author: Richard
 */
#include "obc_adc.h"
#include "obc_fs_structure.h"
#include "obc_hardwaredefs.h"
#include "obc_rtc.h"
#include "obc_spiffs.h"
#include "obc_state.h"
#include "obc_task_utils.h"
#include "obc_tasks.h"
#include "obc_utils.h"
#include "adc.h"
#include "sys_pmu.h"
#include "flash_mibspi.h"
#include "unit_tests/unit_tests.h"
#include "printf.h"
#include "stlm75.h"
#include "stdtelem.h"
#include "reg_tcram.h"
#include "bq25703.h"
UART_RF_MUX_INIT();

telem_config_t telemConfig[NUM_TELEM_POINTS];
stdtelem_t stdTelem;

TaskHandle_t xgeneralTelemTaskHandle = NULL;
TaskHandle_t xobcCurrentTelemTaskHandle = NULL;
TaskHandle_t xtemperatureTelemTaskHandle = NULL;
TaskHandle_t xBMSTelemTaskHandle = NULL;
TaskHandle_t xTransmitTelemTaskHandle = NULL;

/* generalTelemTask()
 * 	- the general telemetry for internal things, like filesystem size
 * 	- most of these aren't logged to files in this task
 * 		- they are logged in system (such as state changes) by other tasks when appropriate
 */

void generalTelemTask(void *pvParameters){
	telemConfig[GENERAL_TELEM] = (telem_config_t){	.max = 0, .min = 0, .period = 12000};
	SET_UART_RF_MUX(UART_RF_MUX_TARGET_NONE);
	while(1){
		vTaskDelay(getStdTelemDelay(GENERAL_TELEM));
		stdTelem.current_state = cur_state;
		stdTelem.state_entry_time = stateEntryTime();
		stdTelem.min_heap = xPortGetMinimumEverFreeHeapSize();
		stdTelem.fs_free_blocks = fs.free_blocks;
		stdTelem.fs_prefix = *(char *) fs.user_data;
		stdTelem.ramoccur_1 = tcram1REG->RAMOCCUR;
		stdTelem.ramoccur_2 = tcram2REG->RAMOCCUR;

		sfu_write_fname(FSYS_SYS, "R1: %i", stdTelem.ramoccur_1);
		sfu_write_fname(FSYS_SYS, "R2: %i", stdTelem.ramoccur_2);
	}
}

void BMSTelemTask(void *pvParameters){
	/* be sure to initialize the config of each telem point */
	telemConfig[BMS_TELEM] = (telem_config_t){	.max = 700, .min = 0, .period = 10000};

	while(1){
		vTaskDelay(getStdTelemDelay(BMS_TELEM));
		stdTelem.bms_volt = read_volt();
		stdTelem.bms_curr = read_curr();

		sfu_write_fname(FSYS_BMS, "%d V, %d uA", stdTelem.bms_volt, stdTelem.bms_curr);
	}
}


/* current monitor function
 * 		- prototype for how similar functions will work
 */
void obcCurrentTelemTask(void *pvParameters){
	/* be sure to initialize the config of each telem point */
	telemConfig[OBC_CURR_TELEM] = (telem_config_t){	.max = 700, .min = 0, .period = 10000};
	uint16_t reading;

	while(1){
		vTaskDelay(getStdTelemDelay(OBC_CURR_TELEM));
		reading = OBCCurrent();
//		if(!checkMaxMin(OBC_CURR_TELEM, reading)){
//			// log error
//			// enter safe mode
//		}

		sfu_write_fname(OBC_CURRENT, "%d", reading);
		stdTelem.obc_current = reading;
	}
}

/* temperatureTelemTask
 * 	- records temperatures from across the satellite
 */
void temperatureTelemTask(void *pvParameters){
	telemConfig[TEMP_TELEM] = (telem_config_t){	.max = 130, .min = -40, .period = 10000};
	volatile uint32_t res;
	while(1){
		vTaskDelay(getStdTelemDelay(TEMP_TELEM));
		res = read_temp(OBC_TEMP);
		sfu_write_fname(TEMPS, "OBC: %d", res);
		stdTelem.obc_temp = res;

		res = read_temp(LB_TEMP);
		sfu_write_fname(TEMPS, "LB: %d", res);
		stdTelem.lb_temp = res;

		res = read_temp(UB_TEMP);
		sfu_write_fname(TEMPS, "UB: %d", res);
		stdTelem.ub_temp = res;
	}
}

/* get stdtelem delay
 * 	- pulls the delay out of the struct
 * 	- applies the same calculation as pdMS_TO_TICKS to get the correct delay for the task
 */
uint32_t getStdTelemDelay(uint8_t telemIndex){
	return (telemConfig[telemIndex].period * ( TickType_t ) configTICK_RATE_HZ ) / (( TickType_t ) 1000 );
}

/* max/min checker
 * 	- return 0 if reading is outside the defined max/min range
 */
bool checkMaxMin(uint8_t telemIndex, int32_t reading){
	if(reading > telemConfig[telemIndex].max || reading < telemConfig[telemIndex].min){
		return 0;
	}
	return 1;
}

/* transmitTelemUART
 * 	- transmit the most recent stdtelem on the UART
 * 	- should be higher priority than any telemetry tasks
 */
void transmitTelemUART(void *pvParameters){
	while(1){
		char buf[50] = {'\0'};
	    vTaskDelay(pdMS_TO_TICKS(15000)); // frequency to send out stdtelem
		snprintf(buf, 49, "S1,%i,%i,%i",
				getCurrentRTCTime(),
				stdTelem.current_state,
				stdTelem.state_entry_time
		);

		UART_RF_MUX_SENDQ(buf);
	    vTaskDelay(pdMS_TO_TICKS(20)); // delay slightly to allow transmission to complete

		snprintf(buf, 49, "S2,%i,%i,%c,%i,%i",
				stdTelem.min_heap,
				stdTelem.fs_free_blocks,
				stdTelem.fs_prefix,
				stdTelem.obc_current,
				stdTelem.obc_temp
		);
		UART_RF_MUX_SENDQ(buf);
	    vTaskDelay(pdMS_TO_TICKS(20)); // delay slightly to allow transmission to complete

		snprintf(buf, 49, "S3,%i,%i,%i,%i,%i,%i",
				stdTelem.lb_temp,
				stdTelem.ub_temp,
				stdTelem.ramoccur_1,
				stdTelem.ramoccur_2,
				stdTelem.bms_curr,
				stdTelem.bms_volt
		);
		UART_RF_MUX_SENDQ(buf);
	    vTaskDelay(pdMS_TO_TICKS(20)); // delay slightly to allow transmission to complete
	}
}



//void vStdTelemTask(void *pvParameters){
//	char buf[50] = {'\0'};
//	int16_t OBC_temp;
//	while(1){
//		OBC_temp = read_temp(OBC_TEMP);
//		snprintf(buf, 50, "STD: %i, %i, %i, %i, %i, %c, %i",
//				getCurrentTime(), 					// epoch at send
//				cur_state, 							// state
//				stateEntryTime(), 					// time we entered state
//				xPortGetMinimumEverFreeHeapSize(), 	// min heap
//				fs.free_blocks,						// filesys free blocks
//				*(char *) fs.user_data,				// filesys prefix
//				OBC_temp
//		);
//		serialSendQ(buf);
//		vTaskDelay(pdMS_TO_TICKS(5000)); // frequency to send out stdtelem
//		// TODO: add temps, voltages, currents
//		// TODO: send out from radio
//		/* for large, time consuming things like voltages and currents, we can have the normal read tasks
//		 * send their info to a queue with depth 1. This task will pick up that (most recent) data whenever it runs,
//		 * allowing these values to be updated in the background by their respective tasks.
//		 */
//	}
//}
