/*
 * sfu_rtc.c
 *
 *  Created on: Jun 12, 2017
 *      Author: steven + richard
 */

#include "gio.h"
#include "spi.h"

#include "sfu_rtc.h"
#include "sfu_hardwaredefs.h"

#include <assert.h>
#include <math.h>

SemaphoreHandle_t xRTCMutex;
spiDAT1_t rtc_spiConfig;
uint32_t epochOffset; // can be used to offset the epoch from the ground. Default 0. (seconds)

// Command modes
#define READ 0x80
#define WRITE 0x00

// RTC Registers
#define CTRL_1
#define CTRL_INT 0X01
#define CTRL_INTFLAG 0x02
#define CTRL_STATUS 0x03
#define CTRL_RESET 0x04
#define PON_REG 32
#define SYSR_REG 16
#define SR_REG 16
#define SRF_REG 16
#define V2F_REG 0x03
#define V1F_REG 0x02
#define CLOCK_SECONDS 0x08
#define CLOCK_MINUTES 0x09
#define CLOCK_HOURS 0x0A
#define CLOCK_DAYS 0x0B
#define CLOCK_WKDAYS 0x0C
#define CLOCK_MONTHS 0x0D
#define CLOCK_YEARS 0x0E
#define ALARM_SECOND 0x10
#define ALARM_MINUTE 0x11
#define ALARM_HOUR 0x12
#define ALARM_DAY 0x13
#define ALARM_WKDAY 0x14
#define ALARM_MONTH 0x15
#define ALARM_YEAR 0x16
#define TIMER_LOW 0x18
#define TIMER_HIGH 0x19
#define TEMP 0x20
#define EEPROM_USER1 0x28
#define EEPROM_USER2 0x29
#define EEPROM_CTRL 0x30
#define XTAL_OFFSET 0x31
#define XTAL_COEFF 0x32
#define XTAL_T0 0x33
#define TEMP_ENABLE 0x00
#define TEMP_INTERVAL 0x01 // 0 = 1sec, 1 = 16sec
#define TRICKLE_R80K 0x07
#define TRICKLE_R20K 0x06
#define TRICKLE_R5K 0x05
#define TRICKLE_R1K 0x04

// contains the cumulative seconds in months so we can easily calculate epochs
const uint32_t cumulativeSecondsInMonth[12] = { 0, 2678400, 5097600, 7776000, 10368000, 13046400, 15638400, 18316800,
		20995200, 23587200, 26265600, 28857600 }; // zero, seconds in january, then seconds in jan + feb, then jan + feb + mar
// in feb, we add seconds in jan plus the current time in feb to get the seconds since start

uint32_t rtc_epoch_time = 0;
unsigned int het_epoch_time = 0;

unsigned int getCurrentTime() {
	/*
	 * Get current HET time.
	 */
	//het_time = 0;
	/*
	 * Return "approximation" to actual RTC time.
	 */
	return rtc_epoch_time + het_epoch_time;
}

void tempAddSecondToHET() {
	het_epoch_time++;
}

void rtcInit() {
	// must call spiInit before this.
	xRTCMutex = xSemaphoreCreateMutex();
	rtc_spiConfig.CS_HOLD = RTC_CONFIG_CS_HOLD; //CS false = high during data transfer
	rtc_spiConfig.WDEL = RTC_CONFIG_WDEL; // wdelay
	rtc_spiConfig.DFSEL = RTC_CONFIG_DFSEL; // data format
	rtc_spiConfig.CSNR = RTC_CONFIG_CSNR; // chip select to use

	epochOffset = 0; // init the offset to zero. This can be set from the ground if we need to fudge the epoch for some reason. TODO: read from nonvolatile

	gioSetBit(RTC_CS_PORT, RTC_CS_PIN, 0); // using PA1 as the CS line since the RTC wants active HIGH CS. Hercules won't let us change CS polarity, which is why we need to do it with GIO.

	rtcReset(); // zero out the time and everything

	// rudimentary checking and time reset on extended power loss
	if (rtcGetBit(CTRL_STATUS, PON_REG)) { // this means we are powering up first time, or time info is otherwise corrupted
		rtcSetBit(CTRL_STATUS, PON_REG, 0); // clear this bit
		rtcResetTime();
		// save time info so we can look at it later?
	}

	if (rtcGetBit(CTRL_STATUS, V2F_REG)) { // low voltage
		rtcSetBit(CTRL_STATUS, V2F_REG, 0);
		rtcResetTime();
	}

	rtcSetBit(EEPROM_CTRL, TRICKLE_R1K, 1); // enable 1.5k ohm trickle charge resistor
}

void rtcReset() {
	rtcWriteRegister(CTRL_RESET, SYSR_REG);
	// once complete, bit 4 of CTRL_STATUS will be set
	// it needs a short delay for the reset to take place
}
void rtcResetTime() {
	rtcWriteRegister(CLOCK_SECONDS, 0x00);
	rtcWriteRegister(CLOCK_MINUTES, 0x00);
	rtcWriteRegister(CLOCK_HOURS, 0x00);
	rtcWriteRegister(CLOCK_DAYS, 0x01);
	rtcWriteRegister(CLOCK_WKDAYS, 0x01);
	rtcWriteRegister(CLOCK_MONTHS, 0x01);
	rtcWriteRegister(CLOCK_YEARS, 0x00);
}

uint8_t rtc_get_startup_status() {
	if (rtcGetBit(CTRL_STATUS, PON_REG)) { // this means we are powering up first time, or time info is otherwise corrupted
		rtcSetBit(CTRL_STATUS, PON_REG, 0); // clear this bit
		// save time info so we can look at it later?
		rtcResetTime();
	}

	// ---------- check reset status
	if (rtcGetBit(CTRL_STATUS, SR_REG)) { // system reset has been triggered (manual or auto)
		rtcSetBit(CTRL_STATUS, SR_REG, 0); // clear this bit
		// save time info so we can look at it later?
		if (rtcGetBit(CTRL_INT, SRF_REG)) {
			// it was a self recovery reset
		} else {
			// it was a manual reset
		}
	}

	// ckeck vlow 1 and vlow 2 (1 is nbd) (2 is potentially bad)
	if (rtcGetBit(CTRL_STATUS, V2F_REG)) {
		rtcResetTime();
	}
	if (rtcGetBit(CTRL_STATUS, V1F_REG)) {
		// time might be messed up,
	}
	return 0; // change this based on what it detects has gone wrong
}

uint32_t getEpochOffset() {
	return epochOffset;
}
uint32_t setEpochOffset(uint32_t inputOffset) {
	epochOffset = inputOffset;
	return epochOffset;
}

//uint8_t convertBCD(uint8_t input) {
//	// the time data from the rtc is in BCD format, https://stackoverflow.com/questions/28133020/how-to-convert-bcd-to-decimal
//	assert(((input & 0xF0) >> 4) < 10);  // More significant nybble is valid
//	assert((input & 0x0F) < 10);         // Less significant nybble is valid
//	return ((input & 0xF0) >> 4) * 10 + (input & 0x0F);
//}

//uint8_t convertBCD(uint8_t input) {
//    uint8_t nResult=0;
//    int32_t ncnt,anHexValueStored[8];
//    uint16_t unflag=0;
//
//    for(ncnt=7 ;ncnt>=0 ; ncnt--){
//        anHexValueStored[ncnt]=input & (0x0000000f << 4*(7-ncnt));
//        anHexValueStored[ncnt]=anHexValueStored[ncnt] >> 4*(7-ncnt);
//        if(anHexValueStored[ncnt]>9)
//        unflag=1;
//    }
//    if(unflag==1){
//        return 0;
//    }
//    else{
//        for(ncnt=0 ;ncnt<8 ; ncnt++)
//        nResult= nResult +anHexValueStored[ncnt]*pow(10,(7-ncnt));
//        return nResult;
//    }
//}
uint8_t convertBCD(uint8_t x) {
	return x - 6 * (x >> 4);
}

uint8_t rtc_get_seconds(void) {
	return convertBCD(rtcReadRegister(CLOCK_SECONDS));
}

uint8_t rtc_get_minutes(void) {
	return convertBCD(rtcReadRegister(CLOCK_MINUTES));

	//    uint16_t txBuffer[] = {READ | CLOCK_MINUTES};
	//    uint16_t rxBuffer[1] = {0x00};
	//    rtcTransmitAndReceive(1, txBuffer, rxBuffer);
	//    return MSBconvertBCD(rxBuffer);
}

uint8_t rtc_get_hours(void) {
	return convertBCD(rtcReadRegister(CLOCK_HOURS));
}

uint8_t rtc_get_month(void) {
	uint8_t month = convertBCD(rtcReadRegister(CLOCK_MONTHS));
	if (month == 0) { // it likes to set this to zero for some reason ... needs to be investigated further
		month = 1;
	}
	return month;
}
uint8_t rtc_get_day(void) {
	uint8_t day = convertBCD(rtcReadRegister(CLOCK_DAYS));
	if (day == 0) { // it likes to set this to zero for some reason ... needs to be investigated further
		day = 1;
	}
	return day;
}

uint8_t rtc_get_year(void) {
	return convertBCD(rtcReadRegister(CLOCK_YEARS));
}

uint32_t getCurrentRTCTime() {
	/*
	 * Reset HET time.
	 */
	xSemaphoreTake(xRTCMutex, pdMS_TO_TICKS(200));
	{
		het_epoch_time = 0;

		/*
		 * Initiate RTC SPI transfers to get current actual rtc_epoch_time.
		 */
		// Note: doesn't handle more than 1 leap year!
		// also assumes we start on jan 1, year 2000 @ RTC power on (which we do)
		uint8_t year = rtc_get_year(); // save these since we need them multiple times and we don't want them to change between reads
		uint8_t month = rtc_get_month();
		// For debuggin
		//    uint32_t secmonth = cumulativeSecondsInMonth[(rtc_get_month()-1)];
		//    uint32_t ymult = year*31536000;
		//    uint32_t straightday = rtc_get_day();
		//    uint32_t multday = ((rtc_get_day() - 1)*86400);
		//    uint32_t multhours = (rtc_get_hours()*3600);
		//    uint32_t multmins = (rtc_get_minutes()*60);
		//    uint32_t secs = (uint32_t)rtc_get_seconds();

		/**
		 * Upper bound the month received from RTC SPI to 11, just
		 * in case, since if we index with anything >= 12 we will explode.
		 * TODO: check if the convertBCD operations in rtc_get_month
		 * negate the need for this check.
		 */
		uint32_t monthIdx = month - 1;
		monthIdx = monthIdx >= 12 ? 11 : monthIdx;
		uint32_t nonLeapYear = epochOffset + (year * 31536000) + (cumulativeSecondsInMonth[monthIdx])
				+ ((rtc_get_day() - 1) * 86400) + (rtc_get_hours() * 3600) + (rtc_get_minutes() * 60)
				+ rtc_get_seconds();

		rtc_epoch_time = nonLeapYear;

		if ((month >= 3 && year == 0) || (year >> 0)) {
			// normally consider feb as having 28 days. However, RTC does 29-day feb on years divisible by 4, and year 0
			// so if we're in year 0 or more and past feb, we need to add that leap day.
			// Also if we're over year 4, past feb, we need to add another one. On feb 29, day will be auto-included because current time is RTC day * seconds per day.
			rtc_epoch_time += 86400;
		}
	}
	xSemaphoreGive(xRTCMutex);
	return rtc_epoch_time;
}

uint32_t no_rtos_test_getCurrentRTCTime() {
	/*
	 * Reset HET time.
	 */
	// no mutex on this so we can test without the rtos
	het_epoch_time = 0;

	/*
	 * Initiate RTC SPI transfers to get current actual rtc_epoch_time.
	 */
	// Note: doesn't handle more than 1 leap year!
	// also assumes we start on jan 1, year 2000 @ RTC power on (which we do)
	uint8_t year = rtc_get_year(); // save these since we need them multiple times and we don't want them to change between reads
	uint8_t month = rtc_get_month();

	/**
	 * Upper bound the month received from RTC SPI to 11, just
	 * in case, since if we index with anything >= 12 we will explode.
	 * TODO: check if the convertBCD operations in rtc_get_month
	 * negate the need for this check.
	 */
	uint32_t monthIdx = month - 1;
	monthIdx = monthIdx >= 12 ? 11 : monthIdx;
	uint32_t nonLeapYear = epochOffset + (year * 31536000) + (cumulativeSecondsInMonth[monthIdx])
			+ ((rtc_get_day() - 1) * 86400) + (rtc_get_hours() * 3600) + (rtc_get_minutes() * 60) + rtc_get_seconds();

	rtc_epoch_time = nonLeapYear;

	if ((month >= 3 && year == 0) || (year >> 0)) {
		// normally consider feb as having 28 days. However, RTC does 29-day feb on years divisible by 4, and year 0
		// so if we're in year 0 or more and past feb, we need to add that leap day.
		// Also if we're over year 4, past feb, we need to add another one. On feb 29, day will be auto-included because current time is RTC day * seconds per day.
		rtc_epoch_time += 86400;
	}

	return rtc_epoch_time;
}

// ------------------------ SPI HANDLERS -----------------------------

void rtcWriteRegister(uint16_t registerToWrite, uint16_t valueToWrite) {
	uint16_t txBuffer[2] = { WRITE | registerToWrite, valueToWrite };
	gioSetBit(RTC_CS_PORT, RTC_CS_PIN, 1); // go high because we're about to send stuff
	spiTransmitData(RTC_SPI_REG, &rtc_spiConfig, 2, txBuffer);
	gioSetBit(RTC_CS_PORT, RTC_CS_PIN, 0); // deactivate CS
}

uint8_t rtcReadRegister(uint16_t registerToRead) {
	uint16_t txBuffer[1] = { READ | registerToRead };
	uint16_t rxBuffer[1] = { 0x0000 };
	rtcTransmitAndReceive(1, txBuffer, rxBuffer);

	return (uint8_t) (rxBuffer[0] & 0x00ff); // return LSB since the spi peripheral uses 16-bit words but the RTC only sends back 8-bits
}

void rtcTransmitAndReceive(uint32 blocksize, uint16 * srcbuff, uint16 * destbuff) {

	//    Usage:
	//uint16_t txBuffer[] = {READ | CLOCK_MINUTES};
	//    uint16_t rxBuffer[1] = {0x00};
	//    spiTransmitAndReceiveData(RTC_SPI_REG, &rtc_spiConfig, 2, txBuffer, rxBuffer); // middle argument must be 2 or it doesn't go into receive

	// this is required because the RTC chip select is active high (nonstandard polarity)
	gioSetBit(RTC_CS_PORT, RTC_CS_PIN, 1); // go high because we're about to send stuff
	spiTransmitData(RTC_SPI_REG, &rtc_spiConfig, blocksize, srcbuff);
	spiReceiveData(RTC_SPI_REG, &rtc_spiConfig, blocksize, destbuff);
	gioSetBit(RTC_CS_PORT, RTC_CS_PIN, 0); // deactivate CS
}

uint8_t rtcGetBit(uint16_t registerToRead, uint8_t bitToCheck) {
	uint8_t rtcbits = rtcReadRegister(registerToRead);
	return (rtcbits & bitToCheck) == bitToCheck;
}

uint8_t rtcSetBit(uint16_t registerToWrite, uint8_t bitToSet, uint8_t bitValue) {
	// bitToSet is zero indexed
	uint8_t rtcbits = rtcReadRegister(registerToWrite);
	if (bitValue == 1) {
		rtcbits |= (1 << bitToSet);
	} else {
		rtcbits &= ~(1 << bitToSet);
	}
//	rtcbits ^= (-bitValue ^ rtcbits) & bitToSet; // this doesn't work correctly!
	uint16_t sendoff = 0x00ff & rtcbits; // spi handlers like 16-bits, so right align the whole thing
	rtcWriteRegister(registerToWrite, sendoff);

	return rtcbits;
}

