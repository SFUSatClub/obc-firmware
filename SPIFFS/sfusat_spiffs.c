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

 void my_spiffs_mount() {
    spiffs_config cfg;
    cfg.phys_size = 2*1024*1024; // use all spi flash
    cfg.phys_addr = 0; // start spiffs at start of spi flash
    cfg.phys_erase_block = 32768; // according to datasheet
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
   return SPIFFS_OK;
 }

 static s32_t my_spiffs_erase(u32_t addr, u32_t size) {
//   my_spi_erase(addr, size);
	 flash_erase_sector(addr);
	 return SPIFFS_OK;
 }

 void test_spiffs() {
   char buf[12];
   int32_t thang;
   thang = 0;
   thang = SPIFFS_errno(&fs);
   // Surely, I've mounted spiffs before entering here

   thang = SPIFFS_buffer_bytes_for_filedescs(&fs, 10);
//   SPIFFS_buffer_bytes_for_cache(&fs);


   spiffs_file fd = SPIFFS_open(&fs, "my_file", SPIFFS_CREAT | SPIFFS_TRUNC | SPIFFS_RDWR, 0);
   thang = SPIFFS_errno(&fs);
   if (SPIFFS_write(&fs, fd, (u8_t *)"Hello world", 12) < 0){
	  thang = SPIFFS_errno(&fs);
   }
   SPIFFS_close(&fs, fd);

   fd = SPIFFS_open(&fs, "my_file", SPIFFS_RDWR, 0);
   if (SPIFFS_read(&fs, fd, (u8_t *)buf, 12) < 0){
	   thang = SPIFFS_errno(&fs);
   }
   SPIFFS_close(&fs, fd);

	serialSendln("we good");

//   printf("--> %s <--\n", buf);
 }
