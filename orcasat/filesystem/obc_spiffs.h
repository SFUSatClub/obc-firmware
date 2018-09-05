/*
 * sfusat_spiffs.h
 *
 *  Created on: Feb 25, 2018
 *      Author: Richard
 *
 *      This header includes our mods to spiffs and code required for integration, such as HAL functions.
 */

#ifndef SPIFFS_SFUSAT_SPIFFS_H_
#define SPIFFS_SFUSAT_SPIFFS_H_
#include "spiffs.h"
#include "FreeRTOS.h"
#include "rtos_semphr.h"
#include "obc_fs_structure.h"

extern spiffs fs;
extern spiffs_config cfg;
extern SemaphoreHandle_t spiffsTopMutex; // ensures we won't interrupt a read with a write and v/v

#define SPIFFS_READ_TIMEOUT_MS 5000 // number of ms to wait before giving up on a write instruction. Long since these can take quite a while
#define SPIFFS_WRITE_TIMEOUT_MS 2000
#define SPIFFS_ERASE_TIMEOUT_MS 2000
#define SPIFFS_TOP_TIMEOUT_MS 3000 // how long we'll wait for basically any other operation to finish

// Tasks
void spiffs_read_task(void *pvParameters); // WIP

// SFUSat Functions
void test_spiffs();
void read_write_example();
void sfusat_spiffs_init();
// test sequences with RTOS are in test_sequences/test_spiffs_rtos.c

// SPIFFS Config stuff
#define LOG_PAGE_SIZE       256
static u8_t spiffs_work_buf[LOG_PAGE_SIZE*2];
static u8_t spiffs_fds[32*4];
static u8_t spiffs_cache_buf[(LOG_PAGE_SIZE+32)*4];

// SPIFFS HAL
void my_spiffs_mount();
static s32_t my_spiffs_read(u32_t addr, u32_t size, u8_t *dst);
static s32_t my_spiffs_write(u32_t addr, u32_t size, u8_t *src);
static s32_t my_spiffs_erase(u32_t addr, u32_t size);

#endif /* SPIFFS_SFUSAT_SPIFFS_H_ */
