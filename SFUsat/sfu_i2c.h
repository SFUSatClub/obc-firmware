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
#define I2C_TIMEOUT_MAX 	200000		// number of attempts before timing out (units are attempts at register check)
#define I2C_TIMEOUT_FAIL 	-77			// if we timeout while waiting for something
#define I2C_ERR_NACK 		-66			// if we receive an unexpected NACK
#define I2C_OK 				1			// I2C functions return this when everything is good

int8_t sfu_i2cSend(i2cBASE_t *i2c, uint32 length, uint8 * data);
int8_t sfu_i2cReceive(i2cBASE_t *i2c, uint32 length, uint8 * data);
int8_t sfu_is_bus_busy();


#endif /* SFUSAT_SFU_I2C_H_ */
