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
	i2cSend(i2cREG1, 1,&cmd);

	i2cSetMode(i2cREG1, I2C_MASTER);
	i2cSetDirection(i2cREG1, I2C_TRANSMITTER);
	i2cSetSlaveAdd(i2cREG1, BATT_CHRG);
	i2cSetCount(i2cREG1, 1);
	i2cSetStop(i2cREG1);
	i2cSetStart(i2cREG1);
	i2cSend(i2cREG1, 1,&cmd);

	while(!i2cIsBusBusy(i2cREG1)){}	// RA: do we need this?

	i2cSetSlaveAdd(i2cREG1, BATT_CHRG);
	i2cSetCount(i2cREG1, 1); 							// 1 is good for 8-bit register read
	i2cSetDirection(i2cREG1, I2C_RECEIVER);
	i2cSetMode(i2cREG1, I2C_MASTER);
	sfu_i2cReceive(i2cREG1, 1, data);
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
	i2cSend(i2cREG1, 2,&cmd[0]);
	i2cSetStop(i2cREG1);
//	i2cClearSCD(i2cREG1);

	i2cSetDirection(i2cREG1, I2C_TRANSMITTER);
	i2cSetMode(i2cREG1, I2C_MASTER);
	i2cSetSlaveAdd(i2cREG1, BATT_CHRG);
	i2cSetCount(i2cREG1,3);
	i2cSetStart(i2cREG1);
	i2cSend(i2cREG1, 2,&cmd[0]);
	i2cSetStop(i2cREG1);
	i2cClearSCD(i2cREG1);
	// RA: this is the hack to get the BMS to talk properly
	i2cInit();
	return I2C_OK;
}

int8_t read_batt_volt_raw(uint8_t addr) {
	uint8_t ADCOPT_MSB;
	uint8_t ADCOPT_LSB;
	uint8_t batt_volt_raw;
	uint8_t batt_volt;

	//configure ADC first
	//Do I need to enable all of EN_ADC_VBAT, EN_ADC_ICHG, EN_ADC_IDCHG if I only want voltage?
	read_register(ADCOPTION_REG_LSB, *ADCOPT_LSB);
	if (!(ADCOPT_LSB & ADCOPT_EN_ADC_VBAT)) {		//If the ADC_VBAT bit is 0 then need to set it to 1
		ADCOPT_LSB = ADCOPT_LSB | ADCOPT_EN_ADC_VBAT;
		write_register(ADCOPTION_REG_LSB, *ADCOPT_LSB);
	}


	//mask and shift to read the ADC_CONV bit, set to 0 for one-shot update
	read_register(ADCOPTION_REG_MSB, *ADCOPT_MSB);
	if (!((ADCOPT_LSB & ADCOPT_ADC_CONV_BIT) >> 7)) {
		ADCOPT_MSB = ADCOPT_MSB & ~ADCOPT_ADC_CONV_BIT;
	}

	//start ADC conversion
	ADCOPT_MSB = ADCOPT_MSB | ADCOPT_ADC_START_BIT;
	write_register (ADCOPTION_REG_MSB, *ADCOPT_MSB);

	//Read voltage
	read_register(ADCVSYSVBAT_REG, *batt_volt_raw);
	for (i = 0; i <= 7; i = i + 1) {
		batt_volt = batt_volt + (batt_volt_raw[i] * 0.064 * (i + 1));
	}
	return batt_volt;
}

int8_t read_volt(uint8_t addr) {
	if (xSemaphoreTake(xI2CMutex, pdMS_TO_TICKS(500) ) == pdTRUE) {
		uint8_t batt_volt;
		batt_volt = read_volt_raw(OBC_BATT);
		if (batt_volt < 0) {
			//error
			//what are the conditions?
		}
		xSemaphoreGive(xI2CMutex);
		return batt_volt;
	}
	else {
		serialSendQ("Battery voltage read can't get mutex");
		return 0; //error
	}
}

int16_t read_curr_raw(uint8_t addr) {
	uint8_t ADCOPT_MSB;
	uint8_t ADCOPT_LSB;
	uint8_t OPT0_REG;
	uint8_t OPT1_REG;
	uint8_t batt_curr_raw;
	uint8_t batt_curr;

	//configure ADC first
	//Do I need to enable all of EN_ADC_VBAT, EN_ADC_ICHG, EN_ADC_IDCHG? Only want discharge current from battery?
	read_register(ADC_OPTION_REG_LSB, *ADCOPT_LSB);
	if (!((ADCOPT_LSB & ADCOPT_EN_ADC_IDCHG) >> 3)) {
		ADCOPT_LSB = ADCOPT_LSB | ADCOPT_EN_ADC_IDCHG;
		write_register(ADCOPTION_REG_LSB, *ADCOPT_LSB);
	}

	//Disable low power mode
	read_register(CHARGEOPT0_REG, *OPT0_REG);
	if ((OPT0_REG & CHARGEOPT0_EN_LWPWR) >> 7) {
		OPT0_REG = OPT0_REG & ~CHARGEOPT0_EN_LWPWR;
		write_register(CHARGEOPT0_REG, *OPT0_REG);
		//Some way to remember that low power was enabled? (set flag?)
	}

	//Enable battery current measurement
	read_register(CHARGEOPT1_REG, *OPT1_REG);
	if (!((OPT1_REG & CHARGEOPT1_EN_IBAT) >> 7)) {
		OPT1_REG = OPT1_REG | CHARGEOPT1_EN_IBAT;
		write_register(CHARGEOPT1_REG, *OPT1_REG);
	}

	//Set one-shot update
	read_register(ADCOPTION_REG_MSB, *ADCOPT_MSB);
	if (!((ADCOPT_LSB & ADCOPT_ADC_CONV_BIT) >> 7)) {
		ADCOPT_MSB = ADCOPT_MSB & ~ADCOPT_ADC_CONV_BIT;
	}

	//start ADC conversion
	ADCOPT_MSB = ADCOPT_MSB | ADCOPT_ADC_START_BIT;
	write_register (ADCOPTION_REG_MSB, *ADCOPT_MSB);

	//Read the discharge current from the battery
	read_register(ADCIBAT_REG, *batt_curr_raw);
	for (i = 0; i <= 7; i = i + 1) {
			batt_curr = batt_curr + (batt_curr_raw[i] * 0.256 * (i + 1));
		}
	return batt_curr;
}

int8_t read_curr(uint8_t addr) {
	if (xSemaphoreTake(xI2CMutex, pdMS_TO_TICKS(500) ) == pdTRUE) {
		uint8_t batt_curr;
		batt_volt = read_volt_raw(OBC_BATT);
		if (batt_curr < 0) {
			//error
			//what are the conditions?
		}
		xSemaphoreGive(xI2CMutex);
		return batt_curr;
	}
	else {
		serialSendQ("Battery voltage read can't get mutex");
		return 0; //error
	}
}

void set_chrg(uint8_t addr) {
	//Just set the current limit to begin charging
	//Bits 13-7 (MSB bits 4-0, LSB bits 7-5)
	uint8_t ILIM_MSB;
	uint8_t ILIM_LSB;

	read_register(OBC_ILIM_MSB, *ILIM_MSB);

	write_register(OBC_ILIM_MSB, *ILIM_MSB);




	read_register(OBC_ILIM_LSB, *ILIM_LSB);

	write_register(OBC_ILIM_LSB, *ILIM_LSB);
}
