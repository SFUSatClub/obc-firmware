/*
 * sfu_logging_queue.c
 *
 *  Created on: Mar 28, 2018
 *      Author: Colin
 */

#include "sfu_logging_queue.h"
#include "sfu_rtc.h"
QueueHandle_t xLoggingQueue;

// returns pdPASS when successful otherwise errQUEUE_FULL
BaseType_t addlogEntry(LogType_t logType, EncodedMessage_t encodedMessage)
{
	LoggingQueueStructure_t input;

	input.rtcEpochTime = 54;///getCurrentRTCTime();
	input.logType = logType;
	input.encodedMessage = encodedMessage;

	if (xQueueSend(xLoggingQueue, &input, 500) == pdPASS)
		return pdPASS;
	else
		return errQUEUE_FULL;
}

// anytime there is a object in the queue, pull it out, and log it into the file.
// refer to git wiki for how to write stuff onto a file.
