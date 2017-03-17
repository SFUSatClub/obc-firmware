/** @file pinmux.c 
*   @brief PINMUX Driver Implementation File
*   @date 05-Oct-2016
*   @version 04.06.00
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


/* Include Files */

#include "pinmux.h"

/*LDRA_INSPECTWINDOW 50 */
/*SAFETYMCUSW 125 S MR: 19.13 REVIEWED " Needs usage of ## in the macro " */
/*SAFETYMCUSW 76 S MR: 19.12 REVIEWED " Needs usage of multiple ## in the macro " */
/*SAFETYMCUSW 78 S MR: 19.10 REVIEWED " Macro parameter used for concatenation " */
/*SAFETYMCUSW 78 S MR: 19.10 REVIEWED " Macro parameter used for concatenation " */
/*SAFETYMCUSW 78 S MR: 19.10 REVIEWED " Macro parameter used for concatenation " */
/*SAFETYMCUSW 78 S MR: 19.10 REVIEWED " Macro parameter used for concatenation " */
/*SAFETYMCUSW 78 S MR: 19.10 REVIEWED " Macro parameter used for concatenation " */
/*SAFETYMCUSW 76 S MR: 19.12 REVIEWED " Needs usage of multiple ## in the macro " */
/*SAFETYMCUSW 76 S MR: 19.12 REVIEWED " Needs usage of multiple ## in the macro " */
/*SAFETYMCUSW 76 S MR: 19.12 REVIEWED " Needs usage of multiple ## in the macro " */
/*SAFETYMCUSW 76 S MR: 19.12 REVIEWED " Needs usage of multiple ## in the macro " */
/*SAFETYMCUSW 76 S MR: 19.12 REVIEWED " Needs usage of multiple ## in the macro " */
/*SAFETYMCUSW 76 S MR: 19.12 REVIEWED " Needs usage of multiple ## in the macro " */
/*SAFETYMCUSW 76 S MR: 19.12 REVIEWED " Needs usage of multiple ## in the macro " */
#define PINMUX_SET(REG, BALLID, MUX)                                \
            (pinMuxReg->PINMMR##REG = (pinMuxReg->PINMMR##REG & PINMUX_BALL_##BALLID##_MASK) | (PINMUX_BALL_##BALLID##_##MUX))


#define PINMUX_GIOB_DISABLE_HET2_ENABLE \
            (pinMuxReg->PINMMR29 = (pinMuxReg->PINMMR29 & PINMUX_GIOB_DISABLE_HET2_MASK) | PINMUX_GIOB_DISABLE_HET2)

/*SAFETYMCUSW 125 S MR: 19.13 REVIEWED " Needs usage of ## in the macro " */
/*SAFETYMCUSW 78 S MR: 19.10 REVIEWED " Macro parameter used for concatenation " */         
#define PINMUX_ALT_ADC_TRIGGER_SELECT(num)  \
            (pinMuxReg->PINMMR30 = (pinMuxReg->PINMMR30 & PINMUX_ALT_ADC_TRIGGER_MASK) | (PINMUX_ALT_ADC_TRIGGER_##num))
            

/* USER CODE BEGIN (0) */
/* USER CODE END */

/* SourceId : PINMUX_SourceId_001 */
/* DesignId : PINMUX_DesignId_001 */
/* Requirements : HL_SR325 */
void muxInit(void){

/* USER CODE BEGIN (1) */
/* USER CODE END */

    /* Enable Pin Muxing */
    kickerReg->KICKER0 = 0x83E70B13U;
    kickerReg->KICKER1 = 0x95A4F1E0U;
    
/* USER CODE BEGIN (2) */
/* USER CODE END */

        pinMuxReg->PINMMR0 = PINMUX_PIN_1_GIOA_0;
    
    pinMuxReg->PINMMR1 = PINMUX_PIN_2_GIOA_1 | PINMUX_PIN_5_GIOA_2 | PINMUX_PIN_8_GIOA_3 | PINMUX_PIN_9_GIOA_4;
    
    pinMuxReg->PINMMR2 = PINMUX_PIN_10_GIOA_5 | PINMUX_PIN_12_GIOA_6 | PINMUX_PIN_18_GIOA_7;
    
    pinMuxReg->PINMMR3 = PINMUX_PIN_27_MIBSPI1nCS_2 | PINMUX_PIN_36_SPI3CLK | PINMUX_PIN_37_SPI3nENA;
    
    pinMuxReg->PINMMR4 = PINMUX_PIN_38_SPI3nCS_0 | PINMUX_PIN_39_MIBSPI1nCS_3 | PINMUX_PIN_58_ADEVT;
    
    pinMuxReg->PINMMR5 = PINMUX_PIN_68_MIBSPI1nENA;
    
    pinMuxReg->PINMMR6 = PINMUX_PIN_93_MIBSPI1nCS_1;
    
    
/* USER CODE BEGIN (3) */
/* USER CODE END */
    
    /* Disable Pin Muxing */
    kickerReg->KICKER0 = 0x00000000U;
    kickerReg->KICKER1 = 0x00000000U;
    
/* USER CODE BEGIN (4) */
/* USER CODE END */
}

/** @fn void pinmuxGetConfigValue(pinmux_config_reg_t *config_reg, config_value_type_t type)
*   @brief Get the initial or current values of the configuration registers
*
*   @param[in] *config_reg: pointer to the struct to which the initial or current 
*                           value of the configuration registers need to be stored
*   @param[in] type:    whether initial or current value of the configuration registers need to be stored
*                       - InitialValue: initial value of the configuration registers will be stored 
*                                       in the struct pointed by config_reg
*                       - CurrentValue: initial value of the configuration registers will be stored 
*                                       in the struct pointed by config_reg
*
*   This function will copy the initial or current value (depending on the parameter 'type') 
*   of the configuration registers to the struct pointed by config_reg
*
*/
/* SourceId : PINMUX_SourceId_002 */
/* DesignId : PINMUX_DesignId_002 */
/* Requirements : HL_SR328 */
void pinmuxGetConfigValue(pinmux_config_reg_t *config_reg, config_value_type_t type)
{
    if (type == InitialValue)
    {/* Do not pass Initial Value as parameter */
    }
    else
    {
    /*SAFETYMCUSW 134 S MR:12.2 <APPROVED> "LDRA Tool issue" */
        config_reg->CONFIG_PINMMR0  = pinMuxReg->PINMMR0;
        config_reg->CONFIG_PINMMR1  = pinMuxReg->PINMMR1;
        config_reg->CONFIG_PINMMR2  = pinMuxReg->PINMMR2;
        config_reg->CONFIG_PINMMR3  = pinMuxReg->PINMMR3;
        config_reg->CONFIG_PINMMR4  = pinMuxReg->PINMMR4;
        config_reg->CONFIG_PINMMR5  = pinMuxReg->PINMMR5;
        config_reg->CONFIG_PINMMR6  = pinMuxReg->PINMMR6;
    }
}

/* USER CODE BEGIN (5) */
/* USER CODE END */
