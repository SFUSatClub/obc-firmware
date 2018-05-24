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

#define RD_CMD 0x1
#define WR_CMD 0x0

int16_t read_batt(uint8_t addr) {
	if (xSemaphoreTake(xI2CMutex, pdMS_TO_TICKS(500) ) == pdTRUE) {
		int16_t batt_chrg;
		batt_chrg = read_batt_raw(OBC_BATT);
		if (batt_chrg ) {
			//error
			//what's the conditions?
		}
		xSemaphoreGive(xI2CMutex);
		return batt_chrg;
	}
	else {
		serialSendQ("Battery charge read can't get mutex");
		return //error
	}
}

int16_t read_batt_raw(uint8_t addr) {
	uint8_t data[2] = {'\0'}; //set second bit to 0? and why?
	uint8_t cmd = (addr << 1 | WR_CMD);
	//I want to set the ADC to do a oneshot or continuous update first?
	//[15] 1-shot = 0, continuous = 1
	//[14] start = 1
	//[7-0] = ?? I think I want [1] = 1 for VSYS? other bits are 0?
	int16_t batt_chrg_v = 0;
	int16_t errcode = 0;

	//Need to write to configure ADC first?
	i2cSetSlaveAdd(i2cREG1, OBC_ADC);
	i2cSetDirection(i2cREG1, I2C_TRANSMITTER);
	i2cSetCount(i2cREG1, 1);
	i2cSetMode(i2cREG1, I2C_MASTER);
	i2cSetStop(i2cREG1);
	i2cSetStart(i2cREG1);
	errcode = sfu_i2cSend(i2cREG1, 1, &cmd;); //how does this work
	if (errcode != I2C_OK)
		return errcode;

	//Then read voltage from the battery
	cmd = (addr << 1 | RD_CMD);
	i2cSetSlaveAdd(i2cREG1, OBC_BATT);
	i2cSetDirection(i2cREG1, I2C_TRANSMITTER);
	i2cSetCount(i2cREG1, 1);
	i2cSetMode(i2cREG1, I2C_MASTER);
	i2cSetStop(i2cREG1);
	i2cSetStart(i2cREG1);
	errcode = sfu_i2cSend(i2cREG1, 1, &cmd;);
	if (errcode != I2C_OK)
		return errcode;

	i2cSetSlaveAdd(i2cREG1, OBC_BATT);
	i2cSetCount(i2cREG1, 2); //we read 16 bits or 8? 15-8 is sys voltage, 7-0 is batt voltage..
	i2cSetDirection(i2cREG1, I2C_RECEIVER);
	i2cSetMode(i2cREG1, I2C_MASTER);
	errcode = sfu_i2cSend(i2cREG1, 2, data;);
	if (errcode != I2C_OK)
		return errcode;
	i2cSetStop(i2cREG1);
	i2cClearSCD(i2cREG1);

	errcode = sfu_is_bus_busy();
	if (errcode != I2C_OK)
		return errcode;

	errcode = sfu_ok_transmit();
	if (errcode != I2C_OK)
		return errcode;

	//Need something to convert the raw data into volts?
	return batt_chrg_v;



	//Read current from the battery (Should this be separate?)
	cmd = (addr << 1 | WR_CMD); //I want to set [15] to 0 to disable low power mode?
	i2cSetSlaveAdd(i2cREG1, OBC_OPT0);
	i2cSetDirection(i2cREG1, I2C_TRANSMITTER);
	i2cSetCount(i2cREG1, 1);
	i2cSetMode(i2cREG1, I2C_MASTER);
	i2cSetStop(i2cREG1);
	i2cSetStart(i2cREG1);
	errcode = sfu_i2cSend(i2cREG1, 1, &cmd);
	if (errcode != I2C_OK)
		return errcode;

	i2cSetSlaveAdd(i2cREG1, OBC_OPT1);
	i2cSetDirection(i2cREG1, I2C_TRANSMITTER);
	i2cSetCount(i2cREG1, 1);
	i2cSetMode(i2cREG1, I2C_MASTER);
	i2cSetStop(i2cREG1);
	i2cSetStart(i2cREG1);
	errcode = sfu_i2cSend(i2cREG1, 1, &cmd);
	if (errcode != I2C_OK)
		return errcode;

	//read back the current. 14-8 = charge current
	//6-0 = discharge current (I think I want this one)
	cmd = (addr << 1 | RD_CMD);
	i2cSetSlaveAdd(i2cREG1, OBC_IBAT);
	i2cSetDirection(i2cREG1, I2C_TRANSMITTER);
	i2cSetCount(i2cREG1, 1);
	i2cSetMode(i2cREG1, I2C_MASTER);
	i2cSetStop(i2cREG1);
	i2cSetStart(i2cREG1);
	errcode = sfu_i2cSend(i2cREG1, 1, &cmd);
	if (errcode != I2C_OK)
		return errcode;

	i2cSetSlaveAdd(i2cREG1, OBC_IBAT);
	i2cSetCount(i2cREG1, 2);//2 or 1 byte to receive, since I think I only want the discharge current, but it still sends 2?
	i2cSetDirection(i2cREG1, I2C_RECEIVER);
	i2cSetMode(i2cREG1, I2C_MASTER);
	if (errcode != I2C_OK)
		return errcode;
	i2cSetStop(i2cREG1);
	i2cClearSCD(i2cREG1);

	//Something to convert into amps
}

void set_chrg(uint8_t addr) {
	uint16_t cmd = //charge current - bits 12 - 6 set the limit
	// Needs to be rewritten every 175 seconds?

	i2cSetSlaveAdd(i2cREG1, OBC_ILIM);
	i2cSetDirection(i2cREG1, I2C_TRANSMITTER);
	i2cSetCount(i2cREG1, 1);
	i2cSetMode(i2cREG1, I2C_MASTER);
	i2cSetStop(i2cREG1);
	i2cSetStart(i2cREG1);
	errcode = sfu_i2cSend(i2cREG1, 1, &cmd);
		if (errcode != I2C_OK)
			return errcode;
}
