/*
 * sfu_i2c.h
 *
 *  Created on: Apr 22, 2018
 *      Author: Richard
 *
 *      - Wrap these functions with a mutex wherever you call them
 *      - Use the read_temp function in stlm75.c as an example of how to use I2C
 *      - Main goal of the SFUSat adaptations is to ensure that we never infinite loop
 *      - DO NOT use the halcogen functions that do anything more than set or read a register.
 *      	- Complicated halcogen functions will wait forever for things to happen and lock up the system.
 *
 *      ERROR HANDLING
 *      - calling functions are responsible for dealing with errors. Note that errors need an int16_t to be contained properly.
 *      - errors should be logged in our system, and retries may optionally be attempted
 *      - retry attempts must be limited
 *
 *
 */

#ifndef SFUSAT_SFU_I2C_H_
#define SFUSAT_SFU_I2C_H_

#include "i2c.h"
#include "FreeRTOS.h"
#include "rtos_semphr.h"

extern SemaphoreHandle_t xI2CMutex;

#define I2C i2cREG1

// I2C error handling
#define I2C_TIMEOUT_MAX 	200000			// number of attempts before timing out (units are attempts at register check), typically takes ~3500 for a correct wait
#define I2C_TIMEOUT_FAIL 	-30077			// if we timeout while waiting for something
#define I2C_ERR_NACK 		-30066			// if we receive an unexpected NACK
#define I2C_BUSBUSY_FAIL	-30055			// bus is busy (both lines aren't high)
#define I2C_TXREADY_FAIL	-30044			// bit indicating we're ok to transmit again did not go low
#define I2C_OK 				1				// I2C functions return this when everything is good

void sfu_i2c_init();
int16_t sfu_i2cSend(i2cBASE_t *i2c, uint32 length, uint8 * data);
int16_t sfu_i2cReceive(i2cBASE_t *i2c, uint32 length, uint8 * data);
int16_t sfu_is_bus_busy();
int16_t sfu_ok_transmit();
int16_t BMS_i2c_send(i2cBASE_t *i2c, uint32 length, uint8 * data);
int16_t BMS_i2c_Receive(i2cBASE_t *i2c, uint32 length, uint8 * data);

int16_t sfu_reset_i2c(i2cBASE_t *i2c);

#endif /* SFUSAT_SFU_I2C_H_ */
