/*
 * sfu_task_logging.h
 *
 *  Created on: Mar 27, 2018
 *      Author: Colin Chan
 *
 *  See https://github.com/SFUSatClub/obc-firmware/issues/9
 */

#ifndef SFUSAT_TASK_LOGGING_H_
#define SFUSAT_TASK_LOGGING_H_

#include "FreeRTOS.h"
#include "rtos_queue.h"

#define LOGGING_QUEUE_LENGTH 20
extern QueueHandle_t xLoggingQueue;

// have task wake up whenever there is something in the queue.

// Type of log (telemetry details, error logging, general information)
typedef enum {
	logtype_i2c,
	logtype_driver,
	logtype_filesystem
} LogType_t;

// The encoded message we are associating with LogType_t
// We will need to document these details in some form
typedef enum {
	error_1,
	error_2,
	error_3,
} EncodedMessage_t;

// Type of structure to add to queue. Records epoch time + type/detail
typedef struct LoggingQueueStructure {
	uint32_t			rtcEpochTime;
	LogType_t			logType;
	EncodedMessage_t 	encodedMessage;
} LoggingQueueStructure_t;

// Methods
BaseType_t addLogItem(LogType_t logType, EncodedMessage_t encodedMessage);
void vLogToFileTask( void *pvParameters );

#endif // SFUSAT_TASK_LOGGING_H_
