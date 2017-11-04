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

const uint16 SMARTRF_VALS_RX[NUM_CONFIG_REGISTERS] = {
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

const uint16 SMARTRF_VALS_TX[NUM_CONFIG_REGISTERS] = {
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
 * Masks.
 */
#define WRITE_BIT	(0x00)
#define READ_BIT	(0x80)
#define BURST_BIT	(0x40)
/**
 * Chip Status Byte Masks (section 10.1, page 31).
 *
 * Status mask:
 * 		- (mask of flag << overlay mask on uint8). The shift is done this way to aid in readability.
 * State mask:
 * 		- mask out the status byte first using mask STATE before using STATE_X to check for equality.
 * 		- eg, to check if in STATE_RX:
 * 			- if ( (statusByte & STATE) == STATE_RX ) { // in STATE_RX }
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

/**
 * FIFO Buffers (section 10.2, page 32).
 *
 * FIFO_TX is write-only.
 * FIFO_RX is read-only.
 * Both are accessed through the 0x3F address with appropriate read/write bits set.
 */
#define FIFO_TX (0x3F)
#define FIFO_RX (0x3F)

static spiDAT1_t spiDataConfig;

QueueHandle_t xRadioTXQueue;
QueueHandle_t xRadioRXQueue;
QueueHandle_t xRadioCHIMEQueue;

static uint8 statusByte;

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

// TX task
void vRadioTX(void *pvParameters) {
	xRadioTXQueue = xQueueCreate(10, sizeof(portCHAR *));
	initRadio(); // required?

	// how does pvparameters work?
	uint8 txsize = 10;
	uint8 txsrc[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	writeToTxFIFO(txsrc, txsize);
}

// RX task
void vRadioRX(void *pvParameters){
	xRadioRXQueue = xQueueCreate(10, sizeof(portCHAR *));
	initRadio(); // required?
}

// CHIME task
void vRadioCHIME(void *pvParameters){
	xRadioCHIMEQueue = xQueueCreate(10, sizeof(portCHAR *));
	initRadio(); // required?
}


static uint8 readRegister(uint8 addr) {
	uint16 src[] = {addr | READ_BIT, 0x00};
	uint16 dest[] = {0x00, 0x00};
	spiTransmitAndReceiveData(TASK_RADIO_REG, &spiDataConfig, 2, src, dest);
	statusByte = dest[0] & 0xff;
	char buffer[30];
	snprintf(buffer, 30, "R 0x%02x\r\n < 0x%02x 0x%02x", addr, statusByte, dest[1]);
	serialSendln(buffer);
	return (dest[1] & 0xff);
}

static void writeRegister(uint8 addr, uint8 val) {
	uint16 src[] = {addr | WRITE_BIT, val};
	uint16 dest[] = {0x00, 0x00};
	spiTransmitAndReceiveData(TASK_RADIO_REG, &spiDataConfig, 2, src, dest);
	statusByte = dest[0] & 0xff;
	char buffer[30];
	snprintf(buffer, 30, "W 0x%02x 0x%02x\r\n < 0x%02x 0x%02x", src[0], src[1], statusByte, dest[1]);
	serialSendln(buffer);
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
	spiTransmitAndReceiveData(TASK_RADIO_REG, &spiDataConfig, 1, src, dest);
	statusByte = dest[0] & 0xff;
	char buffer[30];
	snprintf(buffer, 30, "S 0x%02x\r\n < 0x%02x", src[0], statusByte);
	serialSendln(buffer);
}

/**
 * Writes values from src buffer into TX FIFO (section 10.5, page 32).
 *
 * Assumes last operation was a write (header has R/W bit set to 0); that FIFO_BYTES_AVAILABLE contains number of bytes free in TX FIFO.
 * Currently only supports complete writes (when size of data to send is <= to numBytesInFIFO).
 * TODO: Support partial writes (write what we can first, eg, what's sent is = numBytesInFIFO, then send the rest of it later).
 *
 * @param src Data buffer to send
 * @param size Size of src (number of bytes)
 * @return 1 on successful write, 0 on failure
 */
static int writeToTxFIFO(const uint8 *src, uint8 size) {
	uint8 numBytesInFIFO = statusByte & FIFO_BYTES_AVAILABLE;
	// TODO: numBytesInFIFO is 4 bits; if it is 15, then >=15 bytes are free in TX FIFO. To get full count of free bytes in FIFO, read the proper status register.
	if (size > numBytesInFIFO || size == 0) { return 0; }
	/*
	 * TODO: Determine if it's better to rely on our initial local count of FIFO_BYTES_AVAILABLE via numBytesInFIFO,
	 * or update this continuously with the radio's count of FIFO_BYTES_AVAILABLE via most recent statusByte.
	 * I.E: What is more robust: to rely on the the SPI link, or this way?
	 */
	uint8 idx = 0;
	while (numBytesInFIFO >= 1 && idx + 1 <= size) {
		writeRegister(FIFO_TX, src[idx++]);
		numBytesInFIFO--;
	}
	return 1;
}

/**
 * Reads values from RX FIFO and  them into dest buffer from dest[0] to dest [size - 1].
 *
 * Assumes last operation was a read (header has R/W bit set to 1); that FIFO_BYTES_AVAILABLE contains number of bytes to read from RX FIFO.
 *
 * @param dest Values read will be stored into this array
 * @param size Size of dest (number of bytes)
 * @return 1 if all bytes in FIFO read successfully, 0 otherwise (partial read, dest is too small to fit the rest, etc)
 */
static int readFromRxFIFO(uint8 *dest, uint8 size) {
	uint8 numBytesInFIFO = statusByte & FIFO_BYTES_AVAILABLE;
	uint8 idx = 0;
	while (numBytesInFIFO > 0 && idx + 1 <= size) {
		dest[idx++] = readRegister(FIFO_RX);
		numBytesInFIFO--;
	}
	/**
	 * If there are no bytes left in the FIFO yet we've managed to read at least one, then all bytes have been read successfully.
	 */
	return (numBytesInFIFO == 0 && idx >= 1);
}

static void writeAllConfigRegisters() {
	uint8 i = 0;
	while (i < NUM_CONFIG_REGISTERS) {
		writeRegister(SMARTRF_ADDRS[i], SMARTRF_VALS_RX[i]);
		i++;
	}
}

static void readAllConfigRegisters() {
	uint8 i = 0;
	while (i < NUM_CONFIG_REGISTERS) {
		readRegister(SMARTRF_ADDRS[i++]);
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
     * CC1101 is active-low, on CS0
     * SPI_CS_0 -> 0xFE -> 11111110
     */
    spiDataConfig.CSNR = SPI_CS_0;

    strobe(SNOP);
    readAllStatusRegisters();
    readAllConfigRegisters();
    writeAllConfigRegisters();
    readAllConfigRegisters();
    strobe(SIDLE);

    uint8 test[] = {0, 3, 9, 27, 14};
    /**
     * R/W bit was previously set to 0 (WRITE) from the above strobe; status byte will have FIFO_BYTES_AVAILABLE for TX FIFO, so no need for another strobe to update our status byte.
     */
    int i = 0;
    for (i = 0; i < 11; i++) {
        writeToTxFIFO(test, 5);
        readRegister(TXBYTES);
    }
    /**
     * Set R/W bit to 1 (READ) to get FIFO_BYTES_AVAILABLE of RX FIFO in status byte.
     */
    strobe(SNOP | READ_BIT);
    readRegister(RXBYTES);
    readFromRxFIFO(test, 5);

	return pdPASS;
}

BaseType_t radioCmd(char * toSend) {
	if (xQueueSendToBack(xSerialTXQueue, &toSend, 0) == pdPASS) {
		return pdPASS;
	} else {
		return pdFAIL;
	}
}
