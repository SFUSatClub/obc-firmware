/*
 * I2C_Protocol.c
 *
 *  Created on: Jun 25, 2018
 *      Author: Derrick
 *
 *      //Referenced with http://e2e.ti.com/support/microcontrollers/hercules/f/312/t/163123#pi320098=3
 */

//#include "i2c.h"
#include "sfu_i2c.h"
#include "I2C_Protocol.h"

void I2cDriver_init(void)
{
   i2cInit_1();
}

void I2cDriver_write(uint8 slaveDeviceAddress, uint32 writeAddress, uint8 const * writeBuffer, uint32 writeLength)
{
   // Something to do?
   if (writeLength > 0)
   {
      assert(writeBuffer);

      // Wait until the slave device is not busy
      waitWhileDeviceIsBusy(slaveDeviceAddress);

      // Disable I2C during configuration
      I2C->MDR = 0;

      // Configure the I2C controller
      I2C->CNT = writeLength + 1; // + 1 bytes for the internal address
      I2C->SAR = (uint32)slaveDeviceAddress >> 1;
      I2C->MDR = I2C_RESET_OUT | I2C_START_COND | I2C_STOP_COND | I2C_TRANSMITTER | I2C_MASTER;

      // Send the internal address
      if (sendByte((uint8)writeAddress) == FALSE)
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

void I2cDriver_read(uint8 slaveDeviceAddress, uint32 readAddress, uint8 * readBuffer, uint32 readLength)
{
   // Something to do?
   if (readLength > 0)
   {
      assert(readBuffer);

      // Wait until the slave device is not busy
      waitWhileDeviceIsBusy(slaveDeviceAddress);

      // Disable I2C during configuration
      I2C->MDR = (uint32)((uint32)0U << 5U);

      // Configure the I2C controller as transmitter
      I2C->CNT = 1; // 1 byte for the internal address
      I2C->SAR = (uint32)slaveDeviceAddress >> 1;
      I2C->MDR = I2C_RESET_OUT | I2C_START_COND | I2C_TRANSMITTER | I2C_MASTER;

      // Send the internal address
      if (sendByte((uint8)readAddress) == FALSE)
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
void i2cNotification(i2cBASE_t *i2c, uint32 flags)
{
}

static void waitWhileDeviceIsBusy(uint8 slaveDeviceAddress)
{
   // Wait until the bus is not busy
   while (I2C->STR & I2C_BUSBUSY);

   // Disable I2C during configuration
   I2C->MDR = 0;

   // Configure the I2C controller as transmitter in repeat mode
   I2C->SAR = (uint32)slaveDeviceAddress >> 1;
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

static boolean sendByte(uint8 byte)
{
   // Wait for the TXRDY flag to transmit data or ARDY if we get NACKed
   while ((I2C->STR & (I2C_TX_INT | I2C_ARDY_INT)) == 0);

   // If a NACK occurred then SCL is held low and STP bit cleared
   if (I2C->STR & I2C_NACK_INT)
   {
      // Reset the I2C
      I2C->MDR = (uint32)((uint32)0U << 5U);
      return FALSE;
   }

   i2cREG1->DXR = byte;
   return TRUE;
}

static boolean receiveByte(uint8 * byte)
{
   // Wait for the RXRDY flag to transmit data or ARDY if we get NACKed
   while ((I2C->STR & (I2C_RX_INT | I2C_ARDY_INT)) == 0);

   // If a NACK occurred then SCL is held low and STP bit cleared
   if (I2C->STR & I2C_NACK_INT)
   {
      // Reset the I2C
      I2C->MDR = 0;
      return FALSE;
   }

   // Make sure that the RXRDY flag is set
   while ((I2C->STR & I2C_RX_INT) == 0);

   *byte = (uint8)I2C->DRR;
   return TRUE;
}

//Modified HALCoGen init:

void i2cInit_1(void)
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
    i2cREG1->CKH = 35;
    i2cREG1->CKL = 35;

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
    g_i2cTransfer_t.mode   = 0 << 8;
    g_i2cTransfer_t.length = 0;

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
    g_i2cTransfer_t.mode   = 0 << 8;
    g_i2cTransfer_t.length = 0;
    g_i2cTransfer_t.data   = NULL;
    g_i2cTransfer_t.mode   = 0 << 8;
    g_i2cTransfer_t.length = 0;
    g_i2cTransfer_t.data   = NULL;
/* USER CODE END */

}

