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

/**
 * Masks
 */
#define READ_BIT	(0x80)
#define BURST_BIT	(0x40)

/**
 * Command Strobe Registers (section 29, page 67)
 */
#define SRES	(0x30) // Reset chip.
#define SFSTXON	(0x31) // Enable and calibrate frequency synthesizer (if MCSM0.FS_AUTOCAL=1). If in RX (with CCA): Go to a wait state where only the synthesizer is running (for quick RX / TX turnaround).
#define SXOFF	(0x32) // Turn off crystal oscillator.
#define SCAL	(0x33) // Calibrate frequency synthesizer and turn it off. SCAL can be strobed from IDLE mode without setting manual calibration mode (MCSM0.FS_AUTOCAL=0)
#define SRX		(0x34) // Enable RX. Perform calibration first if coming from IDLE and MCSM0.FS_AUTOCAL=1.
#define STX		(0x35) // In IDLE state: Enable TX. Perform calibration first if MCSM0.FS_AUTOCAL=1.  If in RX state and CCA is enabled: Only go to TX if channel is clear.
#define SIDLE	(0x36) // Exit RX / TX, turn off frequency synthesizer and exit Wake-On-Radio mode if applicable.
#define SWOR	(0x38) // Start automatic RX polling sequence (Wake-on-Radio) as described in Section 19.5 if WORCTRL.RC_PD=0.
#define SPWD	(0x39) // Enter power down mode when CSn goes high.
#define SFRX	(0x3A) // Flush the RX FIFO buffer. Only issue SFRX in IDLE or RXFIFO_OVERFLOW states.
#define SFTX	(0x3B) // Flush the TX FIFO buffer. Only issue SFTX in IDLE or TXFIFO_UNDERFLOW states.
#define SWORRST	(0x3C) // Reset real time clock to Event1 value.
#define SNOP	(0x3D) // No operation. May be used to get access to the chip status byte.

/**
 * Status Registers (section 29.3, page 92)
 */
#define PARTNUM			(0x30 | BURST_BIT) // Chip part number.
#define VERSION			(0x31 | BURST_BIT) // Chip version number.
#define FREQEST			(0x32 | BURST_BIT) // Frequency offset estimate from demodulator.
#define LQI				(0x33 | BURST_BIT) // Demodulator estimate for link quality.
#define RSSI			(0x34 | BURST_BIT) // Received signal strength indication.
#define MARCSTATE		(0x35 | BURST_BIT) // Main radio control state machine state.
#define WORTIME1		(0x36 | BURST_BIT) // High byte of WOR time.
#define WORTIME0		(0x37 | BURST_BIT) // Low byte of WOR time.
#define PKTSTATUS		(0x38 | BURST_BIT) // Current GDOx status and packet status.
#define VCO_VC_DAC		(0x39 | BURST_BIT) // Current setting from PLL calibration module.
#define TXBYTES			(0x3A | BURST_BIT) // Underflow and number of bytes.
#define RXBYTES			(0x3B | BURST_BIT) // Overflow and number of bytes.
#define RCCTRL1_STATUS	(0x3C | BURST_BIT) // High byte of last RC oscillator calibration result.
#define RCCTRL0_STATUS	(0x3D | BURST_BIT) // Low byte of last RC oscillator calibration result.

static spiDAT1_t spiDataConfig;

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

static void readRegister(uint8 addr) {
	uint16 src[] = {addr | READ_BIT, 0x00};
	uint16 dest[] = {0x00, 0x00};
	spiTransmitAndReceiveData(TASK_RADIO_REG, &spiDataConfig, 2, src, dest);
	char buffer[30];
	snprintf(buffer, 30, "R 0x%02x\r\n < 0x%02x 0x%02x", src[0], dest[0], dest[1]);
	serialSendln(buffer);
}


static void strobe(uint8 addr) {
	uint16 src[] = {addr};
	uint16 dest[] = {0x00};
	spiTransmitAndReceiveData(TASK_RADIO_REG, &spiDataConfig, 1, src, dest);
	char buffer[30];
	snprintf(buffer, 30, "S 0x%02x\r\n < 0x%02x", src[0], dest[0]);
	serialSendln(buffer);
}

static void writeRegister(uint8 addr, uint8 val) {
	uint16 src[] = {addr, val};
	uint16 dest[] = {0x00, 0x00};
	spiTransmitAndReceiveData(TASK_RADIO_REG, &spiDataConfig, 2, src, dest);
	char buffer[30];
	snprintf(buffer, 30, "W 0x%02x 0x%02x\r\n < 0x%02x 0x%02x", src[0], src[1], dest[0], dest[1]);
	serialSendln(buffer);
	//vTaskDelay(pdMS_TO_TICKS(1));
}

static void readAllRegisters() {

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
    readRegister(PARTNUM);
    readRegister(VERSION);
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
