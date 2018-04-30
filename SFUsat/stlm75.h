/*
 * stlm75.h
 *
 *  Created on: Apr 18, 2018
 *      Author: Richard
 *
 *      Usage: if temperature read functions return a large negative number (ex: -30077), there was an error
 *      		- calling function should trigger an entry into safe mode
 */

#ifndef SFUSAT_STLM75_H_
#define SFUSAT_STLM75_H_

// Chip addresses for all sensors in the satellite
#define OBC_TEMP 0x48
#define RF1_TEMP 0x49
#define RF2_TEMP 0x4A



//typedef struct g_i2cTransfer_t g_i2cTransfer_x;

uint16_t obc_temp_test();


int16_t read_temp(uint8_t addr);
void sfu_i2cInit(void);
static bool receiveByte(uint8_t * byte);
static bool sendByte(uint8_t byte);
static void waitWhileDeviceIsBusy(uint8_t slaveDeviceAddress);
void i2cNotification(i2cBASE_t *i2c, uint32_t flags);
void I2cDriver_read(uint8_t slaveDeviceAddress, uint32_t readAddress, uint8_t * readBuffer, uint32_t readLength);
void I2cDriver_write(uint8_t slaveDeviceAddress, uint32_t writeAddress, uint8_t const * writeBuffer, uint32_t writeLength);
#define I2C i2cREG1
#endif /* SFUSAT_STLM75_H_ */
