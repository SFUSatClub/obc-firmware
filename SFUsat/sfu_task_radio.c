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

typedef enum {
	IOCFG0,
	FIFOTHR,
	SYNC1,
	SYNC0,
	PKTLEN,
	PKTCTRL1,
	PKTCTRL0,
	FSCTRL1,
	FREQ2,
	FREQ1,
	FREQ0,
	MDMCFG4,
	MDMCFG3,
	MDMCFG2,
	MDMCFG1,
	DEVIATN,
	MCSM1,
	MCSM0,
	FOCCFG,
	AGCCTRL2,
	AGCCTRL1,
	WORCTRL,
	FSCAL3,
	FSCAL2,
	FSCAL1,
	FSCAL0,
	TEST2,
	TEST1,
	TEST0,
	NUM_CONFIG_REGISTERS
} SMARTRF_IDX;

/**
 * Named initializer (designated initializer)
 */
const uint16 SMARTRF_ADDRS[NUM_CONFIG_REGISTERS] = {
	[IOCFG0] = SMARTRF_SETTING_IOCFG0_ADDR,
	[FIFOTHR] = SMARTRF_SETTING_FIFOTHR_ADDR,
	[SYNC1] = SMARTRF_SETTING_SYNC1_ADDR,
	[SYNC0] = SMARTRF_SETTING_SYNC0_ADDR,
	[PKTLEN] = SMARTRF_SETTING_PKTLEN_ADDR,
	[PKTCTRL1] = SMARTRF_SETTING_PKTCTRL1_ADDR,
	[PKTCTRL0] = SMARTRF_SETTING_PKTCTRL0_ADDR,
	[FSCTRL1] = SMARTRF_SETTING_FSCTRL1_ADDR,
	[FREQ2] = SMARTRF_SETTING_FREQ2_ADDR,
	[FREQ1] = SMARTRF_SETTING_FREQ1_ADDR,
	[FREQ0] = SMARTRF_SETTING_FREQ0_ADDR,
	[MDMCFG4] = SMARTRF_SETTING_MDMCFG4_ADDR,
	[MDMCFG3] = SMARTRF_SETTING_MDMCFG3_ADDR,
	[MDMCFG2] = SMARTRF_SETTING_MDMCFG2_ADDR,
	[MDMCFG1] = SMARTRF_SETTING_MDMCFG1_ADDR,
	[DEVIATN] = SMARTRF_SETTING_DEVIATN_ADDR,
	[MCSM1] = SMARTRF_SETTING_MCSM1_ADDR,
	[MCSM0] = SMARTRF_SETTING_MCSM0_ADDR,
	[FOCCFG] = SMARTRF_SETTING_FOCCFG_ADDR,
	[AGCCTRL2] = SMARTRF_SETTING_AGCCTRL2_ADDR,
	[AGCCTRL1] = SMARTRF_SETTING_AGCCTRL1_ADDR,
	[WORCTRL] = SMARTRF_SETTING_WORCTRL_ADDR,
	[FSCAL3] = SMARTRF_SETTING_FSCAL3_ADDR,
	[FSCAL2] = SMARTRF_SETTING_FSCAL2_ADDR,
	[FSCAL1] = SMARTRF_SETTING_FSCAL1_ADDR,
	[FSCAL0] = SMARTRF_SETTING_FSCAL0_ADDR,
	[TEST2] = SMARTRF_SETTING_TEST2_ADDR,
	[TEST1] = SMARTRF_SETTING_TEST1_ADDR,
	[TEST0] = SMARTRF_SETTING_TEST0_ADDR
};

const uint16 SMARTRF_RX_VALS[NUM_CONFIG_REGISTERS] = {
	[IOCFG0] = SMARTRF_SETTING_IOCFG0_VAL_RX,
	[FIFOTHR] = SMARTRF_SETTING_FIFOTHR_VAL_RX,
	[SYNC1] = SMARTRF_SETTING_SYNC1_VAL_RX,
	[SYNC0] = SMARTRF_SETTING_SYNC0_VAL_RX,
	[PKTLEN] = SMARTRF_SETTING_PKTLEN_VAL_RX,
	[PKTCTRL1] = SMARTRF_SETTING_PKTCTRL1_VAL_RX,
	[PKTCTRL0] = SMARTRF_SETTING_PKTCTRL0_VAL_RX,
	[FSCTRL1] = SMARTRF_SETTING_FSCTRL1_VAL_RX,
	[FREQ2] = SMARTRF_SETTING_FREQ2_VAL_RX,
	[FREQ1] = SMARTRF_SETTING_FREQ1_VAL_RX,
	[FREQ0] = SMARTRF_SETTING_FREQ0_VAL_RX,
	[MDMCFG4] = SMARTRF_SETTING_MDMCFG4_VAL_RX,
	[MDMCFG3] = SMARTRF_SETTING_MDMCFG3_VAL_RX,
	[MDMCFG2] = SMARTRF_SETTING_MDMCFG2_VAL_RX,
	[MDMCFG1] = SMARTRF_SETTING_MDMCFG1_VAL_RX,
	[DEVIATN] = SMARTRF_SETTING_DEVIATN_VAL_RX,
	[MCSM1] = SMARTRF_SETTING_MCSM1_VAL_RX,
	[MCSM0] = SMARTRF_SETTING_MCSM0_VAL_RX,
	[FOCCFG] = SMARTRF_SETTING_FOCCFG_VAL_RX,
	[AGCCTRL2] = SMARTRF_SETTING_AGCCTRL2_VAL_RX,
	[AGCCTRL1] = SMARTRF_SETTING_AGCCTRL1_VAL_RX,
	[WORCTRL] = SMARTRF_SETTING_WORCTRL_VAL_RX,
	[FSCAL3] = SMARTRF_SETTING_FSCAL3_VAL_RX,
	[FSCAL2] = SMARTRF_SETTING_FSCAL2_VAL_RX,
	[FSCAL1] = SMARTRF_SETTING_FSCAL1_VAL_RX,
	[FSCAL0] = SMARTRF_SETTING_FSCAL0_VAL_RX,
	[TEST2] = SMARTRF_SETTING_TEST2_VAL_RX,
	[TEST1] = SMARTRF_SETTING_TEST1_VAL_RX,
	[TEST0] = SMARTRF_SETTING_TEST0_VAL_RX
};

const uint16 SMARTRF_TX_VALS[NUM_CONFIG_REGISTERS] = {
	[IOCFG0] = SMARTRF_SETTING_IOCFG0_VAL_TX,
	[FIFOTHR] = SMARTRF_SETTING_FIFOTHR_VAL_TX,
	[SYNC1] = SMARTRF_SETTING_SYNC1_VAL_TX,
	[SYNC0] = SMARTRF_SETTING_SYNC0_VAL_TX,
	[PKTLEN] = SMARTRF_SETTING_PKTLEN_VAL_TX,
	[PKTCTRL1] = SMARTRF_SETTING_PKTCTRL1_VAL_TX,
	[PKTCTRL0] = SMARTRF_SETTING_PKTCTRL0_VAL_TX,
	[FSCTRL1] = SMARTRF_SETTING_FSCTRL1_VAL_TX,
	[FREQ2] = SMARTRF_SETTING_FREQ2_VAL_TX,
	[FREQ1] = SMARTRF_SETTING_FREQ1_VAL_TX,
	[FREQ0] = SMARTRF_SETTING_FREQ0_VAL_TX,
	[MDMCFG4] = SMARTRF_SETTING_MDMCFG4_VAL_TX,
	[MDMCFG3] = SMARTRF_SETTING_MDMCFG3_VAL_TX,
	[MDMCFG2] = SMARTRF_SETTING_MDMCFG2_VAL_TX,
	[MDMCFG1] = SMARTRF_SETTING_MDMCFG1_VAL_TX,
	[DEVIATN] = SMARTRF_SETTING_DEVIATN_VAL_TX,
	[MCSM1] = SMARTRF_SETTING_MCSM1_VAL_TX,
	[MCSM0] = SMARTRF_SETTING_MCSM0_VAL_TX,
	[FOCCFG] = SMARTRF_SETTING_FOCCFG_VAL_TX,
	[AGCCTRL2] = SMARTRF_SETTING_AGCCTRL2_VAL_TX,
	[AGCCTRL1] = SMARTRF_SETTING_AGCCTRL1_VAL_TX,
	[WORCTRL] = SMARTRF_SETTING_WORCTRL_VAL_TX,
	[FSCAL3] = SMARTRF_SETTING_FSCAL3_VAL_TX,
	[FSCAL2] = SMARTRF_SETTING_FSCAL2_VAL_TX,
	[FSCAL1] = SMARTRF_SETTING_FSCAL1_VAL_TX,
	[FSCAL0] = SMARTRF_SETTING_FSCAL0_VAL_TX,
	[TEST2] = SMARTRF_SETTING_TEST2_VAL_TX,
	[TEST1] = SMARTRF_SETTING_TEST1_VAL_TX,
	[TEST0] = SMARTRF_SETTING_TEST0_VAL_TX
};


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
	snprintf(buffer, 30, "R 0x%02x\r\n < 0x%02x 0x%02x", addr, dest[0], dest[1]);
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

static void writeAllConfigRegisters() {
	int i;
	for (i = 0; i < NUM_CONFIG_REGISTERS; i++) {
		writeRegister(SMARTRF_ADDRS[i], SMARTRF_RX_VALS[i]);
	}
}

static void readAllConfigRegisters() {
	int i;
	for (i = 0; i < NUM_CONFIG_REGISTERS; i++) {
		readRegister(SMARTRF_ADDRS[i]);
	}
}

static void readAllStatusRegisters() {
	readRegister(PARTNUM);
	readRegister(VERSION);
	readRegister(FREQEST);
	readRegister(LQI);
	readRegister(RSSI);
	readRegister(MARCSTATE);
	readRegister(WORTIME1);
	readRegister(WORTIME0);
	readRegister(PKTSTATUS);
	readRegister(VCO_VC_DAC);
	readRegister(TXBYTES);
	readRegister(RXBYTES);
	readRegister(RCCTRL1_STATUS);
	readRegister(RCCTRL0_STATUS);
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

    strobe(SNOP);
    readAllStatusRegisters();
    readAllConfigRegisters();
    writeAllConfigRegisters();
    readAllConfigRegisters();

	return pdPASS;
}



BaseType_t radioCmd(char * toSend) {
	if (xQueueSendToBack(xSerialTXQueue, &toSend, 0) == pdPASS) {
		return pdPASS;
	} else {
		return pdFAIL;
	}
}
