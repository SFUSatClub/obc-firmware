/*
 * SFU_SPI.c
 *
 *  Created on: Feb 17, 2017
 *      Author: steven
 */

#include "obc_spi.h"
#include "obc_uart.h"

static spiDAT1_t spiDataConfig;

void spi_init() {
    spiInit();
    // enable loopback for testing
    //spiEnableLoopback(spiREG3, Digital_Lbk);
//    serialSendQ("SFU spi init!");
    spiDataConfig.CS_HOLD = FALSE;
    spiDataConfig.WDEL = TRUE;
    spiDataConfig.DFSEL = SPI_FMT_0;
    spiDataConfig.CSNR = 0xFE;
}

void spi_transmit(uint32 blocksize, uint16 *srcbuff) {
    spiTransmitData(spiREG3, &spiDataConfig, blocksize, srcbuff);
}

void spi_transmit_text(const char* txt) {
    const unsigned int numBytes = strlen(txt);
    const unsigned int remainder = numBytes % 2;
    const unsigned int numUint16s = (numBytes / 2) + remainder;
    uint16 *srcbuff = (uint16 *)malloc(sizeof(uint16) * numUint16s);
    strncpy((char*)srcbuff, txt, numBytes);
    spiTransmitData(spiREG3, &spiDataConfig, numUint16s, srcbuff);
    serialSendQ("SPI_Transmit: ");
    serialSendQ((const char*)srcbuff);
    free(srcbuff);
}
