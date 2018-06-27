/*
 * bq25703.c
 *
 *  Created on: May 9, 2018
 *      Author: Eric Kwok
 */

#include "i2c.h"
#include "stlm75.h"
#include "reg_i2c.h"
#include "sfu_i2c.h"
#include "FreeRTOS.h"
#include "rtos_semphr.h"
#include "sfu_uart.h"
#include "bq25703.h"

#define RD_CMD 0x1
#define WR_CMD 0x0
//
//int16_t read_batt(uint8_t addr) {
//	if (xSemaphoreTake(xI2CMutex, pdMS_TO_TICKS(500) ) == pdTRUE) {
//		int16_t batt_chrg;
//		batt_chrg = read_batt_raw(OBC_BATT);
//		if (batt_chrg ) {
//			//error
//			//what's the conditions?
//		}
//		xSemaphoreGive(xI2CMutex);
//		return batt_chrg;
//	}
//	else {
//		serialSendQ("Battery charge read can't get mutex");
//		return 0; //error
//	}
//}



/* BMS test
 * 	- several tests used during verification of the i2C drivers for the BMS
 * 	- check manufacturer ID
 * 	- BMS status assignment demonstration
 * 	- read/write check
 */
void bms_test() {
	int16_t res;
	uint8_t read_val;
	uint8_t wr16[2] = {06, 05};
	volatile BATTERY_STATUS_BITS_t status; // RA: mark volatile since we don't do anything with this, but we don't want it to be optimized away

	if (xSemaphoreTake(xI2CMutex, pdMS_TO_TICKS(500) ) == pdTRUE) {
	/* Test 1: manufacturer ID read */
		res = read_register(0x2E, &read_val);
		if(read_val == 0x40){	// check that the manifacturer ID comes up
			serialSendln("BMS detected!");
		}
		else{
			serialSendln("Error: no BMS response");
		}

		/* Test 2: set the BMS status */
		status = get_bms_status();

		/* Test 3: read a register */
		res = read_register(ADCOPTION_REG, &read_val);
		res = read_register(ADCOPTION_REG + 1, &read_val);

		/* set up the ADC */
		// configure LSbyte and MSByte
		wr16[0] = ADCOPT_EN_ADC_VBAT | ADCOPT_EN_ADC_ICHG | ADCOPT_EN_ADC_IDCHG;
		wr16[1] = ADCOPT_ADC_CONV_BIT | ADCOPT_ADC_START_BIT;	// continuous mode + start

		write_register(ADCOPTION_REG, &wr16[0]);
		write_register(ADCOPTION_REG + 1, &wr16[1]);

		/* RA: usually the start bit doesn't take hold. Probably more configuration to do */
		res = read_register(ADCOPTION_REG, &read_val);
		res = read_register(ADCOPTION_REG + 1, &read_val);

		/* Test 4: working register read/write/read */
		res = read_register(CHARGEOPT1_REG, &read_val);
		res = read_register(CHARGEOPT1_REG + 1, &read_val);

		wr16[1] = 0x71;	// setting the MSbit in the LSbyte of the register

		write_register(CHARGEOPT1_REG, &wr16[1]);
		res = read_register(CHARGEOPT1_REG, &read_val);

		xSemaphoreGive(xI2CMutex);
	}
	else {
		serialSendQ("Battery charge read can't get mutex");
	}
}


/* returns the battery status struct */
BATTERY_STATUS_BITS_t get_bms_status(void){
	int16_t res; // contain i2c error code
	uint8_t read_data;
	BATTERY_STATUS_t batteryStatus;

	/* read LSbyte, assign to the struct */
	res = read_register(CHARGERSTATUS_REG, &read_data);
	if(res < 0){
		serialSendQ("Battery test error");
	}
	batteryStatus.struct_assign = read_data;

	/* read MSbyte, shift and assign to the struct */
	res = read_register(CHARGERSTATUS_REG+1, &read_data);
	if(res < 0){
		serialSendQ("Battery test error");
	}
	batteryStatus.struct_assign = batteryStatus.struct_assign | (read_data << 8);

	return batteryStatus.bits_struct;
}



/* read register
 * - halcogen read with a double initial register write
 * - also employs the I2C reset hack to allow multiple reads/writes to the BMS
 */
int16_t read_register(uint8_t addr, uint8_t *reg_return) {
	/* I2C behaviour
	 * - peripheral will automatically send a write command (including bit shift + append 1) when set as transmitter
	 * - data sent to i2cSend will happen after peripheral sends the slave addr + 1 (write) command
	 * - same goes for reads, peripheral will take care of the device addr + read bit, then clock in the data from the slave
	 */
	uint8_t data[2] = {'\0'};
	uint8_t cmd;
	cmd = addr;

	i2cSetMode(i2cREG1, I2C_MASTER);
	i2cSetDirection(i2cREG1, I2C_TRANSMITTER);
	i2cSetSlaveAdd(i2cREG1, BATT_CHRG);
	i2cSetCount(i2cREG1, 1);
	i2cSetStop(i2cREG1);
	i2cSetStart(i2cREG1);
	BMS_i2cSend(i2cREG1, 1,&cmd);

	i2cSetMode(i2cREG1, I2C_MASTER);
	i2cSetDirection(i2cREG1, I2C_TRANSMITTER);
	i2cSetSlaveAdd(i2cREG1, BATT_CHRG);
	i2cSetCount(i2cREG1, 1);
	i2cSetStop(i2cREG1);
	i2cSetStart(i2cREG1);
// 	i2cSend(i2cREG1, 1,&cmd);
  BMS_i2cSend(i2cREG1, 1,&cmd);


	while(!i2cIsBusBusy(i2cREG1)){}	// RA: do we need this?

	i2cSetSlaveAdd(i2cREG1, BATT_CHRG);
	i2cSetCount(i2cREG1, 1); 							// 1 is good for 8-bit register read
	i2cSetDirection(i2cREG1, I2C_RECEIVER);
	i2cSetMode(i2cREG1, I2C_MASTER);
	BMS_i2cReceive(i2cREG1, 1, data);
	i2cSetStop(i2cREG1);
	i2cClearSCD(i2cREG1);
	i2cSetStop(i2cREG1);
	i2cClearSCD(i2cREG1);
	*reg_return = data[0];

	// RA: this is the hack to get the BMS to talk properly
	i2cInit();
	return I2C_OK;
}

int16_t write_register(uint8_t addr, uint8_t *to_write) {
	/* I2C behaviour
	 * - peripheral will automatically send a write command (including bit shift + append 1) when set as transmitter
	 * - data sent to i2cSend will happen after peripheral sends the slave addr + 1 (write) command
	 * - same goes for reads, peripheral will take care of the device addr + read bit, then clock in the data from the slave
	 */
	uint8_t cmd[3];
	cmd[0] = addr;
	cmd[1] = to_write[0];
	cmd[2] = to_write[1];

	i2cSetDirection(i2cREG1, I2C_TRANSMITTER);
	i2cSetMode(i2cREG1, I2C_MASTER);
	i2cSetSlaveAdd(i2cREG1, BATT_CHRG);
	i2cSetCount(i2cREG1,3);
	i2cSetStart(i2cREG1);
// 	i2cSend(i2cREG1, 2,&cmd[0]);
  BMS_i2cSend(i2cREG1, 2,&cmd[0]);

	i2cSetStop(i2cREG1);
//	i2cClearSCD(i2cREG1);

	i2cSetDirection(i2cREG1, I2C_TRANSMITTER);
	i2cSetMode(i2cREG1, I2C_MASTER);
	i2cSetSlaveAdd(i2cREG1, BATT_CHRG);
	i2cSetCount(i2cREG1,3);
	i2cSetStart(i2cREG1);
// 	i2cSend(i2cREG1, 2,&cmd[0]);
    BMS_i2cSend(i2cREG1, 2,&cmd[0]);

	i2cSetStop(i2cREG1);
	i2cClearSCD(i2cREG1);
	// RA: this is the hack to get the BMS to talk properly
	i2cInit();
	return I2C_OK;
}

//int16_t read_batt_raw(uint8_t addr) {
//	uint8_t data[2] = {'\0'}; //set second bit to 0? and why?
//	uint8_t cmd = (addr);
//	//I want to set the ADC to do a oneshot or continuous update first?
//	//[15] 1-shot = 0, continuous = 1
//	//[14] start = 1
//	//[7-0] = ?? I think I want [1] = 1 for VSYS? other bits are 0?
//	int16_t batt_chrg_v = 0;
//	int16_t errcode = 0;
//
//	//Need to write to configure ADC first?
//	i2cSetSlaveAdd(i2cREG1, BATT_CHRG << 1 | WR_CMD);
//	i2cSetDirection(i2cREG1, I2C_TRANSMITTER);
//	i2cSetCount(i2cREG1, 1);
//	i2cSetMode(i2cREG1, I2C_MASTER);
//	i2cSetStop(i2cREG1);
//	i2cSetStart(i2cREG1);
//	errcode = sfu_i2cSend(i2cREG1, 1, &cmd;); //how does this work
//	if (errcode != I2C_OK)
//		return errcode;
//
//	//Then read voltage from the battery
//	cmd = (addr << 1 | RD_CMD);
//	i2cSetSlaveAdd(i2cREG1, OBC_BATT);
//	i2cSetDirection(i2cREG1, I2C_TRANSMITTER);
//	i2cSetCount(i2cREG1, 1);
//	i2cSetMode(i2cREG1, I2C_MASTER);
//	i2cSetStop(i2cREG1);
//	i2cSetStart(i2cREG1);
//	errcode = sfu_i2cSend(i2cREG1, 1, &cmd;);
//	if (errcode != I2C_OK)
//		return errcode;
//
//	i2cSetSlaveAdd(i2cREG1, OBC_BATT);
//	i2cSetCount(i2cREG1, 2); //we read 16 bits or 8? 15-8 is sys voltage, 7-0 is batt voltage..
//	i2cSetDirection(i2cREG1, I2C_RECEIVER);
//	i2cSetMode(i2cREG1, I2C_MASTER);
//	errcode = sfu_i2cSend(i2cREG1, 2, data;);
//	if (errcode != I2C_OK)
//		return errcode;
//	i2cSetStop(i2cREG1);
//	i2cClearSCD(i2cREG1);
//
//	errcode = sfu_is_bus_busy();
//	if (errcode != I2C_OK)
//		return errcode;
//
//	errcode = sfu_ok_transmit();
//	if (errcode != I2C_OK)
//		return errcode;
//
//	//Need something to convert the raw data into volts?
//	return batt_chrg_v;
//
//
//
//	//Read current from the battery (Should this be separate?)
//	cmd = (addr << 1 | WR_CMD); //I want to set [15] to 0 to disable low power mode?
//	i2cSetSlaveAdd(i2cREG1, OBC_OPT0);
//	i2cSetDirection(i2cREG1, I2C_TRANSMITTER);
//	i2cSetCount(i2cREG1, 1);
//	i2cSetMode(i2cREG1, I2C_MASTER);
//	i2cSetStop(i2cREG1);
//	i2cSetStart(i2cREG1);
//	errcode = sfu_i2cSend(i2cREG1, 1, &cmd);
//	if (errcode != I2C_OK)
//		return errcode;
//
//	i2cSetSlaveAdd(i2cREG1, OBC_OPT1);
//	i2cSetDirection(i2cREG1, I2C_TRANSMITTER);
//	i2cSetCount(i2cREG1, 1);
//	i2cSetMode(i2cREG1, I2C_MASTER);
//	i2cSetStop(i2cREG1);
//	i2cSetStart(i2cREG1);
//	errcode = sfu_i2cSend(i2cREG1, 1, &cmd);
//	if (errcode != I2C_OK)
//		return errcode;
//
//	//read back the current. 14-8 = charge current
//	//6-0 = discharge current (I think I want this one)
//	cmd = (addr << 1 | RD_CMD);
//	i2cSetSlaveAdd(i2cREG1, OBC_IBAT);
//	i2cSetDirection(i2cREG1, I2C_TRANSMITTER);
//	i2cSetCount(i2cREG1, 1);
//	i2cSetMode(i2cREG1, I2C_MASTER);
//	i2cSetStop(i2cREG1);
//	i2cSetStart(i2cREG1);
//	errcode = sfu_i2cSend(i2cREG1, 1, &cmd);
//	if (errcode != I2C_OK)
//		return errcode;
//
//	i2cSetSlaveAdd(i2cREG1, OBC_IBAT);
//	i2cSetCount(i2cREG1, 2);//2 or 1 byte to receive, since I think I only want the discharge current, but it still sends 2?
//	i2cSetDirection(i2cREG1, I2C_RECEIVER);
//	i2cSetMode(i2cREG1, I2C_MASTER);
//	if (errcode != I2C_OK)
//		return errcode;
//	i2cSetStop(i2cREG1);
//	i2cClearSCD(i2cREG1);
//
//	//Something to convert into amps
//}

//void set_chrg(uint8_t addr) {
//	uint16_t cmd = //charge current - bits 12 - 6 set the limit
//	// Needs to be rewritten every 175 seconds?
//
//	i2cSetSlaveAdd(i2cREG1, OBC_ILIM);
//	i2cSetDirection(i2cREG1, I2C_TRANSMITTER);
//	i2cSetCount(i2cREG1, 1);
//	i2cSetMode(i2cREG1, I2C_MASTER);
//	i2cSetStop(i2cREG1);
//	i2cSetStart(i2cREG1);
//	errcode = sfu_i2cSend(i2cREG1, 1, &cmd);
//		if (errcode != I2C_OK)
//			return errcode;
//}
