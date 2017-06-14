/*
 * sfu_rtc.h
 *
 *  Created on: Jun 12, 2017
 *      Author: steven
 */

#ifndef SFUSAT_SFU_RTC_H_
#define SFUSAT_SFU_RTC_H_

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

#endif /* SFUSAT_SFU_RTC_H_ */
