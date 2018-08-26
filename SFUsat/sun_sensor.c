/*
 * sun_sensor.c
 *
 *  Created on: May 9, 2018
 *      Author: Derrick
 */

#include <obc_i2c.h>
#include <obc_uart.h>
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

volatile output_value output[32]; //Changed from 16 to 32


/* adc from mux
 * - returns the ADC channel (0 based, so the real ADC number) associated with a mux
 */
uint8_t adc_from_mux(uint8_t mux_addr){
	switch (mux_addr){
		case 0x4C:
			return 20;
		case 0x4D:
			return 18;
		case 0x4E:
			return 17;
		case 0x4F:
			return 19;
		case 0x00:	// for testing
			return 2;
	}
	return 0; /* should never get here */
}

/* ADC CHANNEL VAL
 * 	- The results from the ADC aren't necessarily in order or at the same position as the channel #
 * 	- search through the data array until we hit the correct ID (channel number = 0 based)
 */
uint16_t adc_channel_val(uint8_t adc_chan, adcData_t * data){
	uint8_t i;
	for(i = 0; i < 15; i++){
		if(data->id == adc_chan){
			return data->value;
		}
		data++;
	}
	serialSendQ("Err: no ADC ID");
	return 0;
}

void read_all_mux_channels(uint8_t addr){
	uint8_t chan;	// mux channel
	uint8_t adc_chan;
	adcData_t adc_data[24];

	adc_chan = adc_from_mux(addr);
	char buf[50];
	snprintf(buf, 49, "MUX: %i\r\n",addr);
	serialSend(buf);

	for(chan = 0; chan < 8; chan++){
		if(addr != 0x00){
			set_mux_channel(addr, chan);
		}
		adcStartConversion(adcREG1,adcGROUP1);
		while((adcIsConversionComplete(adcREG1,adcGROUP1))==0);
		adcGetData(adcREG1, adcGROUP1,&adc_data[0]);

		clearBuf(buf, 50);
		snprintf(buf, 49, "\t Channel %i: %i\r\n",chan,  adc_channel_val(adc_chan, &adc_data[0]));
		serialSend(buf);
	}
	serialSend("\r\n");
}

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
//		uint8_t data[2] = {'/0'};
//		uint8_t cmd;
//		cmd = addr;
//		data[0]=channel;
		i2cSetMode(i2cREG1, I2C_MASTER);
		i2cSetDirection(i2cREG1, I2C_TRANSMITTER);
		i2cSetSlaveAdd(i2cREG1, addr);
		i2cSetCount(i2cREG1, 2);
		i2cSetStop(i2cREG1);
		i2cSetStart(i2cREG1);
//		i2cSend(i2cREG1, 1,&channel);
		if(BMS_i2c_send(i2cREG1,1,&channel) < 0){
			serialSendln("MUX I2C ERR");
		}

		i2cSetMode(i2cREG1, I2C_MASTER);
		i2cSetDirection(i2cREG1, I2C_TRANSMITTER);
		i2cSetSlaveAdd(i2cREG1, addr);
		i2cSetCount(i2cREG1, 2);
		i2cSetStop(i2cREG1);
		i2cSetStart(i2cREG1);
//		i2cSend(i2cREG1, 2,&channel);
		if(BMS_i2c_send(i2cREG1,1,&channel) < 0){
			serialSendln("MUX I2C ERR");
		}
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


bool read_sun_sensor() {						// must loop 4-6 times to get all the values of each sensor of each mux
	adcData_t adc_data[24];

	//table of values for sensor number, addr of mux its assigned to as well as channel on mux
	//Each XY PCB Panel has 6 photodiodes and an 8 output mux. (2 Channels arent used)
	//Photodiodes on 2, 6, 8 are operational on June 25th 2018
	mux_info mux1[8] = {
			{1,			Sensor_MUX_X_pos,	1},
			{2,			Sensor_MUX_X_pos,	2},
			{3,			Sensor_MUX_X_pos,	3},
			{4,			Sensor_MUX_X_pos,	6},
			{5,			Sensor_MUX_X_pos,	7},
			{6,			Sensor_MUX_X_pos,	8},
			{25,		Sensor_MUX_X_pos,	4},
			{26,		Sensor_MUX_X_pos,	5}
	};
	mux_info mux2[8] = {
			{7,			Sensor_MUX_X_neg,	1},
			{8,			Sensor_MUX_X_neg,	2},
			{9,			Sensor_MUX_X_neg,	3},
			{10,		Sensor_MUX_X_neg,	6},
			{11,		Sensor_MUX_X_neg,	7},
			{12,		Sensor_MUX_X_neg,	8},
			{27,		Sensor_MUX_X_neg,	4},
			{28,		Sensor_MUX_X_neg,	5}
	};
	mux_info mux3[8] = {
			{13,		Sensor_MUX_Y_pos,	1},
			{14,		Sensor_MUX_Y_pos,	2},
			{15,		Sensor_MUX_Y_pos,	3},
			{16,		Sensor_MUX_Y_pos,	6},
			{17,		Sensor_MUX_Y_pos,	7},
			{18,		Sensor_MUX_Y_pos,	8},
			{29,		Sensor_MUX_Y_pos,	4},
			{30,		Sensor_MUX_Y_pos,	5}
	};
	mux_info mux4[8] = {
			{19,		Sensor_MUX_Y_neg,	1},
			{20,		Sensor_MUX_Y_neg,	2},
			{21,		Sensor_MUX_Y_neg,	3},
			{22,		Sensor_MUX_Y_neg,	6},
			{23,		Sensor_MUX_Y_neg,	7},
			{24,		Sensor_MUX_Y_neg,	8},
			{31,		Sensor_MUX_Y_neg,	4},
			{32,		Sensor_MUX_Y_neg,	5}
	};

	uint8_t i = 0;
	for(i = 0; i < 8; i++)
	{
		//Loops through the channels of each mux. Pulls the data from the data line
		set_mux_channel(mux1[i].mux_addr, mux1[i].mux_channel);
		set_mux_channel(mux2[i].mux_addr, mux2[i].mux_channel);
		set_mux_channel(mux3[i].mux_addr, mux3[i].mux_channel);
		set_mux_channel(mux4[i].mux_addr, mux4[i].mux_channel);

		//Reading Data from the corresponding channel number for each mux
		adcStartConversion(adcREG1, adcGROUP1); //Sample all channels on ADC1
		uint32_t timeout = 0;
		while((adcIsConversionComplete(adcREG1, adcGROUP1)) == 0 && timeout < 30000){
			timeout++;
		}
		if(timeout >= (30000 - 1)){
			return 0;	// failure: ADC timeout
		}
		adcGetData(adcREG1, adcGROUP1, &adc_data[0]);

		output[(mux1[i].sensor_num-1)].value = adc_channel_val(adc_from_mux(mux1[i].mux_addr), &adc_data[0]); //adc_data[7].value;
		output[(mux1[i].sensor_num-1)].sensor_num = mux1[i].sensor_num;

		output[(mux2[i].sensor_num-1)].value = adc_channel_val(adc_from_mux(mux2[i].mux_addr), &adc_data[0]);//adc_data[5].value;
		output[(mux2[i].sensor_num-1)].sensor_num = mux2[i].sensor_num;

		output[(mux3[i].sensor_num-1)].value = adc_channel_val(adc_from_mux(mux3[i].mux_addr), &adc_data[0]); //adc_data[4].value;
		output[(mux3[i].sensor_num-1)].sensor_num = mux3[i].sensor_num;

		output[(mux4[i].sensor_num-1)].value = adc_channel_val(adc_from_mux(mux4[i].mux_addr), &adc_data[0]);//adc_data[6].value;
		output[(mux4[i].sensor_num-1)].sensor_num = mux4[i].sensor_num;
	}

	return 1;
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
