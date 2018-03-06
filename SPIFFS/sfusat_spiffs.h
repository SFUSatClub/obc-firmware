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

static spiffs fs;
spiffs_config cfg;

SemaphoreHandle_t newMutex;
// RTOS things
//#define SFUSAT_SPIFFS_ENTER_MUTEX xSemaphoreTake( xSpiffsMutex, pdMS_TO_TICKS(60) );
//#define SFUSAT_SPIFFS_EXIT_MUTEX xSemaphoreGive( xSpiffsMutex );



#define LOG_PAGE_SIZE       256

static u8_t spiffs_work_buf[LOG_PAGE_SIZE*2];
static u8_t spiffs_fds[32*4];
static u8_t spiffs_cache_buf[(LOG_PAGE_SIZE+32)*4];

void my_spiffs_mount();

static s32_t my_spiffs_read(u32_t addr, u32_t size, u8_t *dst);
static s32_t my_spiffs_write(u32_t addr, u32_t size, u8_t *src);
static s32_t my_spiffs_erase(u32_t addr, u32_t size);

void test_spiffs();
void read_write_example();

void sfusat_spiffs_init();


#endif /* SPIFFS_SFUSAT_SPIFFS_H_ */
