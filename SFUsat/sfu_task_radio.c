/*
 * sfu_task_radio.c
 *
 *  Created on: Apr 30, 2017
 *      Author: steven
 */

#include "sfu_hardwaredefs.h"
#include "sfu_task_radio.h"
#include "sfu_uart.h"
#include "sfu_smartrf_cc1101.h"

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


    readRegister(0x2D);
    readRegister(0x2E);
    readRegister(0x30 | BURST_BIT);
    readRegister(0x31 | BURST_BIT);
    strobe(SNOP);
    //readAllRegisters();
    readRegister(SMARTRF_SETTING_IOCFG0_ADDR);
    writeRegister(SMARTRF_SETTING_IOCFG0_ADDR, SMARTRF_SETTING_IOCFG0_VAL_RX);
    readRegister(SMARTRF_SETTING_IOCFG0_ADDR);
    //readAllRegisters();

	return pdPASS;
}



BaseType_t radioCmd(char * toSend) {
	if (xQueueSendToBack(xSerialTXQueue, &toSend, 0) == pdPASS) {
		return pdPASS;
	} else {
		return pdFAIL;
	}
}
