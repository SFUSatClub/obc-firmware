/*
 * sfu_logging_queue.c
 *
 *  Created on: Mar 28, 2018
 *      Author: Colin
 */

#include "sfu_logging_queue.h"
#include "sfu_rtc.h"
#include "sfu_uart.h"
#include "sfu_fs_structure.h"

QueueHandle_t xLoggingQueue;

// returns pdPASS (1) when successful otherwise errQUEUE_FULL (0)
BaseType_t addLogItem(LogType_t logType, EncodedMessage_t encodedMessage)
{
	LoggingQueueStructure_t input;

	input.rtcEpochTime = getCurrentRTCTime();
	input.logType = logType;
	input.encodedMessage = encodedMessage;

	if (xQueueSend(xLoggingQueue, &input, 500) == pdPASS)
	{
		serialSendQ("Added item to queue");
		return pdPASS;
	}
	else
	{
		xQueueReceive(xLoggingQueue, &input, 500);
		char buf[30] = { '\0' };
		snprintf(buf, 30, "%i", input.rtcEpochTime);
		serialSendQ(buf);
	}
}
