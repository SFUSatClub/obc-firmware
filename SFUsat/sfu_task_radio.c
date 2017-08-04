/*
 * sfu_task_radio.c
 *
 *  Created on: Apr 30, 2017
 *      Author: steven
 */

#include "sfu_hardwaredefs.h"
#include "sfu_task_radio.h"
#include "sfu_uart.h"

#define READ_BIT 0x80
#define BURST_BIT 0x40
#define SRES 0x30
#define SFRX 0x3A
#define SFTX 0x3B
#define SNOP 0x3D

spiDAT1_t spiDataConfig;

QueueHandle_t xRadioTXQueue;
QueueHandle_t xRadioRXQueue;

void vRadioTask(void *pvParameters) {
	xRadioTXQueue = xQueueCreate(10, sizeof(portCHAR *));
	xRadioRXQueue = xQueueCreate(10, sizeof(portCHAR));

	initRadio();
	while (1) {
		serialSendln("radio task");
		vTaskDelay(pdMS_TO_TICKS(5000));
		initRadio();
	}
}

void readRegister(uint8 addr) {
	uint16 src[] = {addr | READ_BIT, 0x00};
	uint16 dest[] = {0x00, 0x00};
	spiTransmitAndReceiveData(TASK_RADIO_REG, &spiDataConfig, 2, src, dest);
	char buffer[30];
	snprintf(buffer, 30, "R 0x%02x\r\n < 0x%02x 0x%02x", src[0], dest[0], dest[1]);
	serialSendln(buffer);
}

void readExtendedRegister(uint16 addr) {
	uint16 src[] = {0x2F | READ_BIT, addr, 0x00};
	uint16 dest[] = {0x00, 0x00, 0x00};
	spiTransmitAndReceiveData(TASK_RADIO_REG, &spiDataConfig, 3, src, dest);
	char buffer[30];
	snprintf(buffer, 30, "RE 0x%02x\r\n < 0x%02x 0x%02x 0x%02x", src[1], dest[0], dest[1], dest[2]);
	serialSendln(buffer);
}

void strobe(uint8 addr) {
	uint16 src[] = {addr};
	uint16 dest[] = {0x00};
	spiTransmitAndReceiveData(TASK_RADIO_REG, &spiDataConfig, 1, src, dest);
	char buffer[30];
	snprintf(buffer, 30, "S 0x%02x\r\n < 0x%02x", src[0], dest[0]);
	serialSendln(buffer);
}

void writeRegister(uint8 addr, uint8 val) {
	uint16 src[] = {addr, val};
	uint16 dest[] = {0x00, 0x00};
	spiTransmitAndReceiveData(TASK_RADIO_REG, &spiDataConfig, 2, src, dest);
	char buffer[30];
	snprintf(buffer, 30, "W 0x%02x 0x%02x\r\n < 0x%02x 0x%02x", src[0], src[1], dest[0], dest[1]);
	serialSendln(buffer);
	//vTaskDelay(pdMS_TO_TICKS(1));
}

void writeExtendedRegister(uint16 addr, uint8 val) {
	uint16 src[] = {0x2F, addr, val};
	uint16 dest[] = {0x00, 0x00, 0x00};
	spiTransmitAndReceiveData(TASK_RADIO_REG, &spiDataConfig, 3, src, dest);
	char buffer[30];
	snprintf(buffer, 30, "WE 0x%02x 0x%02x\r\n < 0x%02x 0x%02x 0x%02x", src[1], src[2], dest[0], dest[1], dest[2]);
	serialSendln(buffer);
	//vTaskDelay(pdMS_TO_TICKS(1));
}

void readAllRegisters() {
    readRegister(SMARTRF_SETTING_IOCFG3_ADDR);
    readRegister(SMARTRF_SETTING_IOCFG2_ADDR);
    readRegister(SMARTRF_SETTING_IOCFG1_ADDR);
    readRegister(SMARTRF_SETTING_IOCFG0_ADDR);
    readRegister(SMARTRF_SETTING_SYNC_CFG1_ADDR);
    readRegister(SMARTRF_SETTING_DEVIATION_M_ADDR);
    readRegister(SMARTRF_SETTING_MODCFG_DEV_E_ADDR);
    readRegister(SMARTRF_SETTING_DCFILT_CFG_ADDR);
    readRegister(SMARTRF_SETTING_FREQ_IF_CFG_ADDR);
    readRegister(SMARTRF_SETTING_IQIC_ADDR);
    readRegister(SMARTRF_SETTING_CHAN_BW_ADDR);
    readRegister(SMARTRF_SETTING_MDMCFG0_ADDR);
    readRegister(SMARTRF_SETTING_SYMBOL_RATE2_ADDR);
    readRegister(SMARTRF_SETTING_SYMBOL_RATE1_ADDR);
    readRegister(SMARTRF_SETTING_SYMBOL_RATE0_ADDR);
    readRegister(SMARTRF_SETTING_AGC_REF_ADDR);
    readRegister(SMARTRF_SETTING_AGC_CS_THR_ADDR);
    readRegister(SMARTRF_SETTING_AGC_CFG1_ADDR);
    readRegister(SMARTRF_SETTING_AGC_CFG0_ADDR);
    readRegister(SMARTRF_SETTING_FIFO_CFG_ADDR);
    readRegister(SMARTRF_SETTING_FS_CFG_ADDR);
    readRegister(SMARTRF_SETTING_PKT_CFG0_ADDR);
    readRegister(SMARTRF_SETTING_PKT_LEN_ADDR);
    readExtendedRegister(SMARTRF_SETTING_IF_MIX_CFG_ADDR);
    readExtendedRegister(SMARTRF_SETTING_FREQOFF_CFG_ADDR);
    readExtendedRegister(SMARTRF_SETTING_FREQ2_ADDR);
    readExtendedRegister(SMARTRF_SETTING_FREQ1_ADDR);
    readExtendedRegister(SMARTRF_SETTING_FREQ0_ADDR);
    readExtendedRegister(SMARTRF_SETTING_IF_ADC0_ADDR);
    readExtendedRegister(SMARTRF_SETTING_FS_DIG1_ADDR);
    readExtendedRegister(SMARTRF_SETTING_FS_DIG0_ADDR);
    readExtendedRegister(SMARTRF_SETTING_FS_CAL0_ADDR);
    readExtendedRegister(SMARTRF_SETTING_FS_DIVTWO_ADDR);
    readExtendedRegister(SMARTRF_SETTING_FS_DSM0_ADDR);
    readExtendedRegister(SMARTRF_SETTING_FS_DVC0_ADDR);
    readExtendedRegister(SMARTRF_SETTING_FS_PFD_ADDR);
    readExtendedRegister(SMARTRF_SETTING_FS_PRE_ADDR);
    readExtendedRegister(SMARTRF_SETTING_FS_REG_DIV_CML_ADDR);
    readExtendedRegister(SMARTRF_SETTING_FS_SPARE_ADDR);
    readExtendedRegister(SMARTRF_SETTING_XOSC5_ADDR);
    readExtendedRegister(SMARTRF_SETTING_XOSC3_ADDR);
    readExtendedRegister(SMARTRF_SETTING_XOSC1_ADDR);
}

BaseType_t initRadio() {
	spiDataConfig.CS_HOLD = TRUE;
	spiDataConfig.WDEL = TRUE;
    spiDataConfig.DFSEL = SPI_FMT_0;
    /*
     * Encoded SPI Transfer Group Chip Select
     * cc1125 is active-low, on CS0
     * SPI_CS_0 -> 0xFE -> 11111110
     */
    spiDataConfig.CSNR = SPI_CS_0;
    //spiEnableLoopback(spiREG1, Digital_Lbk);
    //spiTransmitData(spiREG1, &spiDataConfig, blocksize, srcbuff);

    readExtendedRegister(0x8F);
    readExtendedRegister(0x90);
    strobe(SNOP);
    readAllRegisters();
    writeRegister(SMARTRF_SETTING_IOCFG3_ADDR, SMARTRF_SETTING_IOCFG3);
    writeRegister(SMARTRF_SETTING_IOCFG2_ADDR, SMARTRF_SETTING_IOCFG2);
    writeRegister(SMARTRF_SETTING_IOCFG1_ADDR, SMARTRF_SETTING_IOCFG1);
    writeRegister(SMARTRF_SETTING_IOCFG0_ADDR, SMARTRF_SETTING_IOCFG0);
    writeRegister(SMARTRF_SETTING_SYNC_CFG1_ADDR, SMARTRF_SETTING_SYNC_CFG1);
    writeRegister(SMARTRF_SETTING_DEVIATION_M_ADDR, SMARTRF_SETTING_DEVIATION_M);
    writeRegister(SMARTRF_SETTING_MODCFG_DEV_E_ADDR, SMARTRF_SETTING_MODCFG_DEV_E);
    writeRegister(SMARTRF_SETTING_DCFILT_CFG_ADDR, SMARTRF_SETTING_DCFILT_CFG);
    writeRegister(SMARTRF_SETTING_FREQ_IF_CFG_ADDR, SMARTRF_SETTING_FREQ_IF_CFG);
    writeRegister(SMARTRF_SETTING_IQIC_ADDR, SMARTRF_SETTING_IQIC);
    writeRegister(SMARTRF_SETTING_CHAN_BW_ADDR, SMARTRF_SETTING_CHAN_BW);
    writeRegister(SMARTRF_SETTING_MDMCFG0_ADDR, SMARTRF_SETTING_MDMCFG0);
    writeRegister(SMARTRF_SETTING_SYMBOL_RATE2_ADDR, SMARTRF_SETTING_SYMBOL_RATE2);
    writeRegister(SMARTRF_SETTING_SYMBOL_RATE1_ADDR, SMARTRF_SETTING_SYMBOL_RATE1);
    writeRegister(SMARTRF_SETTING_SYMBOL_RATE0_ADDR, SMARTRF_SETTING_SYMBOL_RATE0);
    writeRegister(SMARTRF_SETTING_AGC_REF_ADDR, SMARTRF_SETTING_AGC_REF);
    writeRegister(SMARTRF_SETTING_AGC_CS_THR_ADDR, SMARTRF_SETTING_AGC_CS_THR);
    writeRegister(SMARTRF_SETTING_AGC_CFG1_ADDR, SMARTRF_SETTING_AGC_CFG1);
    writeRegister(SMARTRF_SETTING_AGC_CFG0_ADDR, SMARTRF_SETTING_AGC_CFG0);
    writeRegister(SMARTRF_SETTING_FIFO_CFG_ADDR, SMARTRF_SETTING_FIFO_CFG);
    writeRegister(SMARTRF_SETTING_FS_CFG_ADDR, SMARTRF_SETTING_FS_CFG);
    writeRegister(SMARTRF_SETTING_PKT_CFG0_ADDR, SMARTRF_SETTING_PKT_CFG0);
    writeRegister(SMARTRF_SETTING_PKT_LEN_ADDR, SMARTRF_SETTING_PKT_LEN);
    writeExtendedRegister(SMARTRF_SETTING_IF_MIX_CFG_ADDR, SMARTRF_SETTING_IF_MIX_CFG);
    writeExtendedRegister(SMARTRF_SETTING_FREQOFF_CFG_ADDR, SMARTRF_SETTING_FREQOFF_CFG);
    writeExtendedRegister(SMARTRF_SETTING_FREQ2_ADDR, SMARTRF_SETTING_FREQ2);
    writeExtendedRegister(SMARTRF_SETTING_FREQ1_ADDR, SMARTRF_SETTING_FREQ1);
    writeExtendedRegister(SMARTRF_SETTING_FREQ0_ADDR, SMARTRF_SETTING_FREQ0);
    writeExtendedRegister(SMARTRF_SETTING_IF_ADC0_ADDR, SMARTRF_SETTING_IF_ADC0);
    writeExtendedRegister(SMARTRF_SETTING_FS_DIG1_ADDR, SMARTRF_SETTING_FS_DIG1);
    writeExtendedRegister(SMARTRF_SETTING_FS_DIG0_ADDR, SMARTRF_SETTING_FS_DIG0);
    writeExtendedRegister(SMARTRF_SETTING_FS_CAL0_ADDR, SMARTRF_SETTING_FS_CAL0);
    writeExtendedRegister(SMARTRF_SETTING_FS_DIVTWO_ADDR, SMARTRF_SETTING_FS_DIVTWO);
    writeExtendedRegister(SMARTRF_SETTING_FS_DSM0_ADDR, SMARTRF_SETTING_FS_DSM0);
    writeExtendedRegister(SMARTRF_SETTING_FS_DVC0_ADDR, SMARTRF_SETTING_FS_DVC0);
    writeExtendedRegister(SMARTRF_SETTING_FS_PFD_ADDR, SMARTRF_SETTING_FS_PFD);
    writeExtendedRegister(SMARTRF_SETTING_FS_PRE_ADDR, SMARTRF_SETTING_FS_PRE);
    writeExtendedRegister(SMARTRF_SETTING_FS_REG_DIV_CML_ADDR, SMARTRF_SETTING_FS_REG_DIV_CML);
    writeExtendedRegister(SMARTRF_SETTING_FS_SPARE_ADDR, SMARTRF_SETTING_FS_SPARE);
    writeExtendedRegister(SMARTRF_SETTING_XOSC5_ADDR, SMARTRF_SETTING_XOSC5);
    writeExtendedRegister(SMARTRF_SETTING_XOSC3_ADDR, SMARTRF_SETTING_XOSC3);
    writeExtendedRegister(SMARTRF_SETTING_XOSC1_ADDR, SMARTRF_SETTING_XOSC1);
    readAllRegisters();

	return pdPASS;
}



BaseType_t radioCmd(char * toSend) {
	if (xQueueSendToBack(xSerialTXQueue, &toSend, 0) == pdPASS) {
		return pdPASS;
	} else {
		return pdFAIL;
	}
}
