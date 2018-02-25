/** @file reg_pinmux.h
*   @brief PINMUX Register Layer Header File
*   @date 07-July-2017
*   @version 04.07.00
*   
*   This file contains:
*   - Definitions
*   - Types
*   - Interface Prototypes
*   .
*   which are relevant for the PINMUX driver.
*/

/* 
* Copyright (C) 2009-2016 Texas Instruments Incorporated - www.ti.com  
* 
* 
*  Redistribution and use in source and binary forms, with or without 
*  modification, are permitted provided that the following conditions 
*  are met:
*
*    Redistributions of source code must retain the above copyright 
*    notice, this list of conditions and the following disclaimer.
*
*    Redistributions in binary form must reproduce the above copyright
*    notice, this list of conditions and the following disclaimer in the 
*    documentation and/or other materials provided with the   
*    distribution.
*
*    Neither the name of Texas Instruments Incorporated nor the names of
*    its contributors may be used to endorse or promote products derived
*    from this software without specific prior written permission.
*
*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
*  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
*  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
*  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
*  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
*  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
*  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
*  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
*  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
*  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
*  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
*/


#ifndef __REG_PINMUX_H__
#define __REG_PINMUX_H__

#include "sys_common.h"


/* USER CODE BEGIN (0) */
/* USER CODE END */

/* IOMM Revision and Boot Register */
#define REVISION_REG		(*(volatile uint32 *)0xFFFFEA00U)
#define ENDIAN_REG		(*(volatile uint32 *)0xFFFFEA20U)

/* IOMM Error and Fault Registers */
/** @struct iommErrFault
*   @brief IOMM Error and Fault Register Definition
*
*   This structure is used to access the IOMM Error and Fault registers.
*/
typedef volatile struct iommErrFault
{
    uint32 ERR_RAW_STATUS_REG;          /* Error Raw Status / Set Register */
    uint32 ERR_ENABLED_STATUS_REG;      /* Error Enabled Status / Clear Register */
    uint32 ERR_ENABLE_REG;              /* Error Signaling Enable Register */
    uint32 ERR_ENABLE_CLR_REG;          /* Error Signaling Enable Clear Register */
	uint32  rsvd;                       /* Reserved */
    uint32 FAULT_ADDRESS_REG;           /* Fault Address Register */
    uint32 FAULT_STATUS_REG;            /* Fault Status Register */
    uint32 FAULT_CLEAR_REG;             /* Fault Clear Register */	
} iommErrFault_t;
/* Pinmux Register Frame Definition */
/** @struct pinMuxKicker
*   @brief Pin Muxing Kicker Register Definition
*
*   This structure is used to access the Pin Muxing Kicker registers.
*/
typedef volatile struct pinMuxKicker
{
    uint32 KICKER0;       /* kicker 0 register */
    uint32 KICKER1;       /* kicker 1 register */
} pinMuxKICKER_t;

/** @struct pinMuxBase
*   @brief PINMUX Register Definition
*
*   This structure is used to access the PINMUX module registers.
*/
/** @typedef pinMuxBASE_t
*   @brief PINMUX Register Frame Type Definition
*
*   This type is used to access the PINMUX Registers.
*/
typedef volatile struct pinMuxBase
{
    uint32 PINMMR0;		/**< 0xEB10 Pin Mux 0 register*/
    uint32 PINMMR1;		/**< 0xEB14 Pin Mux 1 register*/
    uint32 PINMMR2;		/**< 0xEB18 Pin Mux 2 register*/
    uint32 PINMMR3;		/**< 0xEB1C Pin Mux 3 register*/
    uint32 PINMMR4;		/**< 0xEB20 Pin Mux 4 register*/
    uint32 PINMMR5;		/**< 0xEB24 Pin Mux 5 register*/
    uint32 PINMMR6;		/**< 0xEB28 Pin Mux 6 register*/
    uint32 PINMMR7;		/**< 0xEB2C Pin Mux 7 register*/
}pinMuxBASE_t;


/** @def iommErrFaultReg
*       @brief IOMM Error Fault Register Frame Pointer
*
*               This pointer is used to control IOMM Error and Fault across the device.
*/      
#define iommErrFaultReg ((iommErrFault_t *) 0xFFFFEAE0U)
        
/** @def kickerReg
*	@brief Pin Muxing Kicker Register Frame Pointer
*
*		This pointer is used to enable and disable muxing across the device.
*/	
#define kickerReg ((pinMuxKICKER_t *) 0xFFFFEA38U)

/** @def pinMuxReg
*	@brief Pin Muxing Control Register Frame Pointer
*
*		This pointer is used to set the muxing registers across the device.
*/	
#define pinMuxReg ((pinMuxBASE_t *) 0xFFFFEB10U)

/* USER CODE BEGIN (1) */
/* USER CODE END */


#endif
