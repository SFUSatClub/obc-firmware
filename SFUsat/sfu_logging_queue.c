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

// Adds an item to xLoggingQueue. Can only be called after xLoggingQueue
// is defined in sfu_task_main.c
// Returns pdPASS (1) when successful otherwise errQUEUE_FULL (0)
BaseType_t addLogItem(LogType_t logType, EncodedMessage_t encodedMessage)
{
	LoggingQueueStructure_t input;
	BaseType_t xStatus;

	input.rtcEpochTime = getCurrentRTCTime();
	input.logType = logType;
	input.encodedMessage = encodedMessage;

	xStatus = xQueueSend(xLoggingQueue, &input, 500);

	// debugging purposes
	if (xStatus == pdPASS)
		serialSendQ("Added item to queue");
	else
		serialSendQ("Queue is full");

	return xStatus;
}
