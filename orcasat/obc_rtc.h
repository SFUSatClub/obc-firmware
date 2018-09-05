/*
 * sfu_rtc.h
 *
 *  Created on: Jun 12, 2017
 *      Author: Steven + Richard + Victor
 */

#ifndef SFUSAT_OBC_RTC_H_
#define SFUSAT_OBC_RTC_H_
#include "FreeRTOS.h"
#include "rtos_semphr.h"
/**
 * Get "approximated" RTC epoch time.
 * Combines last actual time from RTC with time from Hercule's HET.
 * @return seconds
 */
unsigned int getCurrentTime();

extern SemaphoreHandle_t xRTCMutex;
extern uint32_t epochOffset; // can be used to offset the epoch from the ground. Default 0. (seconds)

uint32_t no_rtos_test_getCurrentRTCTime();

/**
 * Get "actual" RTC epoch time.
 * Updates internal rtc_epoch_time with new ground truth received from the RTC.
 * Initiates several SPI transactions.
 * @return seconds
 */
uint32_t getCurrentRTCTime();

void tempAddSecondToHET();

void rtcInit();
uint8_t rtc_get_seconds();
uint8_t rtc_get_minutes();
uint8_t rtc_get_hours();
uint8_t rtc_get_day();
uint8_t rtc_get_month();
uint8_t rtc_get_year();
uint32_t getEpochOffset();
uint32_t setEpochOffset(uint32_t inputOffset);
uint8_t rtc_get_startup_status();

void rtcReset(); // triggers the reset register (does not affect time)
void rtcResetTime(); // resets time to jan 1, year 2000

uint8_t convertBCD(uint8_t input);
void rtcWriteRegister(uint16_t registerToWrite, uint16_t valueToWrite);
uint8_t rtcReadRegister(uint16_t registerToRead);
uint8_t rtcGetBit(uint16_t registerToRead, uint8_t bitToCheck);
uint8_t rtcSetBit(uint16_t registerToWrite, uint8_t bitToSet, uint8_t bitValue);
void rtcTransmitAndReceive(uint32 blocksize, uint16 * srcbuff, uint16 * destbuff);

#endif /* SFUSAT_OBC_RTC_H_ */
