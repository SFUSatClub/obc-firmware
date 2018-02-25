/** @file pinmux.h
*   @brief PINMUX Driver Implementation File
*   @date 07-July-2017
*   @version 04.07.00
*
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


#ifndef __PINMUX_H__
#define __PINMUX_H__

#include "reg_pinmux.h"



#define PINMUX_PIN_1_SHIFT          8U
#define PINMUX_PIN_2_SHIFT          0U
#define PINMUX_PIN_5_SHIFT          8U
#define PINMUX_PIN_8_SHIFT          16U
#define PINMUX_PIN_9_SHIFT          24U
#define PINMUX_PIN_10_SHIFT         0U
#define PINMUX_PIN_12_SHIFT         8U
#define PINMUX_PIN_18_SHIFT         16U
#define PINMUX_PIN_27_SHIFT         0U
#define PINMUX_PIN_36_SHIFT         16U
#define PINMUX_PIN_37_SHIFT         24U
#define PINMUX_PIN_38_SHIFT         0U
#define PINMUX_PIN_39_SHIFT         8U
#define PINMUX_PIN_58_SHIFT         16U
#define PINMUX_PIN_68_SHIFT         8U
#define PINMUX_PIN_93_SHIFT         8U

#define PINMUX_PIN_1_MASK           (~(uint32)((uint32)0xFFU << PINMUX_PIN_1_SHIFT))
#define PINMUX_PIN_2_MASK           (~(uint32)((uint32)0xFFU << PINMUX_PIN_2_SHIFT))
#define PINMUX_PIN_5_MASK           (~(uint32)((uint32)0xFFU << PINMUX_PIN_5_SHIFT))
#define PINMUX_PIN_8_MASK           (~(uint32)((uint32)0xFFU << PINMUX_PIN_8_SHIFT))
#define PINMUX_PIN_9_MASK           (~(uint32)((uint32)0xFFU << PINMUX_PIN_9_SHIFT))
#define PINMUX_PIN_10_MASK          (~(uint32)((uint32)0xFFU << PINMUX_PIN_10_SHIFT))
#define PINMUX_PIN_12_MASK          (~(uint32)((uint32)0xFFU << PINMUX_PIN_12_SHIFT))
#define PINMUX_PIN_18_MASK          (~(uint32)((uint32)0xFFU << PINMUX_PIN_18_SHIFT))
#define PINMUX_PIN_27_MASK          (~(uint32)((uint32)0xFFU << PINMUX_PIN_27_SHIFT))
#define PINMUX_PIN_36_MASK          (~(uint32)((uint32)0xFFU << PINMUX_PIN_36_SHIFT))
#define PINMUX_PIN_37_MASK          (~(uint32)((uint32)0xFFU << PINMUX_PIN_37_SHIFT))
#define PINMUX_PIN_38_MASK          (~(uint32)((uint32)0xFFU << PINMUX_PIN_38_SHIFT))
#define PINMUX_PIN_39_MASK          (~(uint32)((uint32)0xFFU << PINMUX_PIN_39_SHIFT))
#define PINMUX_PIN_58_MASK          (~(uint32)((uint32)0xFFU << PINMUX_PIN_58_SHIFT))
#define PINMUX_PIN_68_MASK          (~(uint32)((uint32)0xFFU << PINMUX_PIN_68_SHIFT))
#define PINMUX_PIN_93_MASK          (~(uint32)((uint32)0xFFU << PINMUX_PIN_93_SHIFT))



#define PINMUX_PIN_1_GIOA_0         ((uint32)((uint32)0x1U <<  PINMUX_PIN_1_SHIFT))
#define PINMUX_PIN_1_SPI3nCS_3      ((uint32)((uint32)0x2U <<  PINMUX_PIN_1_SHIFT))

#define PINMUX_PIN_2_GIOA_1         ((uint32)((uint32)0x1U <<  PINMUX_PIN_2_SHIFT))
#define PINMUX_PIN_2_SPI3nCS_2      ((uint32)((uint32)0x2U <<  PINMUX_PIN_2_SHIFT))

#define PINMUX_PIN_5_GIOA_2         ((uint32)((uint32)0x1U <<  PINMUX_PIN_5_SHIFT))
#define PINMUX_PIN_5_SPI3nCS_1      ((uint32)((uint32)0x2U <<  PINMUX_PIN_5_SHIFT))

#define PINMUX_PIN_8_GIOA_3         ((uint32)((uint32)0x1U <<  PINMUX_PIN_8_SHIFT))
#define PINMUX_PIN_8_SPI2nCS_3      ((uint32)((uint32)0x2U <<  PINMUX_PIN_8_SHIFT))

#define PINMUX_PIN_9_GIOA_4         ((uint32)((uint32)0x1U <<  PINMUX_PIN_9_SHIFT))
#define PINMUX_PIN_9_SPI2nCS_2      ((uint32)((uint32)0x2U <<  PINMUX_PIN_9_SHIFT))

#define PINMUX_PIN_10_GIOA_5        ((uint32)((uint32)0x1U <<  PINMUX_PIN_10_SHIFT))
#define PINMUX_PIN_10_EXTCLKIN      ((uint32)((uint32)0x2U <<  PINMUX_PIN_10_SHIFT))

#define PINMUX_PIN_12_GIOA_6        ((uint32)((uint32)0x1U <<  PINMUX_PIN_12_SHIFT))
#define PINMUX_PIN_12_SPI2nCS_1     ((uint32)((uint32)0x2U <<  PINMUX_PIN_12_SHIFT))
#define PINMUX_PIN_12_HET_31        ((uint32)((uint32)0x4U <<  PINMUX_PIN_12_SHIFT))

#define PINMUX_PIN_18_GIOA_7        ((uint32)((uint32)0x1U <<  PINMUX_PIN_18_SHIFT))
#define PINMUX_PIN_18_HET_29        ((uint32)((uint32)0x2U <<  PINMUX_PIN_18_SHIFT))

#define PINMUX_PIN_27_MIBSPI1nCS_2  ((uint32)((uint32)0x1U <<  PINMUX_PIN_27_SHIFT))
#define PINMUX_PIN_27_HET_20        ((uint32)((uint32)0x2U <<  PINMUX_PIN_27_SHIFT))
#define PINMUX_PIN_27_HET_19        ((uint32)((uint32)0x4U <<  PINMUX_PIN_27_SHIFT))

#define PINMUX_PIN_36_SPI3CLK       ((uint32)((uint32)0x1U <<  PINMUX_PIN_36_SHIFT))
#define PINMUX_PIN_36_EQEPA         ((uint32)((uint32)0x2U <<  PINMUX_PIN_36_SHIFT))

#define PINMUX_PIN_37_SPI3nENA      ((uint32)((uint32)0x1U <<  PINMUX_PIN_37_SHIFT))
#define PINMUX_PIN_37_EQEPB         ((uint32)((uint32)0x2U <<  PINMUX_PIN_37_SHIFT))

#define PINMUX_PIN_38_SPI3nCS_0     ((uint32)((uint32)0x1U <<  PINMUX_PIN_38_SHIFT))
#define PINMUX_PIN_38_EQEPI         ((uint32)((uint32)0x2U <<  PINMUX_PIN_38_SHIFT))

#define PINMUX_PIN_39_MIBSPI1nCS_3  ((uint32)((uint32)0x1U <<  PINMUX_PIN_39_SHIFT))
#define PINMUX_PIN_39_HET_26        ((uint32)((uint32)0x2U <<  PINMUX_PIN_39_SHIFT))

#define PINMUX_PIN_58_ADEVT         ((uint32)((uint32)0x1U <<  PINMUX_PIN_58_SHIFT))
#define PINMUX_PIN_58_HET_28        ((uint32)((uint32)0x2U <<  PINMUX_PIN_58_SHIFT))

#define PINMUX_PIN_68_MIBSPI1nENA   ((uint32)((uint32)0x1U <<  PINMUX_PIN_68_SHIFT))
#define PINMUX_PIN_68_HET_23        ((uint32)((uint32)0x2U <<  PINMUX_PIN_68_SHIFT))
#define PINMUX_PIN_68_HET_30        ((uint32)((uint32)0x4U <<  PINMUX_PIN_68_SHIFT))

#define PINMUX_PIN_93_MIBSPI1nCS_1  ((uint32)((uint32)0x1U <<  PINMUX_PIN_93_SHIFT))
#define PINMUX_PIN_93_EQEPS         ((uint32)((uint32)0x2U <<  PINMUX_PIN_93_SHIFT))
#define PINMUX_PIN_93_HET_17        ((uint32)((uint32)0x4U <<  PINMUX_PIN_93_SHIFT))

typedef struct pinmux_config_reg
{
    uint32 CONFIG_PINMMR0;
    uint32 CONFIG_PINMMR1;
    uint32 CONFIG_PINMMR2;
    uint32 CONFIG_PINMMR3;
    uint32 CONFIG_PINMMR4;
    uint32 CONFIG_PINMMR5;
    uint32 CONFIG_PINMMR6;
}pinmux_config_reg_t;


/**
 *  @defgroup IOMM IOMM
 *  @brief I/O Multiplexing and Control Module.
 *
 *  The IOMM contains memory-mapped registers (MMR) that control device-specific multiplexed functions.
 *  The safety and diagnostic features of the IOMM are:
 *  - Kicker mechanism to protect the MMRs from accidental writes
 *  - Master-id checker to only allow the CPU to write to the MMRs
 *  - Error indication for access violations
 *
 *  Related Files
 *   - reg_pinmux.h
 *   - pinmux.h
 *   - pinmux.c
 *  @addtogroup IOMM
 *  @{
 */

/** @fn void muxInit(void)
*   @brief Initializes the PINMUX Driver
*
*   This function initializes the PINMUX module and configures the selected
*   pinmux settings as per the user selection in the GUI
*/
void muxInit(void);
void pinmuxGetConfigValue(pinmux_config_reg_t *config_reg, config_value_type_t type);
/* USER CODE BEGIN (0) */
/* USER CODE END */

/**@}*/
#endif
