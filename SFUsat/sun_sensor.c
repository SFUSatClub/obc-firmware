/*
 * sun_sensor.c
 *
 *  Created on: May 9, 2018
 *      Author: Derrick
 */

#include "sfu_i2c.h"
#include "adc.h"
#include "sun_sensor.h"

//address of muxes
#define Sensor_MUX_X_pos 0x4C
#define Sensor_MUX_X_neg  0x4D
#define Sensor_MUX_Y_pos  0x4E
#define Sensor_MUX_Y_neg  0x4F

// CHIP COMMANDS
#define RD_CMD 0x1
#define WR_CMD 0x0

uint8_t set_mux_channel(uint8_t addr, uint8_t channel) {
	uint8_t cmd = (addr << 1 | WR_CMD);
	int8_t errcode = 0;
	i2cSetSlaveAdd(i2cREG1, addr);					//add the address of the slave device (mux)
	i2cSetDirection(i2cREG1, I2C_TRANSMITTER);
	i2cSetCount(i2cREG1, 1); 						// the total number of bytes to transact before sending stop bit
	i2cSetMode(i2cREG1, I2C_MASTER);
	i2cSetStop(i2cREG1);
	i2cSetStart(i2cREG1);
	errcode = sfu_i2cSend(i2cREG1, 1, &cmd);		//send the command through I2C to mux
	if (errcode != I2C_OK)
		return errcode;
	errcode = sfu_i2cSend(i2cREG1, 1, &channel);	// do I need to switch the address to the address of the internal register?
		if (errcode != I2C_OK)
			return errcode;
	return channel;
}

output_value output[16];
output_value *read_sun_sensor() {						// must loop 4-6 times to get all the values of each sensor of each mux
	output_value *sun_info_ptr;
	adcData_t adc_data[24];

	//table of values for sensor number, addr of mux its assigned to as well as channel on mux
	mux_info mux1[6] = {
			{0,			Sensor_MUX_X_pos,	2},
			{1,			Sensor_MUX_X_pos,	3},
			{2,			Sensor_MUX_X_pos,	4},
			{3,			Sensor_MUX_X_pos,	5},
			{-1,			-1,				-1}, // -1 are taken as NULL values
			{-1,			-1,				-1}
	};
	mux_info mux2[6] = {
			{4,			Sensor_MUX_X_neg,	0},
			{5,			Sensor_MUX_X_neg,	2},
			{6,			Sensor_MUX_X_neg,	4},
			{7,			Sensor_MUX_X_neg,	6},
			{8,			Sensor_MUX_X_neg,	7},
			{-1,			-1,				-1}
	};
	mux_info mux3[6] = {
			{9,			Sensor_MUX_Y_pos,		0},
			{10,			Sensor_MUX_Y_pos,	1},
			{11,			Sensor_MUX_Y_pos,	4},
			{12,			Sensor_MUX_Y_pos,	6},
			{-1,			-1,					-1},
			{-1,			-1,					-1}
	};
	mux_info mux4[6] = {
			{13,			Sensor_MUX_Y_neg,	0},
			{14,			Sensor_MUX_Y_neg,	1},
			{15,			Sensor_MUX_Y_neg,	2},
			{16,			Sensor_MUX_Y_neg,	6},
			{-1,			-1,					-1},
			{-1,			-1,					-1}
	};

	uint8_t i = 0;
	for(i = 0; i < 6; i++) { //loop through channels on muxes (As far as I know there are at most 6 sensors per mux)
		//sets the channel for the muxes
		if(mux1[i-1].sensor_num != -1){
			set_mux_channel(mux1[i-1].mux_addr, mux1[i-1].mux_channel);
		};
		if(mux2[i-1].sensor_num != -1){
			set_mux_channel(mux2[i-1].mux_addr, mux2[i-1].mux_channel);
		};
		if(mux3[i-1].sensor_num != -1){
			set_mux_channel(mux1[i-1].mux_addr, mux3[i-1].mux_channel);
		};
		if(mux4[i].sensor_num != -1){
			set_mux_channel(mux1[i].mux_addr, mux4[i].mux_channel);
		};
		//Reading the values from the ADC
		adcStartConversion(adcREG1,adcGROUP1); // sample all channels on ADC1
		while((adcIsConversionComplete(adcREG1,adcGROUP1))==0); // wait for conversion to complete.
		adcGetData(adcREG1, adcGROUP1,&adc_data[0]);

		if(mux1[i-1].sensor_num != -1) {		//place holder for garbage value
			output[(mux1[i].sensor_num-1)].value = adc_data[0].value;
			output[(mux1[i].sensor_num-1)].sensor_num = mux1[i].sensor_num;
		};
		if(mux2[i-1].sensor_num != -1) {
			output[(mux2[i].sensor_num-1)].value = adc_data[1].value;
			output[(mux2[i].sensor_num-1)].sensor_num = mux2[i].sensor_num;
		};
		if(mux3[i-1].sensor_num != -1) {
			output[(mux3[i].sensor_num-1)].value = adc_data[2].value;
			output[(mux3[i].sensor_num-1)].sensor_num = mux3[i].sensor_num;
		};
		if(mux4[i-1].sensor_num != -1) {
			output[(mux4[i].sensor_num-1)].value = adc_data[3].value;
			output[(mux4[i].sensor_num-1)].sensor_num = mux4[i].sensor_num;
		};
	};
	sun_info_ptr = output; //pointer to output struct
	return sun_info_ptr;
}
