/*
 * stlm75.h
 *
 *  Created on: Apr 18, 2018
 *      Author: Richard
 */

#ifndef SFUSAT_STLM75_H_
#define SFUSAT_STLM75_H_

// Chip addresses for all sensors in the satellite
#define OBC_TEMP 0x48
#define RF1_TEMP 0x49
#define RF2_TEMP 0x4A

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

//typedef struct g_i2cTransfer_x
//{
//    uint32  mode;
//    uint32  length;
//    uint8   * data;
//} g_i2cTransfer_t;

//typedef struct g_i2cTransfer_t g_i2cTransfer_x;

uint16_t obc_temp_test();
void hcg_test();
#define DATA_COUNT 3


uint16_t read_temp(uint8_t addr);
void sfu_i2cInit(void);
static bool receiveByte(uint8_t * byte);
static bool sendByte(uint8_t byte);
static void waitWhileDeviceIsBusy(uint8_t slaveDeviceAddress);
void i2cNotification(i2cBASE_t *i2c, uint32_t flags);
void I2cDriver_read(uint8_t slaveDeviceAddress, uint32_t readAddress, uint8_t * readBuffer, uint32_t readLength);
void I2cDriver_write(uint8_t slaveDeviceAddress, uint32_t writeAddress, uint8_t const * writeBuffer, uint32_t writeLength);
#define I2C i2cREG1
#endif /* SFUSAT_STLM75_H_ */
