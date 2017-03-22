/*
 * SFU_SPI.h
 *
 *  Created on: Feb 17, 2017
 *      Author: steven
 */

#ifndef SFUSAT_SFU_SPI_H_
#define SFUSAT_SFU_SPI_H_

#include <string.h>
#include <stdlib.h>

#include "spi.h"

extern spiDAT1_t spiDataConfig;

void spi_init();
void spi_transfer();



#endif /* SFUSAT_SFU_SPI_H_ */
