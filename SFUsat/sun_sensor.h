/*
 * sun_sensor.h
 *
 *  Created on: May 9, 2018
 *      Author: Derrick
 */

#ifndef SFUSAT_SUN_SENSOR_H_
#define SFUSAT_SUN_SENSOR_H_

//address of muxes
#define Sensor_MUX_X_pos 0x4C
#define Sensor_MUX_X_neg  0x4D
#define Sensor_MUX_Y_pos  0x4E
#define Sensor_MUX_Y_neg  0x4F

typedef struct mux_info{		// need to create a lookup table with the associated values
	int8_t sensor_num;
	int8_t mux_addr;
	int8_t mux_channel;
}mux_info;

//extern mux_info arrayOfMUX_info[];

typedef struct output_value {
	uint8_t sensor_num;
	int16_t value;
}output_value;


//sets the channel on one mux
uint8_t set_mux_channel(uint8_t addr, uint8_t channel);

output_value *read_sun_sensor();



#endif /* SFUSAT_SUN_SENSOR_H_ */
