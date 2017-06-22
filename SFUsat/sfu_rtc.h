/*
 * sfu_rtc.h
 *
 *  Created on: Jun 12, 2017
 *      Author: steven
 */

#ifndef SFUSAT_SFU_RTC_H_
#define SFUSAT_SFU_RTC_H_
#include "spi.h"
#include "sfu_hardwaredefs.h"

/**
 * Get "approximated" RTC epoch time.
 * Combines last actual time from RTC with time from Hercule's HET.
 * @return seconds
 */
unsigned int getTime();

/**
 * Get RTC epoch time.
 * RTC time is treated as ground truth.
 * Initiates a SPI transaction.
 * @return
 */
unsigned int getRTCTime();

void tempAddSecondToHET();


// -------------------- Additions by Richard ---------------

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

// Not included: USER RAM registers

// Command modes
#define READ 0x80
#define WRITE 0x00

// variables
spiDAT1_t rtc_spiConfig; // holds the SPI config (set in rtcInit)
extern const uint32_t cumulativeSecondsInMonth[12]; // contains the cumulative seconds in months so we can easily calculate epochs
static uint32_t epochOffset; // can be used to offset the epoch from the ground. Default 0. (seconds)

// functions
void rtcInit();
uint8_t rtc_get_seconds();
uint8_t rtc_get_minutes();
uint8_t rtc_get_hours();
uint8_t rtc_get_day();
uint8_t rtc_get_month();
uint8_t rtc_get_year();
uint32_t getEpoch();
uint32_t getEpochOffset();
uint32_t setEpochOffset(uint32_t inputOffset);
uint8_t rtc_get_startup_status();

uint8_t convertBCD(uint8_t input);

void rtcReset(); // triggers the reset register (does not affect time)
void rtcResetTime(); // resets time to jan 1, year 2000
void rtcWriteRegister(uint16_t registerToWrite, uint16_t valueToWrite);
uint8_t rtcReadRegister(uint16_t registerToRead);
uint8_t rtcGetBit(uint16_t registerToRead, uint8_t bitToCheck);
uint8_t rtcSetBit(uint16_t registerToWrite, uint8_t bitToSet, uint8_t bitValue);
void rtcTransmitAndReceive(uint32 blocksize, uint16 * srcbuff, uint16 * destbuff);

#endif /* SFUSAT_SFU_RTC_H_ */
