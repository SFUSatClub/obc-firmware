/*
 * stlm75.c
 *
 *  Created on: Apr 18, 2018
 *      Author: Richard
 *
 *      Driver for STLM75 temperature sensor
 *
 *      Primary function: read_temp()
 *
 */
#include "i2c.h"
#include "stlm75.h"
#include "reg_i2c.h"
#include "obc_i2c.h"
#include "FreeRTOS.h"
#include "rtos_semphr.h"
#include "rtos_mpu_wrappers.h"
#include "obc_uart.h"
#include "obc_task_logging.h"

/* To access a register of the device, you write the register pointer bits in the command/pointer
 * register (lowest 2 bits). This register holds its value between set operations, so you only
 * set it when you want to start working with a different register.
 */

// REGISTER ADDRESSES
#define TEMP_REG 0x00
#define CONFIG_REG 0x01
#define HYST_EG 0x02
#define OVTEMP 0x03

// CHIP COMMANDS
#define RD_CMD 0x1
#define WR_CMD 0x0

/* Read_temp
 * 	- this is the main temperature reading function
 * 	- pass it the correct temp sensor address (in stlm75.h)
 * 	- be sure to check for values -999 and less in the calling function
 * 	- largely negative results indicate a particular error
 */

int16_t read_temp(uint8_t addr) {
	if (xSemaphoreTake(xI2CMutex, pdMS_TO_TICKS(500) ) == pdTRUE) {
		int16_t temp;
		temp = read_temp_raw(addr);
		if (temp <= -20000) {
//			sfu_reset_i2c(i2cREG1);
		}
		xSemaphoreGive(xI2CMutex);
		return temp;
	} else {
		serialSendln("Temp read can't get mutex");
		addLogItem(logtype_driver, error_1);

//		xSemaphoreGive(xI2CMutex);
		return TEMP_READ_ERROR;
	}

}

/* sanity check that the OBC temp sensor returns something
 * - not for flight
 * */
int16_t obc_temp_test_no_rtos() {
	int16_t temp;
	temp = read_temp_raw(OBC_TEMP);
	if (temp <= -20000) {
		sfu_reset_i2c(i2cREG1);
	}
	return temp;
}

/* Read temperature
 * - from page 24 of datasheet:
 * - Send out: [address 7:1][0], [0] means read
 * - STLM 75 will return two bytes, left aligned ugh
 * - details about scaling were taken from here: https://github.com/jfitter/LM75/blob/master/LM75.cpp
 *
 * - Will return an I2C error code if one happens.
 * - Calling function is responsible for handling the error (logging, retrying, etc.).
 */
int16_t read_temp_raw(uint8_t addr) {
	uint8_t data[2] = { '\0' };
	uint8_t cmd = (addr << 1 | RD_CMD);
	int16_t temp_deg_c = 0;
	int16_t errcode = 0;

	i2cSetSlaveAdd(i2cREG1, addr);
	i2cSetDirection(i2cREG1, I2C_TRANSMITTER);
	i2cSetCount(i2cREG1, 1); 							// the total number of bytes to transact before sending stop bit
	i2cSetMode(i2cREG1, I2C_MASTER);
	i2cSetStop(i2cREG1);
	i2cSetStart(i2cREG1);
	errcode = sfu_i2cSend(i2cREG1, 1, &cmd);
	if (errcode != I2C_OK)
		return errcode;

	i2cSetSlaveAdd(i2cREG1, addr);
	i2cSetCount(i2cREG1, 2); 							// the total number of bytes to transact before sending stop bit
	i2cSetDirection(i2cREG1, I2C_RECEIVER);
	i2cSetMode(i2cREG1, I2C_MASTER);
	errcode = sfu_i2cReceive(i2cREG1, 2, data);
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

	temp_deg_c = data[0] << 8; 									// MSBits
	temp_deg_c = temp_deg_c | data[1]; 							// LSBits
	temp_deg_c = ((temp_deg_c & 0xFF10) >> 5) * 0.125; // mask off in case there's garbage, shift over so we're correctly right-aligned, scale due to 0.125ºC resolution
	return temp_deg_c;
}

