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
	[PKTLEN] = SMARTRF_SETTING_PKTLEN_ADDR,
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
	[MCSM0] = SMARTRF_SETTING_MCSM0_ADDR,
	[FOCCFG] = SMARTRF_SETTING_FOCCFG_ADDR,
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
	[MCSM0] = SMARTRF_SETTING_MCSM0_VAL_TX,
	[FOCCFG] = SMARTRF_SETTING_FOCCFG_VAL_TX,
	[FSCAL3] = SMARTRF_SETTING_FSCAL3_VAL_TX,
	[FSCAL2] = SMARTRF_SETTING_FSCAL2_VAL_TX,
	[FSCAL1] = SMARTRF_SETTING_FSCAL1_VAL_TX,
	[FSCAL0] = SMARTRF_SETTING_FSCAL0_VAL_TX,
	[TEST2] = SMARTRF_SETTING_TEST2_VAL_TX,
	[TEST1] = SMARTRF_SETTING_TEST1_VAL_TX,
	[TEST0] = SMARTRF_SETTING_TEST0_VAL_TX,
	[IOCFG2] = SMARTRF_SETTING_IOCFG2_VAL_TX,
	[IOCFG1] = SMARTRF_SETTING_IOCFG1_VAL_TX,
	[IOCFG0] = SMARTRF_SETTING_IOCFG0_VAL_TX,
	[PKTCTRL0] = SMARTRF_SETTING_PKTCTRL0_VAL_TX,
	[PKTLEN] = SMARTRF_SETTING_PKTLEN_VAL_TX,
	[FIFOTHR] = SMARTRF_SETTING_FIFOTHR_VAL_TX,
	[WORCTRL] = SMARTRF_SETTING_WORCTRL_VAL_TX
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
 * Status Register Masks (section 29.3, page 94).
 *
 * Use these masks to extract relevant fields when reading from certain status registers above.
 */
#define TXFIFO_UNDERFLOW	(0b10000000) // TXBYTES Bits 7    Indicates if TX FIFO has underflowed.
#define NUM_TXBYTES			(0b01111111) // TXBYTES Bits 6:0  Number of bytes in TX FIFO.
#define RXFIFO_OVERFLOW		(0b10000000) // RXBYTES Bits 7    Indicates if RX FIFO has overflowed.
#define NUM_RXBYTES			(0b01111111) // RXBYTES Bits 6:0  Number of bytes in RX FIFO.

/**
 * FIFO Buffers (section 10.2, page 32).
 *
 * FIFO_TX is write-only.
 * FIFO_RX is read-only.
 * Both are accessed through the 0x3F address with appropriate read/write bits set.
 */
#define PA_TABLE_ADDR (0x3E)
#define PA_TABLE_SETTING (0x60)
#define FIFO_TX (0x3F)
#define FIFO_RX (0x3F)

#define FIFO_LENGTH (64)

static spiDAT1_t spiDataConfig;

QueueHandle_t xRadioTXQueue;
QueueHandle_t xRadioRXQueue;
QueueHandle_t xRadioCHIMEQueue;

static uint8 statusByte;

typedef struct RadioDAT {
	uint8 srcsz;
	uint8 srcdat[100];
} RadioDAT_t;


/**
 * Forward declarations
 */
static uint8 readRegister(uint8 addr);
static int readFromRxFIFO(uint8 *dest, uint8 size);
static void strobe(uint8 addr);

//Declarations for RF Interrupt
SemaphoreHandle_t gioRFSem;
TaskHandle_t xRFInterruptTaskHandle;

void vRadioTask(void *pvParameters) {
	xRadioTXQueue = xQueueCreate(10, sizeof(portCHAR *));
	xRadioRXQueue = xQueueCreate(10, sizeof(portCHAR));

	initRadio();

	strobe(SRX);

	while (1) {
		serialSendln("radio task");

		char buffer[30];

		//uint8 test[] = {0, 3, 9, 27, 14, 15, 16, 17, 18, 19};
		uint8 packetLen = 3;
		uint8 test[100] = { 0 };
		int i = 2;
		test[0] = packetLen;
		test[1] = 0x10;
		while (i < packetLen) {
			test[i] = i;
			i++;
		}

		uint8 txbytes = readRegister(TXBYTES);
		uint8 tx_underflowed = txbytes & TXFIFO_UNDERFLOW;
		uint8 tx_numbytes = txbytes & NUM_TXBYTES;
		snprintf(buffer, 30, "tx_underflowed:%u tx_numbytes:%u", tx_underflowed, tx_numbytes);
		serialSendln(buffer);

		strobe(SNOP);
		serialSend("TX FIFO_BYTES_AVAILABLE: ");
		snprintf(buffer, 30, "0x%x 0d%d", statusByte & FIFO_BYTES_AVAILABLE, statusByte & FIFO_BYTES_AVAILABLE);
		serialSendln(buffer);

		strobe(SNOP | READ_BIT);
		serialSend("RX FIFO_BYTES_AVAILABLE: ");
	    snprintf(buffer, 30, "0x%x 0d%d", statusByte & FIFO_BYTES_AVAILABLE, statusByte & FIFO_BYTES_AVAILABLE);
	    serialSendln(buffer);

		if (1) { //1 for tx
			/**
			 * Strobe a NOP to ensure last operation was a write.
			 * Then statusByte will be primed with FIFO_BYTES_AVAILABLE for TX FIFO.
			 */
			strobe(SNOP);
			if (writeToTxFIFO(test, packetLen)) {
				snprintf(buffer, 30, "%d Bytes Radio TX FIFO written", packetLen);
				serialSendln(buffer);
			} else {
				snprintf(buffer, 30, "Radio did not write");
				serialSendln(buffer);
			}

			strobe(STX);
		}

	    //TODO: use timer and return timeout error if radio never returns to IDLE, this should be done for most state transitions
	    if(readRegister(RXBYTES) != 0){
	    	readFromRxFIFO(test, 5);
	    	snprintf(buffer, 30, "RX: %02x %02x %02x %02x %02x", test[0], test[1], test[2], test[3], test[4]);
	    	serialSendln(buffer);
	    }


		vTaskDelay(pdMS_TO_TICKS(5000)); // do we need this?
	}
}

// TX task
void vRadioTX(void *pvParameters) {
	RadioDAT_t Radio_container;
	Radio_container.srcsz = 100;
	uint8 idx = 0;
	for (idx = 0; idx < Radio_container.srcsz; idx++){
		Radio_container.srcdat[idx] = idx;
	}

	xRadioTXQueue = xQueueCreate(10, sizeof(portCHAR *));
	//initialize radio herehello



	if (!writeToTxFIFO(Radio_container.srcdat, Radio_container.srcsz)){
		//error
	}


	//writeToTxFIFO(*(Radio_container*)pvParameters.srcdat, *(Radio_container*)pvParameters.srcsz);


	// how does pvparameters work?

	uint8 txsize = 10;
	uint8 txsrc[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	writeToTxFIFO(txsrc, txsize);
	strobe(STX);
	//TODO: check last packet was transmitted

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

/**
 * Writes values from src buffer into TX FIFO (section 10.5, page 32).
 *
 * Assumes last operation was a write (header has R/W bit set to 0); that FIFO_BYTES_AVAILABLE contains number of bytes free in TX FIFO.
 * Currently only supports complete writes (when size of data to send is <= to numBytesInFIFO).
 * TODO: Support partial writes (write what we can first, eg, what's sent is = numBytesInFIFO, then send the rest of it later).
 *
 * @param src Data buffer to send
 * @param size Size of src (number of bytes)
 * @return 0 on failure, 1 on successful write, 2 on unexpected size
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
		writeRegister(FIFO_TX, src[idx++]); //FIFO_TX single byte access
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

static void writeAllConfigRegisters(const uint16 config[NUM_CONFIG_REGISTERS]) {
	uint8 i = 0;
	while (i < NUM_CONFIG_REGISTERS) {
		char buffer[30];
		//snprintf(buffer, "addr %02x, config %02x,", SMARTRF_ADDRS[i], config[i]);
		writeRegister(SMARTRF_ADDRS[i], config[i]); //why is this only VAL_RX?


		//snprintf(buffer, 30, "new reg content = %02x", readRegister(SMARTRF_ADDRS[i++]));
		//serialSendln(buffer);
		i++;
	}
}

static void readAllConfigRegisters() {
	uint8 i = 0;
	while (i < NUM_CONFIG_REGISTERS) {
		readRegister(SMARTRF_ADDRS[i++]);
	}
}

static uint8 * readAllStatusRegisters() {
	static uint8 contents[14];
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

static int checkConfig(const uint16 config[NUM_CONFIG_REGISTERS]) {
	uint8 i = 0;
	while (i < NUM_CONFIG_REGISTERS) {
		uint8 regVal = readRegister(SMARTRF_ADDRS[i]);
		if(config[i] != regVal){
			char buffer[30];
			snprintf(buffer, 30, "Reg %02x = %02x != %02x", SMARTRF_ADDRS[i], regVal, config[i]);
			serialSendln(buffer);
			return 1;
		}
		i++;
	}
	return 0;
}

static int configureRadio(const uint16 config[NUM_CONFIG_REGISTERS], const uint8 PA_TABLE) {
    writeAllConfigRegisters(SMARTRF_VALS_TX);
    writeRegister(PA_TABLE_ADDR, PA_TABLE);
    return checkConfig(SMARTRF_VALS_TX);
}

static void switchRadioMode(){
	//
	serialSendQ("Switch here");
}


void vRFInterruptTask(void *pvParameters){
	// This task gets invoked by the ISR callback (notification) whenever the pin sees a rising edge

	//pulled from example
	while(1){
		xSemaphoreTake(gioRFSem, portMAX_DELAY);
		serialSendQ("It's an RF one!");
		//gioSetBit(DEBUG_LED_PORT, DEBUG_LED_PIN, gioGetBit(DEBUG_LED_PORT, DEBUG_LED_PIN) ^ 1);   // Toggles the blinky LED
		switchRadioMode();
	}
}

void rf_interrupt_init(void){
	//pulled from example
	gioRFSem = xSemaphoreCreateBinary();
	xRFInterruptTaskHandle = NULL;

	if(gioRFSem != NULL){ // setup the task to handle the ISR
		xTaskCreate(vRFInterruptTask, "RF Interrupt", 200, NULL, 3, xRFInterruptTaskHandle);
	}

	gioEnableNotification(RF_IRQ_PORT, RF_IRQ_PIN); // enable the notification callback for this particular pin
}

void gio_notification_RF(gioPORT_t *port, uint32 bit){
	// This gets called by the ISR callback (notification). Doesn't need to be in another function, but this just keeps things tidy
	// See FreeRTOS tutorial guide pg. 200

	//pulled from Example

	BaseType_t xHigherPriorityTaskWoken;
	xHigherPriorityTaskWoken = pdFALSE;

	if(port == GIO_IRQ_PORT && bit == GIO_IRQ_PIN){ // Always need to check which pin actually triggered the interrupt

		xSemaphoreGiveFromISR(gioRFSem, &xHigherPriorityTaskWoken);
		portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
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
    spiDataConfig.CSNR = RF_CONFIG_CSNR;

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
