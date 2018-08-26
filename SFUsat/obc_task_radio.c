/*
 * sfu_task_radio.c
 *
 *  Created on: Apr 30, 2017
 *      Author: steven
 */

#include <obc_hardwaredefs.h>
#include <obc_smartrf_cc1101.h>
#include <obc_task_radio.h>
#include <obc_uart.h>
#include "string.h"

//Interrupt stuff
#include "rtos_semphr.h"
#include "rtos_task.h"

typedef enum {
	IOCFG0,
	IOCFG1,
	IOCFG2,
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
	MDMCFG0,
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
	[IOCFG1] = SMARTRF_SETTING_IOCFG1_ADDR,
	[IOCFG2] = SMARTRF_SETTING_IOCFG2_ADDR,
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
	[MDMCFG0] = SMARTRF_SETTING_MDMCFG0_ADDR,
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


const uint16 SMARTRF_VALS_TX[NUM_CONFIG_REGISTERS] = {
		[IOCFG0] = SMARTRF_SETTING_IOCFG0_VAL_TX,
		[IOCFG1] = SMARTRF_SETTING_IOCFG1_VAL_TX,
		[IOCFG2] = SMARTRF_SETTING_IOCFG2_VAL_TX,
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
		[MDMCFG0] = SMARTRF_SETTING_MDMCFG0_VAL_TX,
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
 * Masks.
 */
#define WRITE_BIT	(0x00)
#define READ_BIT	(0x80)
#define BURST_BIT	(0x40)
/**
 * Chip Status Byte Masks (section 10.1, page 31).
 *
 * Use the IS_STATE macro below to check state.
 *
 * Status mask:
 * 		- (mask of flag << overlay mask on uint8). The shift is done this way to aid in readability.
 * State mask:
 * 		- mask out the status byte first using mask STATE before using STATE_X to check for equality.
 * 		- eg, to check if in STATE_RX:
 * 			- if ( (statusByte & STATE) == (STATE_RX << 4)) { // in STATE_RX }
 */
#define CHIP_RDY				(0b1000 << 4)	// Bits 7   Stays high until power and crystal have stabilized. Should always be low when using the SPI interface.
#define STATE					(0b0111 << 4)	// Bits 6:4 Indicates the current main state machine mode.
#define STATE_IDLE 					(0b000)			// IDLE state (Also reported for some transitional states instead of SETTLING or CALIBRATE).
#define STATE_RX 					(0b001)			// Receive mode.
#define STATE_TX 					(0b010)			// Transmit mode.
#define STATE_FSTXON 				(0b011)			// Fast TX ready.
#define STATE_CALIBRATE 			(0b100)			// Frequency synthesizer calibration is running.
#define STATE_SETTLING 				(0b101)			// PLL is settling.
#define STATE_RXFIFO_OVERFLOW 		(0b110)			// RX FIFO has overflowed. Read out any useful data, then flush the FIFO with SFRX.
#define STATE_TXFIFO_UNDERFLOW 		(0b111)			// TX FIFO has underflowed. Acknowledge with SFTX.
#define FIFO_BYTES_AVAILABLE	(0b1111 << 0)	// Bits 3:0 The number of bytes available in the RX FIFO or free bytes in the TX FIFO.
/**
 * Macro to easily check state from statusByte.
 *
 * Assumes statusByte is defined and up-to-date.
 * Strobe a NOP with strobe(SNOP) to update statusByte.
 *
 * Example:
 * 		- if ( IS_STATE(STATE_IDLE) ) { // in STATE_IDLE }
 */
#define IS_STATE(x) ( (statusByte & STATE) >> 4 == (x) )

/**
 * Command Strobe Registers (section 29.0, page 67).
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
 * Status Registers (section 29.3, page 92).
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
#define NUM_STATUS_REGISTERS (14)

/**
 * Status Register Masks (section 29.3, page 94).
 *
 * Use these masks to extract relevant fields when reading from certain status registers above.
 */
#define TXFIFO_UNDERFLOW	(0b10000000) // TXBYTES Bits 7    Indicates if TX FIFO has underflowed.
#define NUM_TXBYTES			(0b01111111) // TXBYTES Bits 6:0  Number of bytes in TX FIFO.
#define RXFIFO_OVERFLOW		(0b10000000) // RXBYTES Bits 7    Indicates if RX FIFO has overflowed.
#define NUM_RXBYTES			(0b01111111) // RXBYTES Bits 6:0  Number of bytes in RX FIFO.
#define CRC_OK				(0b10000000) // PKTSTATUS Bit 7	  The last CRC comparison matched. Cleared when entering/restarting RX mode.
#define PKTSTATUS_CS		(0b01000000) // PKTSTATUS Bit 6   Carrier sense. Cleared when entering IDLE mode.

/**
 * FIFO Buffers (section 10.2, page 32).
 *
 * FIFO_TX is write-only.
 * FIFO_RX is read-only.
 * Both are accessed through the 0x3F address with appropriate read/write bits set.
 */
#define PA_TABLE_ADDR		(0x3E)
#define PA_TABLE_SETTING	(0x60)
#define FIFO_TX				(0x3F)
#define FIFO_RX				(0x3F)

/**
 * Begin SFUSat-specific symbols.
 */
#define FIFO_LENGTH			(64)


static spiDAT1_t spiDataConfig;

QueueHandle_t xRadioTXQueue;
QueueHandle_t xRadioRXQueue;
QueueHandle_t xRadioCHIMEQueue;

/**
 * This global statusByte gets set on each RF SPI transaction.
 * Used by the IS_STATE(STATE_X) macro.
 * See "Chip Status Byte Masks" defined above for more information.
 */
static uint8 statusByte;

/**
 * Forward declarations
 */
static uint8 readRegister(uint8 addr);
static int readFromRxFIFO(uint8 *dest, uint8 numBytesToRead);
static void strobe(uint8 addr);
static uint8 * readAllStatusRegisters();
//void rfTestSequence();
static void printStatusByte();
void read_RX_FIFO();
static int8_t sendPacket(const uint8_t *payload, uint8_t size);
//static void sendPacket(const uint8_t *payload, uint8_t size);
static void writeRegister(uint8 addr, uint8 val);
bool validateCommand(const uint8_t *input, uint8_t size);
static int receivePacket(uint8_t *destPayload, uint8_t size);

#define RF_CALLSIGN			("VA7TSN")
#define RF_CALLSIGN_LEN		(sizeof(RF_CALLSIGN) - 1) // Don't include the null terminator
#define PACKET_LENGTH 		(SMARTRF_SETTING_PKTLEN_VAL_TX - RF_CALLSIGN_LEN)

//Declarations for RF Interrupt
//SemaphoreHandle_t gioRFSem;
TaskHandle_t xRFInterruptTaskHandle;
bool enableRFISR = 0;
bool rfInhibit = 1;

void vRadioTask(void *pvParameters) {
	xRadioTXQueue = xQueueCreate(40, sizeof(RadioDAT_t));
	xRadioRXQueue = xQueueCreate(10, sizeof(portCHAR));
	rfInhibit = 1;
	serialSendQ("RF INHIBITED");
	//vTaskDelay(pdMS_TO_TICKS(30000));
	rfInhibit = 0;
	serialSendQ("RF ENABLED");
	enableRFISR = 0;

	initRadio();
	gioEnableNotification(RF_IRQ_PORT, RF_IRQ_PIN);
	strobe(SRX);

	char buffer[100] = {'\0'};
	uint8_t rxbuf[FIFO_LENGTH] = {'\0'};
	uint8_t CRC_status_int;

	RadioDAT_t prevQueuedPacket = { 0 };
	while (1) {
		enableRFISR = 1;

		/**
		 * Notification example.
		 * TODO: Use flags on notif instead of overwrite. Make dedicated notification handler.
		 * ulTaskNotifyTake( BaseType_t xClearCountOnExit, TickType_t xTicksToWait );
		 * https://www.freertos.org/ulTaskNotifyTake.html
		 */
		uint32 notif = ulTaskNotifyTake(pdTRUE, 0);

		switch (notif) {
			case 0xBEEFDEAD: {
				spiEnableLoopback(RF_SPI_REG, Analog_Lbk);
				break;
			} case 0xDEADBEEF: {
				spiDisableLoopback(RF_SPI_REG);
				break;
			} case RF_NOTIF_TX: {
				rfTestSequence();
				break;
			} case RF_NOTIF_RX: {
				uint8_t tries = 0;
				uint8_t rxbytes = 0;
				uint8_t rx_numbytes = 0;
				do {
					CRC_status_int = readRegister(PKTSTATUS) & CRC_OK;
					rxbytes = readRegister(RXBYTES);
					rx_numbytes = rxbytes & NUM_RXBYTES;
					snprintf(buffer, sizeof(buffer), "Try %d: RX ISR numBytes: 0x%x, CRC: %s", tries, rx_numbytes, CRC_status_int ? "OK!" : "BAD!");
					serialSendln(buffer);
				} while (tries++ < 10 && rx_numbytes < SMARTRF_SETTING_PKTLEN_VAL_TX);


				memset( rxbuf, '\0', sizeof(rxbuf) );
				uint8_t bytes_actually_read = receivePacket(rxbuf, sizeof(rxbuf));
				rxbuf[sizeof(rxbuf) - 1] = '\0'; // just in case

				/* complete command and CRC are good, feed to UART */
				if(validateCommand(rxbuf, bytes_actually_read)) {
					uint8_t cmd_len = strlen((const char*)rxbuf);
					uint8_t uart_cnt;
					for(uart_cnt = RF_CALLSIGN_LEN; uart_cnt < cmd_len; uart_cnt++) {
						/* loop through the command, including \r\n, don't send null char */
						if(xQueueSendToBack(xSerialRXQueue, &rxbuf[uart_cnt], 500) != pdTRUE) {
							serialSendln("RF RX -> UART ERR");
							break;
						}
					}
				} else {
					serialSend("Rcvd invalid cmd from rf: ");
					serialSendln((const char *)rxbuf);
//					uint8_t i = 0;
//					for ( i = 0; i < bytes_actually_read; i++ ) {
//						snprintf(buffer, sizeof(buffer), "RX Byte #%d: 0x%02x", i, rxbuf[i]);
//						serialSendln(buffer);
//					}
				}

				break;
			} case RF_NOTIF_RESET: {
				strobe(SIDLE);
				strobe(SFRX);
				strobe(SFTX);
				strobe(SRX);
				break;
			} case RF_NOTIF_STX: {
				strobe(STX);
				break;
			}
		}

		if(IS_STATE(STATE_IDLE)){	//TODO: this should be handled by reg config or ISR
			strobe(SRX);
		}

		RadioDAT_t currQueuedPacket;
		while (xQueueReceive(xRadioTXQueue, &currQueuedPacket, pdMS_TO_TICKS(1000)) == pdPASS) {
			snprintf(buffer, sizeof(buffer), "Dequeued 0x%02x of %d bytes from xRadioTXQueue", currQueuedPacket.unused, currQueuedPacket.size);
			serialSendln(buffer);
			if (prevQueuedPacket.size >= PACKET_LENGTH) {
				if (sendPacket(prevQueuedPacket.data, prevQueuedPacket.size)) {
					prevQueuedPacket.size = 0;
				} else {
//					snprintf(buffer, sizeof(buffer), "RF(1) FAILED buffering packet 0x%02x of %d bytes; requeueing...", prevQueuedPacket.unused, prevQueuedPacket.size);
//					serialSendln(buffer);
					prevQueuedPacket.size = 0;
					xQueueSendToBack(xRadioTXQueue, &prevQueuedPacket, 0);
				}
			}
			if (prevQueuedPacket.size + currQueuedPacket.size < PACKET_LENGTH) {
//				snprintf(buffer, sizeof(buffer), "RF(2) Buffering packet 0x%02x of %d bytes", currQueuedPacket.unused, currQueuedPacket.size);
//				serialSendln(buffer);
				memcpy(prevQueuedPacket.data + prevQueuedPacket.size, currQueuedPacket.data, currQueuedPacket.size);
				prevQueuedPacket.size += currQueuedPacket.size;
			}  else if (prevQueuedPacket.size + currQueuedPacket.size == PACKET_LENGTH) {
//				snprintf(buffer, sizeof(buffer), "RF(3) Buffering packet 0x%02x of %d bytes", currQueuedPacket.unused, currQueuedPacket.size);
//				serialSendln(buffer);
				memcpy(prevQueuedPacket.data + prevQueuedPacket.size, currQueuedPacket.data, currQueuedPacket.size);
				prevQueuedPacket.size += currQueuedPacket.size;
				if (sendPacket(prevQueuedPacket.data, prevQueuedPacket.size)) {
					prevQueuedPacket.size = 0;
				} else {
//					snprintf(buffer, sizeof(buffer), "RF(3.5) FAILED buffering packet 0x%02x of %d bytes; requeueing...", prevQueuedPacket.unused, prevQueuedPacket.size);
//					serialSendln(buffer);
					prevQueuedPacket.size = 0;
					xQueueSendToBack(xRadioTXQueue, &prevQueuedPacket, 0);
				}
			}  else if (prevQueuedPacket.size + currQueuedPacket.size > PACKET_LENGTH) {
				uint8_t room = PACKET_LENGTH - prevQueuedPacket.size;
				memcpy(prevQueuedPacket.data + prevQueuedPacket.size, currQueuedPacket.data, room);
				prevQueuedPacket.size += room;
				if (sendPacket(prevQueuedPacket.data, prevQueuedPacket.size)) {
//					snprintf(buffer, sizeof(buffer), "RF(4) Buffering split packet 0x%02x of %d bytes", currQueuedPacket.unused, currQueuedPacket.size - room);
//					serialSendln(buffer);
					memcpy(prevQueuedPacket.data, currQueuedPacket.data + room, currQueuedPacket.size - room);
					prevQueuedPacket.size = currQueuedPacket.size - room;
				} else {
//					snprintf(buffer, sizeof(buffer), "RF(5) FAILED buffering split packet 0x%02x of %d bytes; requeueing both...", prevQueuedPacket.unused, prevQueuedPacket.size);
//					serialSendln(buffer);
					xQueueSendToBack(xRadioTXQueue, &prevQueuedPacket, 0);
					prevQueuedPacket.size = 0;
					RadioDAT_t currSplitQueuedPacket = { 0 };
					memcpy(currSplitQueuedPacket.data, currQueuedPacket.data + room, currQueuedPacket.size - room);
					currSplitQueuedPacket.size = currQueuedPacket.size - room;
					xQueueSendToBack(xRadioTXQueue, &currSplitQueuedPacket, 0);
				}

			}
		}
	}
}

/**
 * Check that call sign and /r/n are in the received data
 */
bool validateCommand(const uint8_t *input, uint8_t size){
	if (size < RF_CALLSIGN_LEN + 2) {
		return 0;
	}
	size_t i;
	for ( i = 0; i < RF_CALLSIGN_LEN; i++ ) {
		if ( input[i] != RF_CALLSIGN[i] ) {
			return 0;
		}
	}
	for ( i = RF_CALLSIGN_LEN; i < size - 1; i++ ) {
		if ( input[i] == '\r' && input[i + 1] == '\n' ) {
			return 1;
		}
	}
	return 0;
}

static int8_t sendPacket(const uint8_t *payload, uint8_t size) {
	char buffer[100] = {'\0'};

	uint8_t txbytes = readRegister(TXBYTES);
	uint8_t tx_underflowed = txbytes & TXFIFO_UNDERFLOW;
	uint8_t tx_numbytes = txbytes & NUM_TXBYTES;
	if (IS_STATE(STATE_TXFIFO_UNDERFLOW)) {
		serialSendln("TX Underflowed; Strobing SFTX...");
		strobe(SFTX);
	}

	snprintf(buffer, sizeof(buffer), "TX underflowed:%s numbytes:%d FIFO_BYTES_AVAILABLE: 0x%x\n",
			tx_underflowed ? "yes" : "no",
			tx_numbytes,
			statusByte & FIFO_BYTES_AVAILABLE);
	serialSend(buffer);

	if ( FIFO_LENGTH - tx_numbytes < SMARTRF_SETTING_PKTLEN_VAL_TX ) {
		serialSendln("Could not fit one packet to TX FIFO; skipping");
		return 0;
	}

	serialSendln("Sending one packet to TX FIFO");
	uint8_t i;
	for ( i = 0; i < RF_CALLSIGN_LEN; i++ ) {
		writeRegister(FIFO_TX, RF_CALLSIGN[i]);
	}
	for ( i = 0; i < size && i < SMARTRF_SETTING_PKTLEN_VAL_TX - RF_CALLSIGN_LEN; i++ ) {
		writeRegister(FIFO_TX, payload[i]);
	}
	const uint8_t payload_bytes_written = i;
	uint8_t num_bytes_padding = SMARTRF_SETTING_PKTLEN_VAL_TX - payload_bytes_written - RF_CALLSIGN_LEN;
	i = 0;
	for ( i = 0; i < num_bytes_padding; i++ ) {
		writeRegister(FIFO_TX, 0);
	}

	strobe(STX);
	return payload_bytes_written;
}

/**
 * TODO: Do we want to receive one full "packet" first? I.e., only start reading from RX FIFO if rx_numbytes > packetsize ?
 * Or should we read everything we can and buffer a full packet on our end?
 *
 * 		- Strip off the callsign.
 */
static int receivePacket(uint8_t *destPayload, uint8_t destSize) {
	uint8_t rxbytes = readRegister(RXBYTES);
	uint8_t rx_overflowed = rxbytes & RXFIFO_OVERFLOW;
	uint8_t rx_numbytes = rxbytes & NUM_RXBYTES;
	uint8_t bytes_actually_read = 0;
	uint8_t i = 0;
	while (i < rx_numbytes && i < destSize) {
		destPayload[i++] = readRegister(FIFO_RX);
	}
	if (rx_overflowed) {
		serialSendln("RX Overflowed; data loss occurred. Strobing SFRX...");
		strobe(SFRX);
	}
	bytes_actually_read = i;
	return bytes_actually_read;
}

void rfTestSequence() {
	/* note: always give an array of len [SMARTRF_SETTING_PKTLEN_VAL_TX - 6]
	 * to sendPacket.
	 *
	 * Place a null character at the end of the valid data.
	 */

	strobe(SNOP);
//	char buffer[100] = {'\0'};
	uint8 mystr[] = "ack\r\n\0";
	uint8 test[SMARTRF_SETTING_PKTLEN_VAL_TX] = { 0 };

	strcpy((char *)test, (char *)mystr);

	sendPacket(test, SMARTRF_SETTING_PKTLEN_VAL_TX);

	strobe(SNOP);
	printStatusByte();
	if(IS_STATE(STATE_IDLE)){
		strobe(SRX);
	}
	printStatusByte();

	//TODO: use timer and return timeout error if radio never returns to IDLE, this should be done for most state transitions
}

static uint8 readRegister(uint8 addr) {
	uint16 src[] = {addr | READ_BIT, 0x00};
	uint16 dest[] = {0x00, 0x00};
	spiTransmitAndReceiveData(RF_SPI_REG, &spiDataConfig, 2, src, dest);
	statusByte = dest[0] & 0xff; //table 23 of CC1101 Datasheet
	/*char buffer[30];
	snprintf(buffer, 30, "R 0x%02x\r\n < 0x%02x 0x%02x", addr, statusByte, dest[1]);
	serialSendln(buffer);
	*/
	return (dest[1] & 0xff);
}

static void writeRegister(uint8 addr, uint8 val) {
	uint16 src[] = {addr | WRITE_BIT, val};
	uint16 dest[] = {0x00, 0x00};
	spiTransmitAndReceiveData(RF_SPI_REG, &spiDataConfig, 2, src, dest);
	statusByte = dest[0] & 0xff;
}

/**
 * Send single byte instruction to CC1101 (section 10.4, page 32).
 *
 * The R/W bit of addr can be either one or zero and will determine how the FIFO_BYTES_AVAILABLE field in the status byte should be interpreted.
 * By default, the R/W bit is not set (0), so strobes to addresses will appear as writes.
 *
 * @param addr The address to strobe
 */
static void strobe(uint8 addr) {
	uint16 src[] = {addr};
	uint16 dest[] = {0x00};
	spiTransmitAndReceiveData(RF_SPI_REG, &spiDataConfig, 1, src, dest);
	statusByte = dest[0] & 0xff;
	char buffer[30];
	snprintf(buffer, 30, "S 0x%02x\r\n < 0x%02x", src[0], statusByte);
	serialSendln(buffer);
}

static void printStatusByte() {
	char buffer[30];
	snprintf(buffer, sizeof(buffer), "StatusByte: 0x%02x\n", statusByte);
	serialSend(buffer);
}

/**
 * Writes values from src buffer into TX FIFO (section 10.5, page 32).
 *
 * Assumes last operation was a write (header has R/W bit set to 0); that FIFO_BYTES_AVAILABLE contains number of bytes free in TX FIFO.
 * Currently only supports complete writes (when size of data to send is <= to numBytesAvailInFIFO).
 * TODO: Support partial writes (write what we can first, eg, what's sent is = numBytesAvailInFIFO, then send the rest of it later).
 *
 * @param src Data buffer to send
 * @param size Size of src (number of bytes)
 * @return -1 on underflow, 0 on failure (insufficient size), 1 on successful write, 2 on unexpected size
 */
static int writeToTxFIFO(const uint8 *src, uint8 numBytesToWrite) {
	uint8 numBytesAvailInFIFO = statusByte & FIFO_BYTES_AVAILABLE;
	/**
	 * numBytesAvailInFIFO is 4 bits; if it is 0xF, then >= 15 bytes are free in TX FIFO (i.e., there could be
	 * anywhere from 15-64 bytes free).
	 * Thus, comparing size to numBytesAvailInFIFO gives a definitive result only if numBytesAvailInFIFO is < 0xF.
	 * To get the actual full count of free bytes in FIFO, we then read the proper status register.
	 */
	if (numBytesToWrite > numBytesAvailInFIFO) {
		if (numBytesAvailInFIFO >= 0xF) {
			numBytesAvailInFIFO = readRegister(TXBYTES);
			if (numBytesAvailInFIFO & TXFIFO_UNDERFLOW) {
				return -1;
			}
			numBytesAvailInFIFO = FIFO_LENGTH - (numBytesAvailInFIFO & NUM_TXBYTES);
			/**
			 * Now numBytesAvailInFIFO is number of bytes currently in RF TX FIFO.
			 */
			if (numBytesToWrite > numBytesAvailInFIFO) {
				return 0;
			}
		} else {
			return 0;
		}
	}
	/*
	 * TODO: Determine if it's better to rely on our initial local count of FIFO_BYTES_AVAILABLE via numBytesAvailInFIFO,
	 * or update this continuously with the radio's count of FIFO_BYTES_AVAILABLE via most recent statusByte.
	 * I.E: What is more robust: to rely on the the SPI link, or this way?
	 */
	uint8 idx = 0;
	while (numBytesAvailInFIFO > 0 && idx < numBytesToWrite) {
		writeRegister(FIFO_TX, src[idx++]); //FIFO_TX single byte access
		/**
		 * TODO: Opportunity to do a sanity check on the new numBytesAvailInFIFO here with our current numBytesAvailInFIFO.
		 * uint8 newnumBytesAvailInFIFO = statusByte & FIFO_BYTES_AVAILABLE;
		 */
		numBytesAvailInFIFO--;
	}
	return 1;
}

/**
 * Reads values from RX FIFO into dest buffer from dest[0] to dest[numBytesToRead - 1].
 *
 * TODO: Currently the code is always reading register RXBYTES no matter what.
 * 		 Fix it to use RX's FIFO_BYTES_AVAILABLE first.
 * 		 See writeToTxFIFO for details.
 * 		 Basically make the below comment valid again.
 * Assumes last operation was a read (header has R/W bit set to 1); that FIFO_BYTES_AVAILABLE contains number of bytes to read from RX FIFO.
 *
 * @param dest Values read will be stored into this array
 * @param size Size of dest (number of bytes)
 * @return 1 if all bytes in FIFO read successfully, 0 otherwise (partial read, dest is too small to fit the rest, etc)
 */
static int readFromRxFIFO(uint8 *dest, uint8 numBytesToRead) {
	//uint8 numBytesAvailInFIFO = statusByte & FIFO_BYTES_AVAILABLE;
	uint8 numBytesAvailInFIFO = readRegister(RXBYTES);
	uint8 idx = 0;
	while (numBytesAvailInFIFO > 0 && idx < numBytesToRead) {
		dest[idx++] = readRegister(FIFO_RX);
		numBytesAvailInFIFO--;
	}
	/**
	 * If there are no bytes left in the FIFO yet we've managed to read at least one, then all bytes have been read successfully.
	 */
	return (numBytesAvailInFIFO == 0 && idx >= 1);
}

static void writeAllConfigRegisters(const uint16 config[NUM_CONFIG_REGISTERS]) {
	uint8 i = 0;
	while (i < NUM_CONFIG_REGISTERS) {
		writeRegister(SMARTRF_ADDRS[i], config[i]);
		i++;
	}
}

static uint8 * readAllStatusRegisters() {
	static uint8 contents[NUM_STATUS_REGISTERS];
	contents[0] = readRegister(PARTNUM);
	contents[1] = readRegister(VERSION);
	contents[2] = readRegister(FREQEST);
	contents[3] = readRegister(LQI);
	contents[4] = readRegister(RSSI);
	contents[5] = readRegister(MARCSTATE);
	contents[6] = readRegister(WORTIME1);
	contents[7] = readRegister(WORTIME0);
	contents[8] = readRegister(PKTSTATUS);
	contents[9] = readRegister(VCO_VC_DAC);
	contents[10] = readRegister(TXBYTES);
	contents[11] = readRegister(RXBYTES);
	contents[12] = readRegister(RCCTRL1_STATUS);
	contents[13] = readRegister(RCCTRL0_STATUS);
	return contents;
}

static int checkConfig(const uint16_t config[NUM_CONFIG_REGISTERS]) {
	uint8_t i = 0;
	uint8_t err = 0;
	while (i < NUM_CONFIG_REGISTERS) {
		uint8_t regVal = readRegister(SMARTRF_ADDRS[i]);
		if(config[i] != regVal){
			char buffer[30];
			snprintf(buffer, 30, "Reg %02x = %02x != %02x", SMARTRF_ADDRS[i], regVal, config[i]);
			serialSendln(buffer);
			err = 1;
		}
		i++;
	}
	if (err) {
		return 1;
	}
	serialSendln("All REG configs good");
	return 0;
}

static int configureRadio(const uint16_t config[NUM_CONFIG_REGISTERS], const uint8_t PA_TABLE) {
    writeAllConfigRegisters(SMARTRF_VALS_TX);
    writeRegister(PA_TABLE_ADDR, PA_TABLE);
    return checkConfig(SMARTRF_VALS_TX);
}

//void vRFInterruptTask(void *pvParameters){
//	// This task gets invoked by the ISR callback (notification) whenever the pin sees a rising edge
//
//	//pulled from example
//	while(1){
//		serialSendln("It's an RF one!");
//		vTaskDelay(pdMS_TO_TICKS(5000));
//
//		//uint8 num_of_RXBYTES = RXBYTES & NUM_RXBYTES;
//		//uint8 RX_dest[RXBYTES] = {0};
//		//char buffer[RXBYTES];
//		//char buffer2[16];
//		//uint8 count = 0;
//		//uint8 size = 0;
//		//const uint8 RX_FIFO_MAX_SIZE = 111100; //60
//
//
//		xSemaphoreTake(gioRFSem, portMAX_DELAY);
//
//		read_RX_FIFO();
//
//		//snprintf(buffer2, 30, "RXBYTES: %02x \n", num_of_RXBYTES);
//		//serialSendln(buffer2);
//
//
////		while (NUM_RXBYTES < RX_FIFO_MAX_SIZE) {
////			//(NUM_RXBYTES);
////			serialSendQ("LOOPING HERE");
////			vTaskDelay(pdMS_TO_TICKS(1000));
////		}
//
//
//
////		if (readRegister(RXBYTES) != 0) {
////			readFromRxFIFO(RX_dest, num_of_RXBYTES);
////			for (count = 0; count < num_of_RXBYTES; count++) {
////				snprintf(buffer, 30, "RX: %02x %02x \n", count, RX_dest[count]);
////				serialSendln(buffer);
////			}
////		}
//		//strobe(SFRX);
//	}
//}

//void read_RX_FIFO() {
//	serialSendln("Reading RX_FIFO");
//	uint8 num_of_RXBYTES = RXBYTES & NUM_RXBYTES;
//	uint8 RX_dest[RXBYTES] = { 0 };
//	char buffer[RXBYTES];
//	//char buffer2[16];
//	uint8 count = 0;
//	uint8 pktlen = 60;
//
//	if (readRegister(RXBYTES) >= pktlen) {
//		readFromRxFIFO(RX_dest, num_of_RXBYTES);
//		for (count = 0; count < num_of_RXBYTES; count++) {
//			snprintf(buffer, 30, "RX: %02x %02x \n", count, RX_dest[count]);
//			serialSendln(buffer);
//		}
//	}
//
//}

//void rf_interrupt_init(void){
//	//pulled from example
//	gioRFSem = xSemaphoreCreateBinary();
//	xRFInterruptTaskHandle = NULL;
//
//	if(gioRFSem != NULL){ // setup the task to handle the ISR
//		xTaskCreate(vRFInterruptTask, "RF Interrupt", 200, NULL, 3, xRFInterruptTaskHandle);
//	}
//
//	gioEnableNotification(RF_IRQ_PORT, RF_IRQ_PIN); // enable the notification callback for this particular pin
//}

void gio_notification_RF(gioPORT_t *port, uint32 bit){
	// This gets called by the ISR callback (notification). Doesn't need to be in another function, but this just keeps things tidy
	// See FreeRTOS tutorial guide pg. 200

	//pulled from Example

	BaseType_t xHigherPriorityTaskWoken;
	xHigherPriorityTaskWoken = pdFALSE;

	if(port == RF_IRQ_PORT && bit == RF_IRQ_PIN){ // Always need to check which pin actually triggered the interrupt
//		xSemaphoreGiveFromISR(gioRFSem, &xHigherPriorityTaskWoken);
		if(xRadioTaskHandle != NULL && enableRFISR){
			xTaskNotifyFromISR(xRadioTaskHandle, RF_NOTIF_RX, eSetValueWithOverwrite, &xHigherPriorityTaskWoken);
			portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
		}
	}
}

BaseType_t initRadio() {
	//what task should SPI initialization occure in?
	spiDataConfig.CS_HOLD = RF_CONFIG_CS_HOLD;
	spiDataConfig.WDEL = RF_CONFIG_WDEL;
    spiDataConfig.DFSEL = RF_CONFIG_DFSEL;
    /*
     * Encoded SPI Transfer Group Chip Select
     * CC1101 is active-low, on CS0
     * SPI_CS_0 -> 0xFE -> 11111110
     */
    spiDataConfig.CSNR = RF_CONFIG_CS_LB;

    uint8 *stat = readAllStatusRegisters();
    char buffer[30];

    strobe(SRES);
    strobe(SNOP);

    snprintf(buffer, 30, "Radio Status Registers:");
    serialSendln(buffer);
    snprintf(buffer, 30, "%02x, %02x, %02x, %02x, %02x, %02x, %02x, %02x, %02x, %02x, %02x, %02x, %02x, %02x, ",
    			stat[0], stat[1], stat[2], stat[3], stat[4], stat[5], stat[6], stat[7], stat[8], stat[9],
				stat[10], stat[11], stat[12], stat[13]);
    serialSendln(buffer);

    if(configureRadio(SMARTRF_VALS_TX, PA_TABLE_SETTING)){
    	snprintf(buffer, 30, "radio registers do not match!");
    	serialSendln(buffer);
    }


    strobe(SNOP);
//attach irq

    strobe(SRX);
//move here ##

//Move ##
	return pdPASS;
}

BaseType_t radioCmd(char * toSend) {
	if (xQueueSendToBack(xSerialTXQueue, &toSend, 0) == pdPASS) {
		return pdPASS;
	} else {
		return pdFAIL;
	}
}
