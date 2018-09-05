/*
 * I2C_Protocol.h
 *
 *  Created on: Jun 25, 2018
 *      Author: Derrick
 */

#ifndef SFUSAT_I2C_FORUM_TEST_H_
#define SFUSAT_I2C_FORUM_TEST_H_


void I2cDriver_init(void);

void I2cDriver_write(uint8 slaveDeviceAddress, uint32 writeAddress, uint8 const * writeBuffer, uint32 writeLength);

void I2cDriver_read(uint8 slaveDeviceAddress, uint32 readAddress, uint8 * readBuffer, uint32 readLength);

void i2cNotification(i2cBASE_t *i2c, uint32 flags);

static void waitWhileDeviceIsBusy(uint8 slaveDeviceAddress);

static boolean sendByte(uint8 byte);

static boolean receiveByte(uint8 * byte);

void i2cInit_1(void);



#endif /* SFUSAT_I2C_FORUM_TEST_H_ */
