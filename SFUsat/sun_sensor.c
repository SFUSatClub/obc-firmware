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
#define Sensor_MUX_X_pos  0x4C
#define Sensor_MUX_X_neg  0x4D
#define Sensor_MUX_Y_pos  0x4E
#define Sensor_MUX_Y_neg  0x4F

// CHIP COMMANDS
#define RD_CMD 0x1
#define WR_CMD 0x0

uint8_t set_mux_channel(uint8_t addr, uint8_t channel) {
//	uint8_t cmd = (addr << 1 | WR_CMD);
//	int8_t errcode = 0;
//	i2cSetSlaveAdd(i2cREG1, addr);					//add the address of the slave device (mux)
//	i2cSetDirection(i2cREG1, I2C_TRANSMITTER);
//	i2cSetCount(i2cREG1, 1); 						// the total number of bytes to transact before sending stop bit
//	i2cSetMode(i2cREG1, I2C_MASTER);
//	i2cSetStop(i2cREG1);
//	i2cSetStart(i2cREG1);
//	errcode = sfu_i2cSend(i2cREG1, 1, &cmd);		//send the command through I2C to mux
//	if (errcode != I2C_OK)
//		return errcode;
//	errcode = sfu_i2cSend(i2cREG1, 1, &channel);	// do I need to switch the address to the address of the internal register?
//		if (errcode != I2C_OK)
//			return errcode;
		uint8_t data[2] = {'/0'};
		uint8_t cmd;
		cmd = addr;
data[0]=channel;
		i2cSetMode(i2cREG1, I2C_MASTER);
		i2cSetDirection(i2cREG1, I2C_TRANSMITTER);
		i2cSetSlaveAdd(i2cREG1, addr);
		i2cSetCount(i2cREG1, 2);
		i2cSetStop(i2cREG1);
		i2cSetStart(i2cREG1);
		i2cSend(i2cREG1, 1,&channel);

				i2cSetMode(i2cREG1, I2C_MASTER);
				i2cSetDirection(i2cREG1, I2C_TRANSMITTER);
				i2cSetSlaveAdd(i2cREG1, addr);
				i2cSetCount(i2cREG1, 2);
				i2cSetStop(i2cREG1);
				i2cSetStart(i2cREG1);
				i2cSend(i2cREG1, 2,&channel);

//		i2cSetMode(i2cREG1, I2C_MASTER);
//		i2cSetDirection(i2cREG1, I2C_TRANSMITTER);
//		i2cSetSlaveAdd(i2cREG1, addr);
//		i2cSetCount(i2cREG1, 1);
//		i2cSetStop(i2cREG1);
//		i2cSetStart(i2cREG1);
//		i2cSend(i2cREG1, 1,&cmd);
//
//
//		i2cSetMode(i2cREG1, I2C_MASTER);
//		i2cSetDirection(i2cREG1, I2C_TRANSMITTER);
//		i2cSetSlaveAdd(i2cREG1, addr);
//		i2cSetCount(i2cREG1, 1);
//		i2cSetStop(i2cREG1);
//		i2cSetStart(i2cREG1);
//		i2cSend(i2cREG1, 1,data);

		// RA: this is the hack to get the BMS to talk properly
		i2cInit();
//		return I2C_OK;

	return channel;
}

output_value *read_sun_sensor() {						// must loop 4-6 times to get all the values of each sensor of each mux
	output_value output[16];

	output_value *sun_info_ptr;
	adcData_t adc_data[24];

	//table of values for sensor number, addr of mux its assigned to as well as channel on mux
	mux_info mux1[6] = {
			{1,			Sensor_MUX_X_neg,	2},
			{2,			Sensor_MUX_X_neg,	3},
			{3,			Sensor_MUX_X_neg,	4},
			{4,			Sensor_MUX_X_neg,	5},
			{-1,			-1,				-1}, // -1 are taken as NULL values
			{-1,			-1,				-1}
	};
	mux_info mux2[6] = {
			{5,			Sensor_MUX_X_neg,	0},
			{6,			Sensor_MUX_X_neg,	2},
			{7,			Sensor_MUX_X_neg,	4},
			{8,			Sensor_MUX_X_neg,	6},
			{9,			Sensor_MUX_X_neg,	7},
			{-1,			-1,				-1}
	};
	mux_info mux3[6] = {
			{10,			Sensor_MUX_Y_pos,	1},
			{11,			Sensor_MUX_Y_pos,	2},
			{12,			Sensor_MUX_Y_pos,	3},
			{13,			Sensor_MUX_Y_pos,	2},
			{14,			Sensor_MUX_Y_pos,	3},
			{15,			Sensor_MUX_Y_pos,	7}
	};
	mux_info mux4[6] = {
			{14,			Sensor_MUX_Y_neg,	0},
			{15,			Sensor_MUX_Y_neg,	1},
			{16,			Sensor_MUX_Y_neg,	2},
			{-1,			-1,					-1},
			//{16,			Sensor_MUX_Y_neg,	6},
			{-1,			-1,					-1},
			{-1,			-1,					-1}
	};

	uint8_t i = 0;
	for(i = 0; i < 6; i++) { //loop through channels on muxes (As far as I know there are at most 6 sensors per mux)
		//sets the channel for the muxes
		if(mux1[i].sensor_num != -1){
			set_mux_channel(mux1[i].mux_addr, mux1[i].mux_channel);
		};
		if(mux2[i].sensor_num != -1){
			set_mux_channel(mux2[i].mux_addr, mux2[i].mux_channel);
		};
		if(mux3[i].sensor_num != -1){
			set_mux_channel(mux1[i].mux_addr, mux3[i].mux_channel);
		};
		if(mux4[i].sensor_num != -1){
			set_mux_channel(mux1[i].mux_addr, mux4[i].mux_channel);
		};
		//Reading the values from the ADC
		adcStartConversion(adcREG1,adcGROUP1); // sample all channels on ADC1
		while((adcIsConversionComplete(adcREG1,adcGROUP1))==0); // wait for conversion to complete.
		adcGetData(adcREG1, adcGROUP1,&adc_data[0]);

		if(mux1[i].sensor_num != -1) {		//place holder for garbage value
			output[(mux1[i].sensor_num-1)].value = adc_data[7].value;
			output[(mux1[i].sensor_num-1)].sensor_num = mux1[i].sensor_num;
		};
		if(mux2[i].sensor_num != -1) {
			output[(mux2[i].sensor_num-1)].value = adc_data[5].value;
			output[(mux2[i].sensor_num-1)].sensor_num = mux2[i].sensor_num;
		};
		if(mux3[i].sensor_num != -1) {
			output[(mux3[i].sensor_num-1)].value = adc_data[4].value;
			output[(mux3[i].sensor_num-1)].sensor_num = mux3[i].sensor_num;
		};
		if(mux4[i].sensor_num != -1) {
			output[(mux4[i].sensor_num-1)].value = adc_data[6].value;
			output[(mux4[i].sensor_num-1)].sensor_num = mux4[i].sensor_num;
		};
	};
	sun_info_ptr = output; //pointer to output struct
	return sun_info_ptr;
}

//int16_t read_register(uint8_t addr, uint8_t *reg_return) {
//	/* I2C behaviour
//	 * - peripheral will automatically send a write command (including bit shift + append 1) when set as transmitter
//	 * - data sent to i2cSend will happen after peripheral sends the slave addr + 1 (write) command
//	 * - same goes for reads, peripheral will take care of the device addr + read bit, then clock in the data from the slave
//	 */
//	uint8_t data[2] = {'\0'};
//	uint8_t cmd;
//	cmd = addr;
//
//	i2cSetMode(i2cREG1, I2C_MASTER);
//	i2cSetDirection(i2cREG1, I2C_TRANSMITTER);
//	i2cSetSlaveAdd(i2cREG1, BATT_CHRG);
//	i2cSetCount(i2cREG1, 1);
//	i2cSetStop(i2cREG1);
//	i2cSetStart(i2cREG1);
//	i2cSend(i2cREG1, 1,&cmd);
//
//	i2cSetMode(i2cREG1, I2C_MASTER);
//	i2cSetDirection(i2cREG1, I2C_TRANSMITTER);
//	i2cSetSlaveAdd(i2cREG1, BATT_CHRG);
//	i2cSetCount(i2cREG1, 1);
//	i2cSetStop(i2cREG1);
//	i2cSetStart(i2cREG1);
//	i2cSend(i2cREG1, 1,&cmd);
//
//	while(!i2cIsBusBusy(i2cREG1)){}	// RA: do we need this?
//
//	i2cSetSlaveAdd(i2cREG1, BATT_CHRG);
//	i2cSetCount(i2cREG1, 1); 							// 1 is good for 8-bit register read
//	i2cSetDirection(i2cREG1, I2C_RECEIVER);
//	i2cSetMode(i2cREG1, I2C_MASTER);
//	sfu_i2cReceive(i2cREG1, 1, data);
//	i2cSetStop(i2cREG1);
//	i2cClearSCD(i2cREG1);
//	i2cSetStop(i2cREG1);
//	i2cClearSCD(i2cREG1);
//	*reg_return = data[0];
//
//	// RA: this is the hack to get the BMS to talk properly
//	i2cInit();
//	return I2C_OK;
//}
//
//int16_t write_register(uint8_t addr, uint8_t *to_write) {
//	/* I2C behaviour
//	 * - peripheral will automatically send a write command (including bit shift + append 1) when set as transmitter
//	 * - data sent to i2cSend will happen after peripheral sends the slave addr + 1 (write) command
//	 * - same goes for reads, peripheral will take care of the device addr + read bit, then clock in the data from the slave
//	 */
//	uint8_t cmd[3];
//	cmd[0] = addr;
//	cmd[1] = to_write[0];
//	cmd[2] = to_write[1];
//
//	i2cSetDirection(i2cREG1, I2C_TRANSMITTER);
//	i2cSetMode(i2cREG1, I2C_MASTER);
//	i2cSetSlaveAdd(i2cREG1, BATT_CHRG);
//	i2cSetCount(i2cREG1,3);
//	i2cSetStart(i2cREG1);
//	i2cSend(i2cREG1, 2,&cmd[0]);
//	i2cSetStop(i2cREG1);
////	i2cClearSCD(i2cREG1);
//
//	i2cSetDirection(i2cREG1, I2C_TRANSMITTER);
//	i2cSetMode(i2cREG1, I2C_MASTER);
//	i2cSetSlaveAdd(i2cREG1, BATT_CHRG);
//	i2cSetCount(i2cREG1,3);
//	i2cSetStart(i2cREG1);
//	i2cSend(i2cREG1, 2,&cmd[0]);
//	i2cSetStop(i2cREG1);
//	i2cClearSCD(i2cREG1);
//	// RA: this is the hack to get the BMS to talk properly
//	i2cInit();
//	return I2C_OK;
//}
//
