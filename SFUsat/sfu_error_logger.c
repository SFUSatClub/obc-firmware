/*
 * sfu_error_logger.c
 *
 *  Created on: Mar 28, 2018
 *      Author: Colin
 */

#include "sfu_error_logger.h"
#include "sfu_rtc.h"

void vErrorLoggerInit(void *pvParameters)
{
	xErrorLoggingQueue = xQueueCreate(QUEUE_LENGTH, sizeof(struct ErrorMessage));
}

