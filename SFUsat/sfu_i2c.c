/*
 * sfu_i2c.c
 *
 *  Created on: Apr 22, 2018
 *      Author: Richard
 *
 *      I2C driver functions that implement some error checking. Application can optionally attempt
 *      to retry upon return of an error code. However, applications must only attempt a few times
 *      before returning so that the bus gets freed up.
 *
 *      See stlm75.c for examples of how to use these functions, as there are several things outside
 *      the scope of these functions that you need to do.
 *
 *      USEFUL RESOURCES:
 *      - http://processors.wiki.ti.com/index.php/I2C_Tips (this one is great)
 */

#include "i2c.h"
#include "sfu_i2c.h"
#include "FreeRTOS.h"
#include "rtos_semphr.h"
#include "sfu_utils.h"
#include "rtos_task.h"
#include "sfu_uart.h"
#include "sfu_task_logging.h"

SemaphoreHandle_t xI2CMutex;
static uint8_t num_resets;

void sfu_i2c_init() {
	i2cInit();
	num_resets = 0;
	xI2CMutex = xSemaphoreCreateMutex();
	if (xI2CMutex != NULL) {
		/* The semaphore was created successfully and
		 can be used. */
	} else {
		sfu_reset_i2c(i2cREG1);
	}
}

int16_t sfu_is_bus_busy() {
	uint32_t timeout = 0;
	while ((I2C->STR & I2C_BUSBUSY) && timeout < I2C_TIMEOUT_MAX) {
		timeout++;
	}
	if (timeout >= I2C_TIMEOUT_MAX)
		return I2C_BUSBUSY_FAIL;
	return I2C_OK;
}

int16_t sfu_ok_transmit() {
	uint32_t timeout = 0;
	while ((I2C->MDR & I2C_MASTER) && timeout < I2C_TIMEOUT_MAX) {// goes low when we're good to transmit again 	[TRM pg. 1498]
		timeout++;
	}
	if (timeout >= I2C_TIMEOUT_MAX)
		return I2C_TXREADY_FAIL;
	return I2C_OK;
}

int16_t sfu_i2cSend(i2cBASE_t *i2c, uint32 length, uint8 * data) {
	uint32_t timeout_count = 0;
	while (length > 0U) {
		while (((i2c->STR & (uint32) I2C_TX_INT | (uint32) I2C_ARDY_INT) == 0U) && (timeout_count < I2C_TIMEOUT_MAX)) {
			timeout_count++;
		}

		if (timeout_count >= I2C_TIMEOUT_MAX) {
			return I2C_TIMEOUT_FAIL;
		}

		/* If a NACK occurred, SCL is held low and STP bit cleared: http://processors.wiki.ti.com/index.php/I2C_Tips */
		if (i2c->STR & (uint32_t) I2C_NACK_INT) {
			i2cSetStop(i2cREG1);
			i2c->STR = I2C_NACKSNT; // write 1 to clear
			return I2C_ERR_NACK;
		}

		i2c->DXR = (uint32) (*data);
		data++;
		length--;
	}
// these don't work here
//	while (I2C->STR & I2C_BUSBUSY);	// goes low when stop has gone through			[TRM pg. 1492]
//	while (I2C->MDR & I2C_MASTER);	// goes low when we're good to transmit again 	[TRM pg. 1498]
	return I2C_OK;
}

int16_t sfu_i2cReceive(i2cBASE_t *i2c, uint32 length, uint8 * data) {
	uint32_t timeout_count;
	timeout_count = 0;

	// maybe do ARDY up here

	while (length > 0U) {
		// It doesn't seem to like a check for ARDY
		while (((i2c->STR & (uint32) I2C_RX_INT) == 0U) && (timeout_count < I2C_TIMEOUT_MAX)) {
			timeout_count++; // typical value seems to be < 3500
		}
		if (timeout_count >= I2C_TIMEOUT_MAX) {
			return I2C_TIMEOUT_FAIL;
		}

		// If a NACK occurred, SCL is held low and STP bit cleared: http://processors.wiki.ti.com/index.php/I2C_Tips
		if (i2c->STR & (uint32_t) I2C_NACK_INT) {
			i2cSetStop(i2cREG1);
			i2c->STR = I2C_NACKSNT; // write 1 to clear
			return I2C_ERR_NACK;
		}

		*data = ((uint8) i2c->DRR);
		data++;
		length--;
	}
	return I2C_OK;
}

/* Reset I2C
 *
 * - the bus can hang for many reasons. Our error handlers must therefore be able to reset the bus.
 * - To do this, we reset the I2C peripheral and clock out 10 pulses
 * - http://www.microchip.com/forums/m175368.aspx
 *
 * - Precondition: we should have the mutex
 *
 * - num_resets ensures we only attempt to reset 5 times before triggering the watchdog.
 * - if we reset successfull, num_resets is set to 0 again.
 */
int16_t sfu_reset_i2c(i2cBASE_t *i2c){
	vTaskSuspendAll();							/* don't want any scheduling happening while we reset */
	do {
		i2c->MDR = (uint32)((uint32)0U << 5U); 	/* reset i2c peripheral */
		i2c->PFNC = (0x01);						/* I2C pins to GPIO mode */
		i2c->DIR = (0x01);						/* SDA is input [bit 1], SCL is output [bit 0], output = 1 */
		uint8_t i;

		/* send out some pulses */
		for(i = 0; i < 10; i++){
			i2c->DOUT = i2c->DOUT | 0x01; 		/* set SCL high */
			busyWait(300);
			i2c->DOUT ^= i2c->DOUT; 			/* set SCL low */
			busyWait(300);
		}
		num_resets++;

	} while (i2c->DIN & 0x02 != 0x02 && num_resets < 5);	/* check that the data line has gone high (idle) */

	if(num_resets >= 5) {
		serialSendln("I2C FAULT");
		while(1);								/* at this point, the watchdog will take care of us */
	}

    serialSendln("I2C RESET");

	sfu_i2c_init();
	xTaskResumeAll();
	addLogItem(logtype_i2c, error_1);
    return I2C_OK;
}


int16_t BMS_i2c_send(i2cBASE_t *i2c, uint32 length, uint8 * data){

	uint32_t timeout_count;
	timeout_count = 0;

    if ((g_i2cTransfer_t.mode & (uint32)I2C_TX_INT) != 0U)
    {
        /* Interrupt mode */
        /*SAFETYMCUSW 45 D MR:21.1 <APPROVED> "Valid non NULL input parameters are only allowed in this driver" */
        g_i2cTransfer_t.data   = data;
        /* start transmit by sending first byte */
        /*SAFETYMCUSW 45 D MR:21.1 <APPROVED> "Valid non NULL input parameters are only allowed in this driver" */
        i2c->DXR = (uint32)(*g_i2cTransfer_t.data);
        /*SAFETYMCUSW 45 D MR:21.1 <APPROVED> "Valid non NULL input parameters are only allowed in this driver" */
        /*SAFETYMCUSW 567 S MR:17.1,17.4 <APPROVED> "Pointer increment needed" */
        g_i2cTransfer_t.data++;

        /* Length -1 since one data is written already */
        g_i2cTransfer_t.length = (length - 1U);

        /* Enable Transmit Interrupt */
        i2c->IMR |= (uint32)I2C_TX_INT;
    }
    else
    {
        /* send the data */
        while (length > 0U)
        {
            /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Potentially infinite loop found - Hardware Status check for execution sequence" */
            while (((i2c->STR & (uint32)I2C_TX_INT) == 0U) && timeout_count < I2C_TIMEOUT_MAX)
            {
            	timeout_count++;
            } /* Wait */
            if(timeout_count >= I2C_TIMEOUT_MAX) {
            	return I2C_TIMEOUT_FAIL;
            }
            /*SAFETYMCUSW 45 D MR:21.1 <APPROVED> "Valid non NULL input parameters are only allowed in this driver" */
            i2c->DXR = (uint32)(*data);
            /*SAFETYMCUSW 45 D MR:21.1 <APPROVED> "Valid non NULL input parameters are only allowed in this driver" */
            /*SAFETYMCUSW 567 S MR:17.1,17.4 <APPROVED> "Pointer increment needed" */
            data++;
            length--;
        }
    }
    return I2C_OK;
}
int16_t BMS_i2c_Receive(i2cBASE_t *i2c, uint32 length, uint8 * data){
	uint32_t timeout_count;
	timeout_count = 0;

    if ((i2c->IMR & (uint32)I2C_RX_INT) != 0U)
    {
        /* we are in interrupt mode */
        /* clear error flags */
        i2c->STR = (uint32)I2C_AL_INT | (uint32)I2C_NACK_INT;

        g_i2cTransfer_t.length = length;
        /*SAFETYMCUSW 45 D MR:21.1 <APPROVED> "Valid non NULL input parameters are only allowed in this driver" */
        g_i2cTransfer_t.data   = data;
    }
    else
    {
        while (length > 0U)
        {
            /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Potentially infinite loop found - Hardware Status check for execution sequence" */
            while (((i2c->STR & (uint32)I2C_RX_INT) == 0U) && timeout_count < I2C_TIMEOUT_MAX)
            {
            	timeout_count++;
            } /* Wait */
            if(timeout_count >= I2C_TIMEOUT_MAX)
            {
            	return I2C_TIMEOUT_FAIL;
            }
            /*SAFETYMCUSW 45 D MR:21.1 <APPROVED> "Valid non NULL input parameters are only allowed in this driver" */
            *data = ((uint8)i2c->DRR);
            /*SAFETYMCUSW 45 D MR:21.1 <APPROVED> "Valid non NULL input parameters are only allowed in this driver" */
            /*SAFETYMCUSW 567 S MR:17.1,17.4 <APPROVED> "Pointer increment needed" */
            data++;
            length--;
        }
    }
    return I2C_OK;
}

