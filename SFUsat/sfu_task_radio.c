/*
 * sfu_task_radio.c
 *
 *  Created on: Apr 30, 2017
 *      Author: steven
 */

#include "sfu_task_radio.h"
#include "sfu_uart.h"

spiDAT1_t spiDataConfig;

QueueHandle_t xRadioTXQueue;
QueueHandle_t xRadioRXQueue;

void vRadioTask(void *pvParameters) {
	xRadioTXQueue = xQueueCreate(10, sizeof(portCHAR *));
	xRadioRXQueue = xQueueCreate(10, sizeof(portCHAR));

	radioInit();
	while (1) {
		serialSendQ("radio task");
		vTaskDelay(pdMS_TO_TICKS(2000));
	}
}

void writeConfigRegister(uint8 addr, uint8 val) {
	uint16 data[] = {addr, val};
	spiTransmitData(spiREG1, &spiDataConfig, 2, data);
}

void writeExtendedRegister(uint16 addr, uint8 val) {
	uint16 data[] = {0x2F, addr, val};
	spiTransmitData(spiREG1, &spiDataConfig, 3, data);
}

BaseType_t radioInit() {
	spiDataConfig.CS_HOLD = TRUE;
	spiDataConfig.WDEL = TRUE;
    spiDataConfig.DFSEL = SPI_FMT_0;
    spiDataConfig.CSNR = 0xFE;
    spiEnableLoopback(spiREG1, Digital_Lbk);
    //spiTransmitData(spiREG1, &spiDataConfig, blocksize, srcbuff);

    writeConfigRegister(SMARTRF_SETTING_IOCFG3_ADDR, SMARTRF_SETTING_IOCFG3);
    writeConfigRegister(SMARTRF_SETTING_IOCFG3_ADDR, SMARTRF_SETTING_IOCFG3);
    writeConfigRegister(SMARTRF_SETTING_IOCFG2_ADDR, SMARTRF_SETTING_IOCFG2);
    writeConfigRegister(SMARTRF_SETTING_IOCFG1_ADDR, SMARTRF_SETTING_IOCFG1);
    writeConfigRegister(SMARTRF_SETTING_IOCFG0_ADDR, SMARTRF_SETTING_IOCFG0);
    writeConfigRegister(SMARTRF_SETTING_SYNC_CFG1_ADDR, SMARTRF_SETTING_SYNC_CFG1);
    writeConfigRegister(SMARTRF_SETTING_DEVIATION_M_ADDR, SMARTRF_SETTING_DEVIATION_M);
    writeConfigRegister(SMARTRF_SETTING_MODCFG_DEV_E_ADDR, SMARTRF_SETTING_MODCFG_DEV_E);
    writeConfigRegister(SMARTRF_SETTING_DCFILT_CFG_ADDR, SMARTRF_SETTING_DCFILT_CFG);
    writeConfigRegister(SMARTRF_SETTING_FREQ_IF_CFG_ADDR, SMARTRF_SETTING_FREQ_IF_CFG);
    writeConfigRegister(SMARTRF_SETTING_IQIC_ADDR, SMARTRF_SETTING_IQIC);
    writeConfigRegister(SMARTRF_SETTING_CHAN_BW_ADDR, SMARTRF_SETTING_CHAN_BW);
    writeConfigRegister(SMARTRF_SETTING_MDMCFG0_ADDR, SMARTRF_SETTING_MDMCFG0);
    writeConfigRegister(SMARTRF_SETTING_SYMBOL_RATE2_ADDR, SMARTRF_SETTING_SYMBOL_RATE2);
    writeConfigRegister(SMARTRF_SETTING_SYMBOL_RATE1_ADDR, SMARTRF_SETTING_SYMBOL_RATE1);
    writeConfigRegister(SMARTRF_SETTING_SYMBOL_RATE0_ADDR, SMARTRF_SETTING_SYMBOL_RATE0);
    writeConfigRegister(SMARTRF_SETTING_AGC_REF_ADDR, SMARTRF_SETTING_AGC_REF);
    writeConfigRegister(SMARTRF_SETTING_AGC_CS_THR_ADDR, SMARTRF_SETTING_AGC_CS_THR);
    writeConfigRegister(SMARTRF_SETTING_AGC_CFG1_ADDR, SMARTRF_SETTING_AGC_CFG1);
    writeConfigRegister(SMARTRF_SETTING_AGC_CFG0_ADDR, SMARTRF_SETTING_AGC_CFG0);
    writeConfigRegister(SMARTRF_SETTING_FIFO_CFG_ADDR, SMARTRF_SETTING_FIFO_CFG);
    writeConfigRegister(SMARTRF_SETTING_FS_CFG_ADDR, SMARTRF_SETTING_FS_CFG);
    writeConfigRegister(SMARTRF_SETTING_PKT_CFG0_ADDR, SMARTRF_SETTING_PKT_CFG0);
    writeConfigRegister(SMARTRF_SETTING_PKT_LEN_ADDR, SMARTRF_SETTING_PKT_LEN);
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


	return pdPASS;
}



BaseType_t radioCmd(char * toSend) {
	if (xQueueSendToBack(xSerialTXQueue, &toSend, 0) == pdPASS) {
		return pdPASS;
	} else {
		return pdFAIL;
	}
}
