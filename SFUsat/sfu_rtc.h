/*
 * sfu_rtc.h
 *
 *  Created on: Jun 12, 2017
 *      Author: steven + richard
 */

#ifndef SFUSAT_SFU_RTC_H_
#define SFUSAT_SFU_RTC_H_

/**
 * Get "approximated" RTC epoch time.
 * Combines last actual time from RTC with time from Hercule's HET.
 * @return seconds
 */
unsigned int getCurrentTime();

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

#endif /* SFUSAT_SFU_RTC_H_ */
