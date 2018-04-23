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

int8_t sfu_is_bus_busy(){
	uint32_t timeout = 0;
	while ((I2C->STR & I2C_BUSBUSY) && timeout < I2C_TIMEOUT_MAX){
		timeout++;
	}
	if(timeout >= I2C_TIMEOUT_MAX) return I2C_TIMEOUT_FAIL;
	return I2C_OK;
}

int8_t sfu_i2cSend(i2cBASE_t *i2c, uint32 length, uint8 * data){
	uint32_t timeout_count = 0;
	    while (length > 0U){
			while (((i2c->STR & (uint32)I2C_TX_INT | (uint32)I2C_ARDY_INT) == 0U) && (timeout_count < I2C_TIMEOUT_MAX)){
				timeout_count++;
			}

			if(timeout_count >= I2C_TIMEOUT_MAX){
				return I2C_TIMEOUT_FAIL;
			}

//	            while ((i2c->STR & (uint32)I2C_TX_INT) == 0U)
//	            {
//	            	timeout_count++;
//	            }
	            i2c->DXR = (uint32)(*data);
	            data++;
	            length--;
	        }
//	        while (I2C->STR & I2C_BUSBUSY);	// goes low when stop has gone through			[TRM pg. 1492]
//	        while (I2C->MDR & I2C_MASTER);	// goes low when we're good to transmit again 	[TRM pg. 1498]
	return I2C_OK;
}

int8_t sfu_i2cReceive(i2cBASE_t *i2c, uint32 length, uint8 * data){
	uint32_t timeout_count;
	timeout_count = 0;

	// maybe do ARDY up here

	 while (length > 0U)
	        {
		 	 	 // It doesn't seem to like a check for ARDY
	            while (((i2c->STR & (uint32)I2C_RX_INT) == 0U) && (timeout_count < I2C_TIMEOUT_MAX)){
	            	timeout_count++; // typical value seems to be < 3500
	            }
	    		if(timeout_count >= I2C_TIMEOUT_MAX){
	    			return I2C_TIMEOUT_FAIL;
	    		}

	    		// If a NACK occurred, SCL is held low and STP bit cleared: http://processors.wiki.ti.com/index.php/I2C_Tips
	    		if ( i2c->STR & (uint32_t)I2C_NACK_INT ){
	    			i2cSetStop(i2cREG1);
	    			i2c->STR = I2C_NACKSNT; // write 1 to clear
	    			return I2C_ERR_NACK;
	    		}

	            *data = ((uint8)i2c->DRR);
	            data++;
	            length--;
	        }
    	return I2C_OK;
}
