/*
 * obc_sci_dma.c
 *
 *  Created on: Aug 8, 2018
 *      Author: sbork
 */

#include "obc_sci_dma.h"
#include "sys_common.h"
#include "sfu_uart.h"


/** @fn void scidmaSend(char *source_address, short mode)
*   @brief Initialize the SCI and DMA to tranfer SCI data via DMA
*   @note This function configures the SCI to trigger a DMA request when the SCI TX is complete.
*
*   This function configures the DMA in single buffer or multibuffer mode.
*   In single buffer mode (0) the DMA moves each Byte to the SCI tranmit register when request is set.
*   In multi buffer mode (1)  the DMA moves 4 Bytes to the SCI transmit buffer when the request is set.
*/
void scidmaSend(char *source_address)
{
#if ((__little_endian__ == 1) || (__LITTLE_ENDIAN__ == 1))
	uint8 dest_addr_offset=0;		/* 0 for LE */
#else
	uint8 dest_addr_offset=3;		/* 3 for BE */
#endif

	/* Wait for the DMA to complete any existing transfers */
    while(DMA_Comp_Flag != 0x55AAD09E);

    /* Reset the Flag to not Done*/
    DMA_Comp_Flag = ~0x55AAD09E;

	/* - Populate dma control packets structure */
	g_dmaCTRLPKT.SADD       = (uint32)source_address;						/* source address             */

    if (((sciREG->GCR1 >> 10U) & 1U) == 0U) {	 						/* SCI2 Multibuffer mode      */
    	g_dmaCTRLPKT.DADD   = (uint32)(&(sciREG->TD))+dest_addr_offset;	/* In big endianism devices, the destination address needs to be adjusted */
        g_dmaCTRLPKT.RDSIZE = ACCESS_8_BIT;     							/* read size                  */
    	g_dmaCTRLPKT.WRSIZE = ACCESS_8_BIT;     							/* write size                 */
        g_dmaCTRLPKT.FRCNT  = strlen(source_address);    					/* frame count                */

    } else {
    	g_dmaCTRLPKT.DADD      = (uint32)(&(linREG->TDx));	 				/* In big endianism devices, the destination address needs to be adjusted
    	                                                  	  	  	  	  	  	   	* for byte access. The DMA is a big endian master. But the SCI Transmit buffer
    	                                                  	  	  	  	  	     	    * is accessible at the least significant byte.  */
    	g_dmaCTRLPKT.RDSIZE    = ACCESS_32_BIT;   		 					/* read size                  */
    	g_dmaCTRLPKT.WRSIZE    = ACCESS_32_BIT;    		 					/* write size                 */
        g_dmaCTRLPKT.FRCNT     = strlen(source_address)/4+8;	    			/* frame count                */
    }

	/* - setting dma control packets for transmit */
    dmaSetCtrlPacket(DMA_CH0,g_dmaCTRLPKT);

    /* - setting the dma channel to trigger on h/w request */
    dmaSetChEnable(DMA_CH0, DMA_HW);

	/* Enable TX DMA */
    sciREG->SETINT = (1 << 16);

} /* scidmaSend */

/** @fn void scidmaInit(short mode)
*   @brief Initialize the SCI and DMA to tranfer SCI data via DMA
*   @note This function configures the SCI to trigger a DMA request when the SCI TX is complete.
*/
void scidmaInit(sciBASE_t *sci)
{
	/* Enable DMA */
	dmaEnable();

	if (sci == scilinREG) {          			/* SCI2 is the default serial comport on LAUNCHXL2 launch pads*/

		/* Enable Interrupt after reception of data */
		dmaEnableInterrupt(DMA_CH0, BTC);		/* DMA_CH0 is highest priority */

		/* assigning dma request: channel-0 with request line - 1  - TX*/
		/* DMA Request 29 is for LIN ( SCI2) Transmit */
		/* Refer Data sheet - Default DMA Request Map section */
		dmaReqAssign(DMA_CH0,29);

	} else if (sci == sciREG) { 				/* SCI1 */
		//return;					   				/* SCI1 is not supported at this time */

		/* Enable Interrupt after reception of data */
		dmaEnableInterrupt(DMA_CH0, BTC);		/* DMA_CH0 is highest priority */

		/* assigning dma request: channel-0 with request line - 1  - TX*/
		/* DMA Request 31 is for SCI1 Transmit */
		/* Refer Data sheet - Default DMA Request Map section */
		dmaReqAssign(DMA_CH0,31);

	} else {
		return;					 				/* Unknown register */
	}

    /* - Populate dma control packets structure */
	g_dmaCTRLPKT.CHCTRL    = 0;                 /* channel control            */
	g_dmaCTRLPKT.ELCNT     = 1;                 /* element count              */
	g_dmaCTRLPKT.ELDOFFSET = 0;                 /* element destination offset */
	g_dmaCTRLPKT.ELSOFFSET = 0;                 /* element source offset      */
	g_dmaCTRLPKT.FRDOFFSET = 0;                 /* frame destination offset   */
	g_dmaCTRLPKT.FRSOFFSET = 0;                 /* frame source offset        */
	g_dmaCTRLPKT.PORTASGN  = 4;                 /* port b                     */
    g_dmaCTRLPKT.RDSIZE    = ACCESS_8_BIT;      /* read size                  */
	g_dmaCTRLPKT.WRSIZE    = ACCESS_8_BIT;      /* write size                 */
	g_dmaCTRLPKT.TTYPE     = FRAME_TRANSFER ;   /* transfer type              */
	g_dmaCTRLPKT.ADDMODERD = ADDR_INC1;         /* address mode read          */
	g_dmaCTRLPKT.ADDMODEWR = ADDR_FIXED;        /* address mode write         */
    g_dmaCTRLPKT.AUTOINIT  = AUTOINIT_OFF;      /* autoinit                   */

    /* Reset the Flag */
    DMA_Comp_Flag = 0x55AAD09E;

	/* Channel 40 - Enable the VIM channel in HalCoGen to include dmaBTCAInterrupt function */
	vimChannelMap(40, 40, &dmaBTCAInterrupt);

	/* Enable VIM DMA BTCA interrupt to CPU on SCI2 transfer complete */
	vimEnableInterrupt(40, SYS_IRQ);

} /* scidmaInit */

void Update_DMA_Comp_Flag()
{
    /* Set the Flag to Done*/
	DMA_Comp_Flag = 0x55AAD09E;

	/* Disable TX DMA Interrupt */
	sciREG->CLEARINT = (1 << 16);
}

/** @fn void number_string(char *string, uint32 count)
*   @brief Generate null ternimated string of characters starting at zero to count.
*   @note This function pads the end with extra null characters and overrun warning
*/
void number_string(char *string, uint32 count) {
	uint32 i, temp, offset = 0;

	for (i = 0; i < count; i++){
		temp = sprintf (&buffer[i + offset],"%d ", i);
		if (temp != -1U) {
			offset += temp - 1; // increment buffer by the number of characters written minus the null character.
		}
	}

	/* For debug purposes add extra characters to identify overrun */
	sprintf (&buffer[i + offset],"\0\0\0\0\0\0\0\0 !!! Overrun !!! ");
}

/* Preconditions:
 * 		- sciInit has been called
 */
void uart_dma_test(){
    // ---- DMA test
	uint32 IDLECOUNT = 0;
//	sciInit();
	/* Init SCI for DMA transfers */
	scidmaInit(sciREG);

	/* Print header on SCI */
	serialSendln("\033[2J"); // Clear terminal & return home commands
	serialSendln("*******************************************************************************\n\r\n\r");
	serialSendln("scidmaSend Example - DMA to transfer single Bytes from RAM to the SCI\n\r");

	/* Setup null terminated string to transmit */
	number_string((char *) buffer, 500);
    scidmaSend(buffer);

    /* Wait for the DMA interrupt ISR to set the Flag   */
    while(DMA_Comp_Flag != 0x55AAD09E){
    	IDLECOUNT++;
    }

    /* scidmaSend is complete and can be called again   */
    serialSendln("\n\r\n\r");
    serialSendln("scidmaSend Example Complete");
    serialSendln("\n\r\n\r");
}
