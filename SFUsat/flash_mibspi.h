/*
 * flash_mibspi.h
 *
 *  Created on: Aug 24, 2017
 *      Author: Richard
 */

#ifndef SFUSAT_FLASH_MIBSPI_H_
#define SFUSAT_FLASH_MIBSPI_H_

#include "sfu_hardwaredefs.h"
#include "mibspi.h"
#include "FreeRTOS.h"
#include "rtos_semphr.h"

// flags for complete transfers
uint32_t TG0_IS_Complete; // in freertos, replace this with a mutex
uint32_t TG1_IS_Complete;
uint32_t TG2_IS_Complete;
uint32_t TG3_IS_Complete;

// Various buffers - these must be the same number of elements as the associated transfer group
uint16_t TG3_RX[20];
uint16 TG1_RX[2];
uint16_t dummyBytes_16[16];
uint32_t addressWritten;
uint32_t lastRead;

SemaphoreHandle_t xFlashMutex;



// Flash Specific
void flash_mibspi_init();
void flash_erase_chip();
void flash_set_burst_64();
uint16_t flash_status();
void flash_busy_erasing_chip();
void flash_read_16(uint32_t address, uint16_t *inBuffer);
void flash_read_16_rtos(uint32_t address, uint16_t *inBuffer);
void flash_write_16_rtos(uint32_t address, uint16_t *inBuffer);
uint32_t getEmptySector(); // finds the first section with 16 1's in it


// tests
boolean flash_test_JEDEC(void); // reads and confirms JEDEC ID
boolean rw16_test(uint32_t address); // reads and writes 16 bytes to the specified address

// Data construction
void construct_packet_6(uint16_t command, uint32_t address, uint16_t * packet, uint16_t databytes);
void construct_packet_16(uint16_t command, uint32_t address, uint16_t * packet);

// SPI drivers
void mibspi_send(uint8_t transfer_group, uint16_t * TX_DATA);
void mibspi_receive(uint8_t transfer_group,uint16_t * RX_DATA);
void mibspi_write_byte(uint16_t toWrite);
void mibspi_write_two(uint16_t arg1, uint16_t arg2);


// Flash Commands
#define FLASH_READ 0x0003
#define FLASH_RDID 0xAB00 // flash read product ID + 1 dummy byte
#define DUMMY2 0x0000 // since we're using 16-bit words, align the commands to the left edge of the word. Then have trailing zeros (see above). Send in 4 bytes (2 words) as {command, DUMMY2}

// instructions
#define WRITE_ENABLE 0x0006 // send this before flash_write
#define FLASH_WRITE 0x0002 // program page command - must come after write enable
#define READ_REG_STATUS 0x0005 // read status register (bit 0 = WIP)
#define NORD 0x0003 // normal read
#define RMDID 0x0090 // read device manufacturer or something
#define RDJDID 0x009F
#define ULBPR 0x0098 // global write unlock
#define CHIP_ERASE 0x00c7

// status register
#define STATUS_WIP 0x01 // WIP bit of status register
#define STATUS_WEL 0x02 // 1 = write enabled

#endif /* SFUSAT_FLASH_MIBSPI_H_ */
