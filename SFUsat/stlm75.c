/*
 * stlm75.c
 *
 *  Created on: Apr 18, 2018
 *      Author: Richard
 *
 *      Driver for STLM75 temperature sensor
 *
 */
#include "i2c.h"
#include "stlm75.h"
#include "reg_i2c.h"

/* sanity check that the OBC temp sensor returns something */
uint16_t obc_temp_test(){
	uint16_t temp;
	temp = 0x00;
	temp = read_temp(OBC_TEMP);
	temp = temp - 1;
	return temp;
	// convert and stuff bleh
}

/* Read temperature
 * - from page 24 of datasheet:
 * - Send out: [address 7:1][0], [0] means read
 * - STLM 75 will return two bytes, left aligned ugh
 * - details about scaling were taken from here: https://github.com/jfitter/LM75/blob/master/LM75.cpp
 */
uint16_t read_temp(uint8_t addr){
	uint8_t data[2] = { '\0' };
	int16_t temp_deg_c;
	uint8_t cmd = (addr << 1 | RD_CMD);

	i2cSetSlaveAdd(i2cREG1, OBC_TEMP);
	i2cSetDirection(i2cREG1, I2C_TRANSMITTER);
	i2cSetCount(i2cREG1, 3); 							// the total number of bytes to transact before sending stop bit
	i2cSetMode(i2cREG1, I2C_MASTER);
	i2cSetStop(i2cREG1);
	i2cSetStart(i2cREG1);
	i2cSend(i2cREG1, 1, &cmd);
//	while(i2cIsBusBusy(i2cREG1) == true);

	i2cSetSlaveAdd(i2cREG1, OBC_TEMP);
	i2cSetDirection(i2cREG1, I2C_RECEIVER);
	i2cSetMode(i2cREG1, I2C_MASTER);
	i2cReceive(i2cREG1, 2, data);
	i2cSetStop(i2cREG1);

	temp_deg_c = data[0] << 8; 							// MSBits
	temp_deg_c = temp_deg_c | data[1]; 					// LSBits
	temp_deg_c = ((temp_deg_c & 0xFF10) >> 5) * 0.125; // mask off in case there's garbage, shift over so we're correctly right-aligned, scale due to 0.125ºC resolution
	return temp_deg_c;
}

void hcg_test(){
	int repeat = 0; int delay =0;
	uint8_t TX_Data_Master[10] = { 0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19};

	i2cSetSlaveAdd(i2cREG1, OBC_TEMP);
	i2cSetDirection(i2cREG1, I2C_TRANSMITTER);
	for(repeat = 0; repeat < 2; repeat++)
		{
			/* Configure Data count */
			/* Note: Optional - It is done in Init, unless user want to change */
			i2cSetCount(i2cREG1, DATA_COUNT);
			i2cSetMode(i2cREG1, I2C_MASTER);
			i2cSetStop(i2cREG1);
			i2cSetStart(i2cREG1);
			i2cSend(i2cREG1, DATA_COUNT, TX_Data_Master);
			while(i2cIsBusBusy(i2cREG1) == true);
			while(i2cIsStopDetected(i2cREG1) == 0);
			i2cClearSCD(i2cREG1);

			/* Simple Dealya before starting Next Block */
			/* Depends on how quick the Slave gets ready */
			for(delay=0;delay<1000000;delay++);

		}

//		///////////////////////////////////////////////////////////////
//		//        Master Receive Functionality                      //
//		///////////////////////////////////////////////////////////////
////
//		i2cSetSlaveAdd(i2cREG1, Slave_Address);
//i2cSetDirection(i2cREG1, I2C_RECEIVER);
//			i2cSetCount(i2cREG1, DATA_COUNT);
//			i2cSetMode(i2cREG1, I2C_MASTER);
//			i2cSetStop(i2cREG1);
//			i2cSetStart(i2cREG1);
//			i2cReceive(i2cREG1, DATA_COUNT, RX_Data_Master);
//			while(i2cIsBusBusy(i2cREG1) == true);
//			while(i2cIsStopDetected(i2cREG1) == 0);
//			i2cClearSCD(i2cREG1);
//
//			/* Simple Dealya before starting Next Block */
//			/* Depends on how quick the Slave gets ready */
//			for(delay=0;delay<1000000;delay++);
//
//		}
//
}


//void I2cDriver_init(void)
//{
//   i2cInit();
//}


void I2cDriver_write(uint8_t slaveDeviceAddress, uint32_t writeAddress, uint8_t const * writeBuffer, uint32_t writeLength)
{
   // Something to do?
   if (writeLength > 0)
   {
//      assert(writeBuffer);

      // Wait until the slave device is not busy
      waitWhileDeviceIsBusy(slaveDeviceAddress);

      // Disable I2C during configuration
      I2C->MDR = 0;

      // Configure the I2C controller
      I2C->CNT = writeLength + 1; // + 1 bytes for the internal address
      I2C->SAR = (uint32_t)slaveDeviceAddress >> 1;
      I2C->MDR = I2C_RESET_OUT | I2C_START_COND | I2C_STOP_COND | I2C_TRANSMITTER | I2C_MASTER;

      // Send the internal address
      if (sendByte((uint8_t)writeAddress) == FALSE)
      {
         return;
      }

      // Transmit the data
      while (writeLength)
      {
         writeLength--;
         if (sendByte(*writeBuffer++) == FALSE)
         {
            return;
         }
      }

      // Wait until the bus isn't busy and the master mode bit is cleared
      while (I2C->STR & I2C_BUSBUSY);
      while (I2C->MDR & I2C_MASTER);
   }
}

void I2cDriver_read(uint8_t slaveDeviceAddress, uint32_t readAddress, uint8_t * readBuffer, uint32_t readLength)
{
   // Something to do?
   if (readLength > 0)
   {
//      assert(readBuffer);

      // Wait until the slave device is not busy
      waitWhileDeviceIsBusy(slaveDeviceAddress);

      // Disable I2C during configuration
      I2C->MDR = 0;

      // Configure the I2C controller as transmitter
      I2C->CNT = 1; // 1 byte for the internal address
      I2C->SAR = (uint32_t)slaveDeviceAddress >> 1;
      I2C->MDR = I2C_RESET_OUT | I2C_START_COND | I2C_TRANSMITTER | I2C_MASTER;

      // Send the internal address
      if (sendByte((uint8_t)readAddress) == FALSE)
      {
         return;
      }

      // Wait for ARDY before beginning the read phase
      while ((I2C->STR & I2C_ARDY_INT) == 0);

      // Configure the I2C controller as receiver
      I2C->CNT = readLength;
      I2C->MDR = I2C_RESET_OUT | I2C_START_COND | I2C_STOP_COND | I2C_MASTER;

      // Receive the data
      while (readLength)
      {
         readLength--;
         if (receiveByte(readBuffer++) == FALSE)
         {
            return;
         }
      }

      // Wait until the bus isn't busy and the master mode bit is cleared
      while (I2C->STR & I2C_BUSBUSY);
      while (I2C->MDR & I2C_MASTER);
   }
}

// Function which has to be implemented for the generated code from HALCoGen
void i2cNotification(i2cBASE_t *i2c, uint32_t flags)
{
}

static void waitWhileDeviceIsBusy(uint8_t slaveDeviceAddress)
{
   // Wait until the bus is not busy
   while (I2C->STR & I2C_BUSBUSY);

   // Disable I2C during configuration
   I2C->MDR = 0;

   // Configure the I2C controller as transmitter in repeat mode
   I2C->SAR = (uint32_t)slaveDeviceAddress >> 1;
   I2C->MDR = I2C_RESET_OUT | I2C_TRANSMITTER | I2C_REPEATMODE;

   // Wait until the slave device acknowledges its address
   while (TRUE)
   {
      // Set the START condition
      I2C->MDR |= I2C_START_COND | I2C_MASTER;

      // Wait for the ARDY flag
      while ((I2C->STR & I2C_ARDY_INT) == 0);

      // Set the STOP condition
      I2C->MDR |= I2C_STOP_COND;

      // Wait until the bus isn't busy and the master mode bit is cleared
      while (I2C->STR & I2C_BUSBUSY);
      while (I2C->MDR & I2C_MASTER);

      // Check if the slave address is acknowledged
      if ((I2C->STR & I2C_NACK_INT) == 0)
      {
         // Slave address ACKed; the slave device is ready again
         return;
      }
      else
      {
         // Slave address NACKed (clear the NACK bit)
         I2C->STR |= I2C_NACK_INT;
      }
   }
}

static bool sendByte(uint8_t byte)
{
   // Wait for the TXRDY flag to transmit data or ARDY if we get NACKed
   while ((I2C->STR & (I2C_TX_INT | I2C_ARDY_INT)) == 0);

   // If a NACK occurred then SCL is held low and STP bit cleared
   if (I2C->STR & I2C_NACK_INT)
   {
      // Reset the I2C
      I2C->MDR = 0;
      return FALSE;
   }

   i2cREG1->DXR = byte;
   return TRUE;
}

static bool receiveByte(uint8_t * byte)
{
   // Wait for the RXRDY flag to transmit data or ARDY if we get NACKed
   while ((I2C->STR & (I2C_RX_INT | I2C_ARDY_INT)) == 0);

   // If a NACK occurred then SCL is held low and STP bit cleared
   if (I2C->STR & I2C_NACK_INT)
   {
      // Reset the I2C
      I2C->MDR = 0;
      return 0;
   }

   // Make sure that the RXRDY flag is set
   while ((I2C->STR & I2C_RX_INT) == 0);

   *byte = (uint8_t)I2C->DRR;
   return 1;
}






void sfu_i2cInit(void)
{
/* USER CODE BEGIN (3) */
/* USER CODE END */
    uint32_t bit_count = I2C_8_BIT;


    /** @b intialize @b I2C */

    /** - i2c out of reset */
    i2cREG1->MDR = (1 << 5);

    /** - set i2c mode */
    i2cREG1->MDR = /* nack mode                         */   (0 << 15)
                   /* free running                      */ | (0 << 14)
                   /* start condtion - master mode only */ |  0
                   /* stop condtion                     */ | (1 <<11)
                   /* Master/Slave mode                 */ | (1 <<10)
                   /* Transmitter/receiver              */ | (I2C_TRANSMITTER)
                   /* xpanded address                   */ | (I2C_7BIT_AMODE)
                   /* repeat mode                       */ | (0 << 7)
                   /* digital loopback                  */ | (0 << 6)
                   /* start byte - master only          */ | (0 << 4)
                   /* free data format                  */ | (0)
                   /* bit count                         */ | (bit_count);


    /** - set i2c extended mode */
    i2cREG1->EMDR = (0 << 25);

    /** - set i2c data count */
    i2cREG1->CNT = 1;

    /** - disable all interrupts */
    i2cREG1->IMR = 0x00U;

    /** - set prescale */
    i2cREG1->PSC = 9;

    /** - set clock rate */
    i2cREG1->CKH = 370;
    i2cREG1->CKL = 370;

    /** - set i2c pins functional mode */
    i2cREG1->PFNC = (1 );

    /** - set i2c pins default output value */
    i2cREG1->DOUT = (0 << 1) /* sda pin */
                  | (0);     /* scl pin */

    /** - set i2c pins output direction */
    i2cREG1->DIR = (0 << 1)   /* sda pin */
                 | (0);       /* scl pin */

    /** - set i2c pins open drain enable */
    i2cREG1->PDR = (0 << 1)   /* sda pin */
                 | (0);       /* scl pin */

    /** - set i2c pins pullup/pulldown enable */
    i2cREG1->PDIS = (0 << 1)  /* sda pin */
                | (0);      /* scl pin */

    /** - set i2c pins pullup/pulldown select */
    i2cREG1->PSEL = (1 << 1)    /* sda pin */
                 | (1);        /* scl pin */

    /** - set interrupt enable */
    i2cREG1->IMR    = (0 << 6)           /* Address as slave interrupt      */
                    | (0 << 5)           /* Stop Condition detect interrupt */
                    | (0 << 4)  /* Transmit data ready interrupt   */
                    | (0 << 3)  /* Receive data ready interrupt    */
                    | (0 << 2)    /* Register Access ready interrupt */
                    | (0 << 1)    /* No Acknowledgement interrupt    */
                    | (0);          /* Arbitration Lost interrupt      */

    i2cREG1->MDR |= I2C_RESET_OUT; /* i2c out of reset */

    /** - inialise global transfer variables */
    //g_i2cTransfer[1].mode   = 0 << 8;
    //g_i2cTransfer[1].length = 0;

/* USER CODE BEGIN (4) */
    /** - Reset I2C */
    i2cREG1->MDR = 0;

    /** - Set I2C mode */
    i2cREG1->MDR = /* nack mode                          */   (0)
                   /* free running                       */ | (0)
                   /* start condition - master mode only */ | (0)
                   /* stop condition                     */ | (I2C_STOP_COND)
                   /* Master/Slave mode                  */ | (I2C_MASTER)
                   /* Transmitter/receiver               */ | (I2C_TRANSMITTER)
                   /* expanded address                   */ | (I2C_7BIT_AMODE)
                   /* repeat mode                        */ | (0)
                   /* digital loopback                   */ | (0)
                   /* start byte - master only           */ | (0)
                   /* free data format                   */ | (0)
                   /* bit count                          */ | (0);

    /** - set i2c pins functional mode */
    i2cREG1->PFNC = (0 );

    /** - I2C out of reset */
    i2cREG1->MDR |= I2C_RESET_OUT;

    /** - initialize global transfer variables */
//    g_i2cTransfer_x.mode   = 0 << 8;
//    g_i2cTransfer_x.length = 0;
//    g_i2cTransfer_x.data   = NULL;
    //g_i2cTransfer[1].mode   = 0 << 8;
    //g_i2cTransfer[1].length = 0;
    //g_i2cTransfer[1].data   = NULL;

    global_struct_init();
/* USER CODE END */

}
