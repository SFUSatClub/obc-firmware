#ifndef SFUSAT_HWDEFS_H_
#define SFUSAT_HWDEFS_H_
/* HARDWARE DEFINITIONS
 *
 * To facilitate easy switching of peripherals and pins, all hardware connections will be defined as macros here.
 * That way, we just update the actual GPIO pin here, and the macro assigned to say, the RTC CS pin (GPIO) will be updated to the correct one.
 *
 * Author: Richard Arthurs
 */

#include "gio.h"
#include "spi.h"

//  ---------------  RTC Defines --------------------
#define RTC_CS_PORT gioPORTA
#define RTC_CS_PIN 1
#define RTC_SPI_REG spiREG2
// RTC SPI Config (set in rtcInit())
#define RTC_CONFIG_CS_HOLD 0 //CS false = high during data transfer
#define RTC_CONFIG_WDEL 1 // wdelay
#define RTC_CONFIG_DFSEL SPI_FMT_0 // data format
#define RTC_CONFIG_CSNR 0x00 // chip select to use
#endif
