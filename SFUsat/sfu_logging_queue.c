/*
 * sfu_logging_queue.c
 *
 *  Created on: Mar 28, 2018
 *      Author: Colin
 */

#include "sfu_logging_queue.h"
#include "sfu_rtc.h"
#include "sfu_uart.h"

QueueHandle_t xLoggingQueue;

// returns pdPASS when successful otherwise errQUEUE_FULL
BaseType_t addLogItem(LogType_t logType, EncodedMessage_t encodedMessage)
{
	LoggingQueueStructure_t input;
	BaseType_t xStatus;

	input.rtcEpochTime = 0;//getCurrentRTCTime(); // gets stuck on sfu_rtc getCurrentRTCTime mutex acquire
	input.logType = logType;
	input.encodedMessage = encodedMessage;

	xStatus = xQueueSend(xLoggingQueue, &input, 500);
	if (xStatus == pdPASS) {
		serialSendQ("xQueueSend return code 1");
		return pdPASS;
	}
	else {
		char genBuf[36] = { '\0' };
		snprintf(genBuf, 36, "xQueueSend return code %d", xStatus);
		serialSendQ(genBuf); // 0: errQUEUE_FULL, 1 : pdPASS
		return errQUEUE_FULL;
	}
}
