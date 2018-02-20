#ifndef SFUSAT_HWDEFS_H_
#define SFUSAT_HWDEFS_H_
/* HARDWARE DEFINITIONS
 *
 * To facilitate easy switching of peripherals and pins, all hardware connections will be defined as macros here.
 * That way, we just update the actual GPIO pin here, and the macro assigned to say, the RTC CS pin (GPIO) will be updated to the correct one.
 *
 * Author: Richard Arthurs
 *
 * Note: This is only a centralized location to change hardware mapping. It must be enabled in HALCoGEN as well.
 */

#include "gio.h"
#include "spi.h"
#include "mibspi.h"

// ======================= SETTINGS FOR v0.4 =======================
#if SFUSAT_BOARD == 2
//  ---------------  RTC Defines --------------------
// Todo: confirm this works. Convert to MIBSPI like for flash section
//#define RTC_CS_PORT mibspiREG5
#define RTC_CS_PIN 0
//#define RTC_SPI_REG spiREG5
// RTC SPI Config (set in rtcInit())
#define RTC_CONFIG_CS_HOLD 0 //CS false = high during data transfer
#define RTC_CONFIG_WDEL 1 // wdelay
#define RTC_CONFIG_DFSEL SPI_FMT_0 // data format
#define RTC_CONFIG_CSNR 0x00 // chip select to use

// ---------------- General Functionality ------------------
#define DEBUG_LED_PORT gioPORTA
#define DEBUG_LED_PIN 6

#define WATCHDOG_TICKLE_PORT gioPORTA
#define WATCHDOG_TICKLE_PIN 7

#define DEMO_ADC_REG adcREG1
#define DEMO_ADC_PIN 1

#define UART_PORT sciREG

// ----------------- Radio -------------------------
#define TASK_RADIO_REG spiREG3

// ---------------- RF IRQ ------------------
#define RF_IRQ_PIN FILL_ME_IN!
#define RF_IRQ_PORT FILL_ME_IN!

//----------------- FLASH --------------------
#define FLASH_MIBSPI_REG mibspiREG1
#define FLASH_DATA_FORMAT 0
#define FLASH0_TRANSFER_GROUP 0
#define FLASH_SINGLE_TRANSFER 1 // transfer group with 1 byte length
#define FLASH_DOUBLE_TRANSFER 2 // transfer group with 2 byte length
#define FLASH_TWENTY 3 // TG 20 byte length
#define FLASH_CHIP_TYPE 1 // 0 = SST26, 1 = IS25LP016D

#endif


// =======================  SETTINGS FOR v0.3 =======================
#if SFUSAT_BOARD == 1
//  ---------------  RTC Defines --------------------
#define RTC_CS_PORT spiPORT4
#define RTC_CS_PIN 0
#define RTC_SPI_REG spiREG4
// RTC SPI Config (set in rtcInit())
#define RTC_CONFIG_CS_HOLD 0 //CS false = high during data transfer
#define RTC_CONFIG_WDEL 1 // wdelay
#define RTC_CONFIG_DFSEL SPI_FMT_0 // data format
#define RTC_CONFIG_CSNR 0x00 // chip select to use

// ---------------- General Functionality ------------------
#define DEBUG_LED_PORT gioPORTA
#define DEBUG_LED_PIN 6

#define WATCHDOG_TICKLE_PORT gioPORTA
#define WATCHDOG_TICKLE_PIN 7

#define DEMO_ADC_REG adcREG1
#define DEMO_ADC_PIN 2

#define UART_PORT sciREG

// ----------------- Radio -------------------------
#define TASK_RADIO_REG spiREG3

// ---------------- RF IRQ ------------------
#define RF_IRQ_PIN 2
#define RF_IRQ_PORT gioPORTB

//----------------- FLASH --------------------
#define FLASH_MIBSPI_REG mibspiREG1
#define FLASH_DATA_FORMAT 0
#define FLASH0_TRANSFER_GROUP 0
#define FLASH_SINGLE_TRANSFER 1 // transfer group with 1 byte length
#define FLASH_DOUBLE_TRANSFER 2 // transfer group with 2 byte length
#define FLASH_TWENTY 3 // TG 20 byte length
#define FLASH_CHIP_TYPE 1 // 0 = SST26, 1 = IS25LP016D
#endif

// ======================= SETTINGS FOR LAUNCHPAD =======================
#if SFUSAT_BOARD == 0
//  ---------------  RTC Defines --------------------
#define RTC_CS_PORT gioPORTA
#define RTC_CS_PIN 1
#define RTC_SPI_REG spiREG2
// RTC SPI Config (set in rtcInit())
#define RTC_CONFIG_CS_HOLD 0 //CS false = high during data transfer
#define RTC_CONFIG_WDEL 1 // wdelay
#define RTC_CONFIG_DFSEL SPI_FMT_0 // data format
#define RTC_CONFIG_CSNR 0x00 // chip select to use

// ---------------- General Functionality ------------------
#define DEBUG_LED_PORT gioPORTA
#define DEBUG_LED_PIN 2

#define WATCHDOG_TICKLE_PORT gioPORTA
#define WATCHDOG_TICKLE_PIN 7

#define DEMO_ADC_REG adcREG1
#define DEMO_ADC_PIN 1

#define UART_PORT scilinREG

// ----------------- Radio -------------------------
#define TASK_RADIO_REG spiREG3

// ---------------- RF IRQ ------------------
#define RF_IRQ_PIN define_me_here
#define RF_IRQ_PORT define_me_here

//----------------- FLASH --------------------
#define FLASH_MIBSPI_REG mibspiREG1
#define FLASH_DATA_FORMAT 0
#define FLASH0_TRANSFER_GROUP 0
#define FLASH_SINGLE_TRANSFER 1 // transfer group with 1 byte length
#define FLASH_DOUBLE_TRANSFER 2 // transfer group with 2 byte length
#define FLASH_TWENTY 3 // TG 20 byte length
#define FLASH_CHIP_TYPE 0 // 0 = SST26, 1 = IS25LP016D

#endif



#endif

