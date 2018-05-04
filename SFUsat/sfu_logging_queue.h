/*
 * sfu_logging_queue.h
 *
 *  Created on: Mar 27, 2018
 *      Author: Colin Chan
 *
 *  See https://github.com/SFUSatClub/obc-firmware/issues/9
 */

#ifndef SFUSAT_ERROR_LOGGING_H_
#define SFUSAT_ERROR_LOGGING_H_

#include "FreeRTOS.h"
#include "rtos_queue.h"

#define LOGGING_QUEUE_LENGTH 20
extern QueueHandle_t xLoggingQueue;

// Type of log (telemetry details, error logging, general information)
typedef enum {
	logtype_1,
	logtype_2,
	logtype_3,
} LogType_t;

// The encoded message we are associating with LogType_t
// We will need to document these details in some form
typedef enum {
	detail_1,
	detail_2,
	detail_3,
} EncodedMessage_t;

// Type of structure to add to queue. Records epoch time + type/detail
typedef struct LoggingQueueStructure {
	uint32_t			rtcEpochTime;
	LogType_t			logType;
	EncodedMessage_t 	encodedMessage;
} LoggingQueueStructure_t;

// Methods
BaseType_t addlogEntry(LogType_t logType, EncodedMessage_t encodedMessage);

#endif // SFUSAT_ERROR_LOGGING_H_
