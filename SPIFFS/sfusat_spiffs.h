/*
 * sfusat_spiffs.h
 *
 *  Created on: Feb 25, 2018
 *      Author: Richard
 *
 *      This header includes our mods to spiffs and any code required for integration.
 */

#ifndef SPIFFS_SFUSAT_SPIFFS_H_
#define SPIFFS_SFUSAT_SPIFFS_H_
#include "spiffs.h"
#include "FreeRTOS.h"
#include "rtos_semphr.h"

static spiffs fs; // Note: this was static but that prevented access from task
spiffs_config cfg;

// SFUSat Configs
#define SFU_MAX_DATA_WRITE 21 // bytes or chars. The max amount of data we can write to a file at once that is GUARANTEED not to be chopped off. The actual max depends on the time stamp.
#define SFU_WRITE_DATA_BUF (SFU_MAX_DATA_WRITE + 12) // DON'T TOUCH: to size the file write buffer

#define SPIFFS_READ_TIMEOUT_MS 2000 // number of ms to wait before giving up on a write instruction. Long since these can take quite a while
#define SPIFFS_WRITE_TIMEOUT_MS 2000
#define SPIFFS_ERASE_TIMEOUT_MS 2000
#define SPIFFS_TOP_TIMEOUT_MS 3000 // how long we'll wait for basically any other operation to finish

// RTOS Stuff
SemaphoreHandle_t spiffsHALMutex; // protects the low level HAL functions in SPIFFS
SemaphoreHandle_t spiffsTopMutex; // ensures we won't interrupt a read with a write and v/v

// Tasks
void spiffs_write_check_test(void *pvParameters);
void spiffs_check_task(void *pvParameters);
void spiffs_read_task(void *pvParameters);

// SFUSat Functions
void sfu_file_write(char* file_name, char *fmt, ...); // printf style writes to files
void test_spiffs();
void read_write_example();
void sfusat_spiffs_init();


// SPIFFS Config stuff
#define LOG_PAGE_SIZE       256
static u8_t spiffs_work_buf[LOG_PAGE_SIZE*2];
static u8_t spiffs_fds[32*4];
static u8_t spiffs_cache_buf[(LOG_PAGE_SIZE+32)*4];

void my_spiffs_mount();

static s32_t my_spiffs_read(u32_t addr, u32_t size, u8_t *dst);
static s32_t my_spiffs_write(u32_t addr, u32_t size, u8_t *src);
static s32_t my_spiffs_erase(u32_t addr, u32_t size);



#endif /* SPIFFS_SFUSAT_SPIFFS_H_ */
