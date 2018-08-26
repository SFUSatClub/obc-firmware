/*
 * sfu_task_logging.c
 *
 *  Created on: Mar 28, 2018
 *      Author: Colin
 */

#include <obc_fs_structure.h>
#include <obc_rtc.h>
#include <obc_task_logging.h>
#include <obc_uart.h>

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

	return xStatus;
}

void vLogToFileTask(void *pvParameters) {
	// Declare variables that will be used in this task
	serialSendQ("Initialized Log File Task");
	struct LoggingQueueStructure received;

	// When an item is present in queue, log to file. Otherwise, block
	for (;;){
		if (xQueueReceive(xLoggingQueue, &received, portMAX_DELAY) == pdPASS)
			sfu_write_fname(FSYS_SYS, "%d, %d, %d",
					received.rtcEpochTime,
					received.logType,
					received.encodedMessage);
	}


}
