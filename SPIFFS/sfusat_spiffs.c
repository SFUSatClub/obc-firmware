/*
 * sfusat_spiffs.c
 *
 *  Created on: Feb 25, 2018
 *      Author: Richard
 */
#include "sfusat_spiffs.h"
#include "spiffs_config.h"
#include "flash_mibspi.h"
#include "sfu_uart.h"
#include <assert.h>

 void my_spiffs_mount() {
//    spiffs_config cfg;
    cfg.phys_size = 2*1024*1024; // use all spi flash (bytes?)
    cfg.phys_addr = 0; // start spiffs at start of spi flash
    cfg.phys_erase_block = 4096; // according to datasheet
    cfg.log_block_size = 65536; // let us not complicate things
    cfg.log_page_size = LOG_PAGE_SIZE; // as we said

    cfg.hal_read_f = my_spiffs_read;
    cfg.hal_write_f = my_spiffs_write;
    cfg.hal_erase_f = my_spiffs_erase;

    int res = SPIFFS_mount(&fs,
      &cfg,
      spiffs_work_buf,
      spiffs_fds,
      sizeof(spiffs_fds),
      spiffs_cache_buf,
      sizeof(spiffs_cache_buf),
      0);
//    printf("mount res: %i\n", res);
  }

 static s32_t my_spiffs_read(u32_t addr, u32_t size, u8_t *dst) {
	 flash_read_arbitrary(addr, size, dst);
   return SPIFFS_OK;
 }

 static s32_t my_spiffs_write(u32_t addr, u32_t size, u8_t *src) {
	 flash_write_arbitrary(addr, size, src);
		while(flash_status() != 0){ // wait for the write to complete
		}
   return SPIFFS_OK;
 }

 static s32_t my_spiffs_erase(u32_t addr, u32_t size) {
	 /* We erase pages - 4096 bytes
	  * Logical block size = 65536 bytes
	  *
	  * Spiffs may want to erase various sizes of section - so we must ensure that it is a multiple of the page size.
	  * More evaluation needs to be done to make sure this is all safe and no data is lost. It's hard to trust the usage
	  * of these functions since the SPIFFS docs are awkwardly worded.
	  *
	  * It's also not clear whether SPIFFS handles this loop or not.
	  */

	 assert(size % cfg.phys_erase_block == 0); // make sure size is a multiple of our erase page size

	 uint32_t num_runs;
	 for(num_runs = size / cfg.phys_erase_block; num_runs > 0; num_runs--){ // erase however many times we need
	 	 flash_erase_sector(addr);
	 	 addr = addr + cfg.phys_erase_block;
	 }
	 return SPIFFS_OK;
 }

 void test_spiffs() {
	 // Spiffs must be mounted before running this

   char buf[12];
   int32_t errNo;
   errNo = 0;
   errNo = SPIFFS_errno(&fs);

   errNo = SPIFFS_buffer_bytes_for_filedescs(&fs, 10);
//   SPIFFS_buffer_bytes_for_cache(&fs); // result of this was 480 bytes

   spiffs_file fd = SPIFFS_open(&fs, "my_file", SPIFFS_CREAT | SPIFFS_TRUNC | SPIFFS_RDWR, 0);
   errNo = SPIFFS_errno(&fs);
   if (SPIFFS_write(&fs, fd, (u8_t *)"Hello world", 12) < 0){
	   errNo = SPIFFS_errno(&fs);
	  serialSendln("Error on spiffs write");
   }

   SPIFFS_close(&fs, fd);

   fd = SPIFFS_open(&fs, "my_file", SPIFFS_RDWR, 0);
   if (SPIFFS_read(&fs, fd, (u8_t *)buf, 12) < 0){
	   errNo = SPIFFS_errno(&fs);
	   serialSendln("Error on spiffs read");
   }

   SPIFFS_close(&fs, fd);

	serialSendln("we good");

	// Inline test to make sure that erase function works
//	my_spiffs_erase(0, 4096);
//	my_spiffs_erase(0, 8192);
//	my_spiffs_erase(0, 9830);
 }
