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

/******************************************************************************
 *
 * System-level config
 * 	used for configuring things without having to dig into a bunch of headers
 *
 *****************************************************************************/
#define DEPLOY_TESTMODE			/* remove before flight - reduces the deploy activate time */


/******************************************************************************
 *
 * SETTINGS FOR PLATFORM-OBC-V0.4
 *
 *****************************************************************************/
#ifdef PLATFORM_OBC_V0_4
/**
 * RTC
 */
#define RTC_CS_PORT 			spiPORT5
#define RTC_CS_PIN 				0
#define RTC_SPI_REG 			spiREG5
// RTC SPI Config (set in rtcInit())
#define RTC_CONFIG_CS_HOLD 		0 			// CS false = high during data transfer
#define RTC_CONFIG_WDEL 		1 			// wdelay
#define RTC_CONFIG_DFSEL 		SPI_FMT_0 	// data format
#define RTC_CONFIG_CSNR 		0x00 		// chip select to use

/**
 * General Functionality
 */
#define RTI_CLK_SPEED_HZ		30000000	// 30 MHz - for RTOS tick
#define DEBUG_LED_PORT			gioPORTA
#define DEBUG_LED_PIN			6
#define WATCHDOG_TICKLE_PORT	gioPORTB
#define WATCHDOG_TICKLE_PIN		0
#define DEMO_ADC_REG			adcREG1
#define DEMO_ADC_PIN			1
#define UART_PORT				sciREG

/**
 * Radio
 */
#define RF_SPI_REG 				spiREG3
#define RF_CONFIG_CS_HOLD 		TRUE				// CS false = high during data transfer
#define RF_CONFIG_WDEL 			TRUE				// wdelay
#define RF_CONFIG_DFSEL 		SPI_FMT_0			// data format
#define RF_CONFIG_CSNR 			0x00 				// chip select to use
#define RF_IRQ_PORT				gioPORTA
#define RF_IRQ_PIN				5
/**
 * Active low. So mask just the bit we want to 0 and set everything else to 1.
 */
#define RF_CONFIG_CS_LB			~( (uint8_t) 2 );	// Lower band CC1101 CS; 435 MHz
#define RF_CONFIG_CS_UB			~( (uint8_t) 16 );	// Upper band CC1101 CS; 790 MHz

/**
 * IRQs
 */

#define GIO_IRQ_PIN 			5
#define GIO_IRQ_PORT 			gioPORTA

/**
 * Flash
 */
#define FLASH_MIBSPI_REG 		mibspiREG1
#define FLASH_DATA_FORMAT 		0
#define FLASH_6_BYTE_GROUP 		0
#define FLASH_1_BYTE_GROUP 		1 // transfer group with 1 byte length
#define FLASH_2_BYTE_GROUP 		2 // transfer group with 2 byte length
#define FLASH_4_BYTE_GROUP 		4 // transfer group with 4 byte length
#define FLASH_20_BYTE_GROUP		3 // TG 20 byte length
#define FLASH_CHIP_TYPE 		1 // 0 = SST26, 1 = IS25LP016D

/*
 * Deployment
 */
#define RF_DEPLOY0_PIN			0
#define RF_DEPLOY0_PORT			gioPORTA
#define RF_DEPLOY1_PIN			1
#define RF_DEPLOY1_PORT			gioPORTA

#define RF_DEPLOY2_PIN			17			/* MIBSPI1SIMO[1], N2HET1[08] (pin 106), [xGPIO8] ---- how to figure this out: https://github.com/SFUSatClub/obc-firmware/wiki/Using-TMS570-Peripheral-Pins-as-GPIO */
#define RF_DEPLOY2_PORT			mibspiPORT1

#define	DEPLOY_SELECT_PIN		7			/* gioA7 [xGPIO7]  output */
#define DEPLOY_SELECT_PORT		gioPORTA

#define DEPLOY_EN_PIN			4			/* MIBSPI1CS4 [xGPIO6] output	RA: enabled pulldown in HCG since this is active high */
#define DEPLOY_EN_PORT			mibspiPORT1

/* ADC
 */

#define OBC_CURRENT_ADC 	2
#endif /* PLATFORM_OBC_V0_4 */


/******************************************************************************
 *
 * SETTINGS FOR PLATFORM-OBC-V0.3
 *
 *****************************************************************************/
#ifdef PLATFORM_OBC_V0_3
/**
 * RTC
 */
#define RTC_CS_PORT 			spiPORT4
#define RTC_CS_PIN 				0
#define RTC_SPI_REG 			spiREG4
// RTC SPI Config (set in rtcInit())
#define RTC_CONFIG_CS_HOLD 		0 			// CS false = high during data transfer
#define RTC_CONFIG_WDEL 		1 			// wdelay
#define RTC_CONFIG_DFSEL 		SPI_FMT_0 	// data format
#define RTC_CONFIG_CSNR 		0x00 		// chip select to use

/**
 * General Functionality
 */
#define RTI_CLK_SPEED_HZ		30000000	// 80 MHz - for RTOS tick
#define DEBUG_LED_PORT 			gioPORTA
#define DEBUG_LED_PIN 			6
#define WATCHDOG_TICKLE_PORT 	gioPORTA
#define WATCHDOG_TICKLE_PIN 	7
#define DEMO_ADC_REG 			adcREG1
#define DEMO_ADC_PIN 			2
#define UART_PORT 				sciREG

/**
 * Radio
 */
#define RF_SPI_REG 				spiREG3
#define RF_CONFIG_CS_HOLD 		TRUE		// CS false = high during data transfer
#define RF_CONFIG_WDEL 			TRUE		// wdelay
#define RF_CONFIG_DFSEL 		SPI_FMT_0 	// data format
#define RF_CONFIG_CSNR 			0x00 		// chip select to use

/**
 * IRQs
 */
#define RF_IRQ_PIN 				2
#define RF_IRQ_PORT 			gioPORTB
#define GIO_IRQ_PIN 			5
#define GIO_IRQ_PORT 			gioPORTA

/**
 * Flash
 */
#define FLASH_MIBSPI_REG 		mibspiREG1
#define FLASH_DATA_FORMAT 		0
#define FLASH_6_BYTE_GROUP 		0
#define FLASH_1_BYTE_GROUP 		1 // transfer group with 1 byte length
#define FLASH_2_BYTE_GROUP 		2 // transfer group with 2 byte length
#define FLASH_4_BYTE_GROUP 		4 // transfer group with 4 byte length
#define FLASH_20_BYTE_GROUP 	3 // TG 20 byte length
#define FLASH_CHIP_TYPE 		1 // 0 = SST26, 1 = IS25LP016D
#endif /* PLATFORM_OBC_V0_3 */


/******************************************************************************
 *
 * SETTINGS FOR LAUNCHPAD
 *
 *****************************************************************************/
#ifdef PLATFORM_LAUNCHPAD
/**
 * RTC
 */
#define RTC_CS_PORT 			gioPORTA
#define RTC_CS_PIN 				1
#define RTC_SPI_REG 			spiREG2
// RTC SPI Config (set in rtcInit())
#define RTC_CONFIG_CS_HOLD 		0 			// CS false = high during data transfer
#define RTC_CONFIG_WDEL 		1 			// wdelay
#define RTC_CONFIG_DFSEL 		SPI_FMT_0 	// data format
#define RTC_CONFIG_CSNR 		0x00 		// chip select to use

/**
 * General Functionality
 */
#define RTI_CLK_SPEED_HZ		40000000	//Note: this is not used in RTOS config for this platform.
#define DEBUG_LED_PORT 			gioPORTA
#define DEBUG_LED_PIN 			2
#define WATCHDOG_TICKLE_PORT 	gioPORTA
#define WATCHDOG_TICKLE_PIN 	7
#define DEMO_ADC_REG 			adcREG1
#define DEMO_ADC_PIN 			1
#define UART_PORT 				scilinREG

/**
 * Radio
 */
#define RF_SPI_REG 				spiREG3
#define RF_CONFIG_CS_HOLD 		TRUE		// CS false = high during data transfer
#define RF_CONFIG_WDEL 			TRUE		// wdelay
#define RF_CONFIG_DFSEL 		SPI_FMT_0 	// data format
#define RF_CONFIG_CSNR 			0x00 		// chip select to use

/**
 * IRQs
 */
#define RF_IRQ_PIN 				0
#define RF_IRQ_PORT 			gioPORTA
#define GIO_IRQ_PIN 			7
#define GIO_IRQ_PORT 			gioPORTA

/**
 * Flash
 */
#define FLASH_MIBSPI_REG 		mibspiREG1
#define FLASH_6_BYTE_GROUP 		0
#define FLASH_1_BYTE_GROUP 		1 // transfer group with 1 byte length
#define FLASH_2_BYTE_GROUP 		2 // transfer group with 2 byte length
#define FLASH_4_BYTE_GROUP 		4 // transfer group with 4 byte length
#define FLASH_20_BYTE_GROUP 	3 // TG 20 byte length
#define FLASH_CHIP_TYPE 		0 // 0 = SST26, 1 = IS25LP016D
#endif /* PLATFORM_LAUNCHPAD */

#endif /* SFUSAT_HWDEFS_H_ */

