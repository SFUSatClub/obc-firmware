/*
 * sfu_error_logger.h
 *
 *  Created on: Mar 27, 2018
 *      Author: Colin
 *
 *  See https://github.com/SFUSatClub/obc-firmware/issues/9
 */

#ifndef SFUSAT_ERROR_LOGGING_H_
#define SFUSAT_ERROR_LOGGING_H_

#include "FreeRTOS.h"
#include "rtos_queue.h"
#include "sys_common.h"

#define QUEUE_LENGTH 20
#define MESSAGE_LENGTH 128

void vErrorLoggerInit(void *pvParameters);

extern QueueHandle_t xErrorLoggingQueue;

typedef enum {
	error_code1,
	error_code2,
	error_code3,
	error_code4,
} Error_t;

typedef enum {
	error,
	telemetry,
	general_information
} Message_t;

typedef struct ErrorMessage {
	uint32_t	rtc_epoch_time;
	char 		message[MESSAGE_LENGTH];
	Error_t 	error_code;
	Message_t	message_type;
} ErrorMessage_t;

#endif // SFUSAT_ERROR_LOGGING_H_
