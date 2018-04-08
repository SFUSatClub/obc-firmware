/*
 * sfusat_spiffs.c
 *
 *  Created on: Feb 25, 2018
 *      Author: Richard
 */
#include "spiffs.h"
#include "spiffs_config.h"
#include "sfusat_spiffs.h"
#include "flash_mibspi.h"
#include "sfu_uart.h"
#include <assert.h>
#include "FreeRTOS.h"
#include "rtos_task.h"
#include "rtos_queue.h"
#include "rtos_semphr.h"
#include "sfu_utils.h"
#include "sfu_fs_structure.h"


void spiffs_read_task(void *pvParameters) {
	spiffs_stat s;
	char buf[30] = { '\0' };
//	uint32_t readno;

	while (1) {

		if ( xSemaphoreTake( spiffsTopMutex, pdMS_TO_TICKS(SPIFFS_READ_TIMEOUT_MS) ) == pdTRUE) {
			my_spiffs_mount(); // need to mount every time because as task gets suspended, we lose the mount
			serialSendQ("Read");
			spiffs_file fd = SPIFFS_open(&fs, "new", SPIFFS_RDWR, 0);
			if (fd < 0) {
				//		    printf("errno %i\n", SPIFFS_errno(&fs));
				serialSendQ("Read, Open error");
//				return;
			}

			if (SPIFFS_fstat(&fs, fd, &s) < 0) {
//				snprintf(buf, 30, "fstat error %d", SPIFFS_errno(&fs));
//				serialSendQ(buf);
			}

//			readno = (s.size > 30)? s.size - 31 : 0;

			// read it
			if (SPIFFS_read(&fs, fd, (u8_t *) buf, 30) < 0) {
				//		    printf("errno %i\n", SPIFFS_errno(&fs));
				serialSendQ("Read, read error");
//				return;
			}
//			  printf("RD: %s", buf);
			serialSendQ(buf);
			// close it
			if (SPIFFS_close(&fs, fd) < 0) {
				//		    printf("errno %i\n", SPIFFS_errno(&fs));
//				  serialSendQ("Read, close error");
//				return;
			}
			xSemaphoreGive(spiffsTopMutex);
		} else {
			serialSendQ("Read can't get top mutex.");
		}
		vTaskDelay(pdMS_TO_TICKS(5000));

	}
}



// ---------------- LOW LEVEL ----------------------------------

void sfusat_spiffs_init() {
	spiffsHALMutex = xSemaphoreCreateMutex(); // protects HAL functions
	spiffsTopMutex = xSemaphoreCreateMutex(); // makes sure we can't interrupt a read with a write and v/v
	sfu_prefix = PREFIX_START;
	fs.user_data = &sfu_prefix;
	my_spiffs_mount();
}

void my_spiffs_mount() {
//    spiffs_config cfg;
//    cfg.phys_size = 2*1024*1024; // use all spi flash (bytes?)
//    cfg.phys_addr = 0; // start spiffs at start of spi flash
//    cfg.phys_erase_block = 4096; // according to datasheet
//    cfg.log_block_size = 65536; // let us not complicate things
//    cfg.log_page_size = LOG_PAGE_SIZE; // as we said

	cfg.hal_read_f = my_spiffs_read;
	cfg.hal_write_f = my_spiffs_write;
	cfg.hal_erase_f = my_spiffs_erase;

	int res = SPIFFS_mount(&fs, &cfg, spiffs_work_buf, spiffs_fds, sizeof(spiffs_fds), spiffs_cache_buf,
			sizeof(spiffs_cache_buf), 0);
//	printf("mount res: %i\n", res);
}

static s32_t my_spiffs_read(u32_t addr, u32_t size, u8_t *dst) {
	if ( xSemaphoreTake( spiffsHALMutex, pdMS_TO_TICKS(SPIFFS_READ_TIMEOUT_MS) ) == pdTRUE) {
		flash_read_arbitrary(addr, size, dst);
		xSemaphoreGive(spiffsHALMutex);
	} else {
		serialSendQ("Read, can't get mutex");
	}
	return SPIFFS_OK;
}

static s32_t my_spiffs_write(u32_t addr, u32_t size, u8_t *src) {
	if ( xSemaphoreTake( spiffsHALMutex, pdMS_TO_TICKS(SPIFFS_WRITE_TIMEOUT_MS) ) == pdTRUE) {
		flash_write_arbitrary(addr, size, src);
		while (flash_status() != 0) { // wait for the write to complete
		}
		xSemaphoreGive(spiffsHALMutex);
	} else {
		serialSendQ("Write can't get mutex");
	}

	return SPIFFS_OK;
}

static s32_t my_spiffs_erase(u32_t addr, u32_t size) {
	if ( xSemaphoreTake( spiffsHALMutex, pdMS_TO_TICKS(SPIFFS_ERASE_TIMEOUT_MS) ) == pdTRUE) {
		/* We erase pages - 4096 bytes
		 * Logical block size = 65536 bytes
		 *
		 * Spiffs may want to erase various sizes of section - so we must ensure that it is a multiple of the page size.
		 * More evaluation needs to be done to make sure this is all safe and no data is lost. It's hard to trust the usage
		 * of these functions since the SPIFFS docs are awkwardly worded.
		 *
		 * It's also not clear whether SPIFFS handles this loop or not.
		 */

//	 assert(size % cfg.phys_erase_block == 0); // make sure size is a multiple of our erase page size
		assert(size % SPIFFS_CFG_PHYS_ERASE_SZ(fs) == 0); // make sure size is a multiple of our erase page size

		uint32_t num_runs;
		for (num_runs = size / SPIFFS_CFG_PHYS_ERASE_SZ(fs); num_runs > 0; num_runs--) { // erase however many times we need
			flash_erase_sector(addr);
			addr = addr + SPIFFS_CFG_PHYS_ERASE_SZ(fs);
		}
		xSemaphoreGive(spiffsHALMutex);
	} else {
		serialSendQ("Erase can't get mutex");
	}
	return SPIFFS_OK;
}

void test_spiffs() {
	// Spiffs must be mounted before running this

	char buf[20];

	printf("Buffer bytes: %i \r\n", SPIFFS_buffer_bytes_for_filedescs(&fs, 10));
	printf("Cache size: %i \r\n", SPIFFS_buffer_bytes_for_cache(&fs, 8192)); // result of this was 480 bytes

	spiffs_file fd = SPIFFS_open(&fs, "my_file", SPIFFS_CREAT | SPIFFS_TRUNC | SPIFFS_RDWR, 0);
	if (SPIFFS_write(&fs, fd, (u8_t *) "Hello world", 20) < 0) {
		printf("t: Error on SPIFFS write, %i\r\n", SPIFFS_errno(&fs));
	}

	// Errors so we know if read succeeds
	buf[0] = 0;
	buf[3] = 0;
	SPIFFS_close(&fs, fd);

	fd = SPIFFS_open(&fs, "my_file", SPIFFS_RDWR, 0);
	if (SPIFFS_read(&fs, fd, (u8_t *) buf, 12) < 0) {
		printf("t: Error on SPIFFS Read, %i\r\n", SPIFFS_errno(&fs));
	}
	serialSendln(buf);

	SPIFFS_close(&fs, fd);

	printf("SPIFFS test complete.\r\n");

	// Inline test to make sure that erase function works
//	my_spiffs_erase(0, 4096);
//	my_spiffs_erase(0, 8192);
//	my_spiffs_erase(0, 9830);
}

void read_write_example() {
	char buf[12];

	// create a file, delete previous if it already exists, and open it for reading and writing
	spiffs_file fd = SPIFFS_open(&fs, "new_file", SPIFFS_CREAT | SPIFFS_TRUNC | SPIFFS_RDWR, 0);
	if (fd < 0) {
		printf("errno %i\n", SPIFFS_errno(&fs));
		return;
	}
	// write to it
	if (SPIFFS_write(&fs, fd, (u8_t *) "Hello world", 12) < 0) {
		printf("errno %i\n", SPIFFS_errno(&fs));
		return;
	}
	// close it
	if (SPIFFS_close(&fs, fd) < 0) {
		printf("errno %i\n", SPIFFS_errno(&fs));
		return;
	}

	// introduce some errors into buf so we know if read is successful
	buf[0] = 0x00;
	buf[1] = 0x00;
	buf[5] = 0x00;

	// open it
	fd = SPIFFS_open(&fs, "new_file", SPIFFS_RDWR, 0);
	if (fd < 0) {
		printf("errno %i\n", SPIFFS_errno(&fs));
		return;
	}

	// read it
	if (SPIFFS_read(&fs, fd, (u8_t *) buf, 12) < 0) {
		printf("errno %i\n", SPIFFS_errno(&fs));
		return;
	}
	printf(buf);
	// close it
	if (SPIFFS_close(&fs, fd) < 0) {
		printf("errno %i\n", SPIFFS_errno(&fs));
		return;
	}

	// check it
}
