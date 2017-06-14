/*
 * sfu_rtc.c
 *
 *  Created on: Jun 12, 2017
 *      Author: steven
 */

#include "sfu_rtc.h"

unsigned int rtc_time = 0;
unsigned int het_time = 0;

unsigned int getTime() {
	/*
	 * Get current HET time.
	 */
	//het_time = 0;

	/*
	 * Return "approximation" to actual RTC time.
	 */
	return rtc_time + het_time;
}

unsigned int getRTCTime() {
	/*
	 * Reset HET time.
	 */
	//het_time = 0;

	/*
	 * Initiate RTC SPI transfer.
	 */
	rtc_time = 0;

	return rtc_time;
}

void tempAddSecondToHET() {
	het_time++;
}
