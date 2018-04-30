/*
 * sfu_i2c.h
 *
 *  Created on: Apr 22, 2018
 *      Author: Richard
 */

#ifndef SFUSAT_SFU_I2C_H_
#define SFUSAT_SFU_I2C_H_

#include "i2c.h"
#define I2C i2cREG1

// I2C error handling
#define I2C_TIMEOUT_MAX 	200000			// number of attempts before timing out (units are attempts at register check)
#define I2C_TIMEOUT_FAIL 	-30077			// if we timeout while waiting for something
#define I2C_ERR_NACK 		-30066			// if we receive an unexpected NACK
#define I2C_BUSBUSY_FAIL	-30055			// bus is busy (both lines aren't high)
#define I2C_TXREADY_FAIL	-30044			// bit indicating we're ok to transmit again did not go low
#define I2C_OK 				1			// I2C functions return this when everything is good

int16_t sfu_i2cSend(i2cBASE_t *i2c, uint32 length, uint8 * data);
int16_t sfu_i2cReceive(i2cBASE_t *i2c, uint32 length, uint8 * data);
int16_t sfu_is_bus_busy();
int16_t sfu_ok_transmit();

#endif /* SFUSAT_SFU_I2C_H_ */
