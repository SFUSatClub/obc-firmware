/*
 * stdtelem.h
 *
 *  Created on: May 22, 2018
 *      Author: Richard
 *
 *      Architecture:
 *      	- this task reads all the sensors and logs data to files
 *      	- certain sensors have appropriate ranges, outside of which we will log an error
 *      		- this may include entry into SAFE mode, depending on the sensor
 *      		- max and min range can be updated from the ground, will be stored in a file with access via LSEEK
 *
 *
 *
 */


#ifndef SFUSAT_STDTELEM_H_
#define SFUSAT_STDTELEM_H_

/* enums for config so that we can reasonably easily update them from the ground */
/* enum for check period */
/* enums for max and min sensor values */

// struct for configuration data (tiemout, max, min)
// enum for indexing
// array of the structs

#define NUM_TELEM_POINTS 5

/* used for indexing arrays of timeout, max, min */
enum telem_index{
	GENERAL_TELEM 	= 0,
	OBC_CURR_TELEM 	= 1,
	SUN_SENS_TELEM 	= 2,
	GPS_TELEM			= 3,
	TEMP_TELEM 		= 4
};

typedef struct telem_config{
	int16_t max;		// max permissible sensor reading
	int16_t min;		// min permissible sensor reading
	uint32_t period;	// period in ms
} telem_config_t;

typedef struct stdtelem{
	State_t current_state;
	uint32_t state_entry_time;
	uint32_t min_heap;
	uint8_t fs_free_blocks;
	char fs_prefix;
	int16_t obc_current;
	int16_t obc_temp;
} stdtelem_t;



/* sensor reading functions */
void generalTelemTask();
void obcCurrentTelemTask();
void temperatureTelemTask();

/* Other tasks */
void transmitTelemUART(void *pvParameters);

/* Task handles */
extern TaskHandle_t xgeneralTelemTaskHandle;
extern TaskHandle_t xobcCurrentTelemTaskHandle;
extern TaskHandle_t xtemperatureTelemTaskHandle;
extern TaskHandle_t xTransmitTelemTaskHandle;


/* convenience */
uint32_t getStdTelemDelay(uint8_t telemIndex);

bool checkMaxMin(uint8_t telemIndex, int32_t reading);

// generate array of pointers to the members
// run through the array, dereference

#endif /* SFUSAT_STDTELEM_H_ */
