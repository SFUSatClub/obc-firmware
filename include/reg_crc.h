/** @file reg_crc.h
*   @brief CRC Register Layer Header File
*   @date 07-July-2017
*   @version 04.07.00
*   
*   This file contains:
*   - Definitions
*   - Types
*   - Interface Prototypes
*   .
*   which are relevant for the CRC driver.
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


#ifndef __REG_CRC_H__
#define __REG_CRC_H__

#include "sys_common.h"

/* USER CODE BEGIN (0) */
/* USER CODE END */

/* Crc Register Frame Definition */
/** @struct crcBase
*   @brief CRC Register Frame Definition
*
*   This type is used to access the CRC Registers.
*/
/** @typedef crcBASE_t
*   @brief CRC Register Frame Type Definition
*
*   This type is used to access the CRC Registers.
*/
typedef volatile struct crcBase
{
    uint32      CTRL0;                /**< 0x0000: Global Control Register 0 >**/
    uint32      rvd1;                 /**< 0x0004: reserved >**/
    uint32      CTRL1;                /**< 0x0008: Global Control Register 1 >**/
    uint32      rvd2;                 /**< 0x000C: reserved >**/    
    uint32      CTRL2;                /**< 0x0010: Global Control Register 2 >**/
    uint32      rvd3[19];             /**< 0x0014-0x0060: reserved >**/        
    uint32      PSA_SIGREGL1;         /**< 0x0060: Channel 1 PSA signature low register >**/
    uint32      PSA_SIGREGH1;         /**< 0x0064: Channel 1 PSA signature high register >**/
    uint32      rvd4[4];              /**< 0x0068-0x0078: reserved >**/        
    uint32      RAW_DATAREGL1;        /**< 0x0078: Channel 1 Raw Data Low Register >**/
    uint32      RAW_DATAREGH1;        /**< 0x007C: Channel 1 Raw Data High Register >**/
    uint32      rvd5[8];              /**< 0x0080-0x00A0: reserved >**/
    uint32      PSA_SIGREGL2;         /**< 0x00A0: Channel 2 PSA signature low register >**/
    uint32      PSA_SIGREGH2;         /**< 0x00A4: Channel 2 PSA signature high register >**/
    uint32      rvd6[4];              /**< 0x00A8-0x00B8: reserved >**/    
	uint32      RAW_DATAREGL2;        /**< 0x00B8: Channel 2 Raw Data Low Register >**/
    uint32      RAW_DATAREGH2;        /**< 0x00BC: Channel 2 Raw Data High Register >**/
}crcBASE_t;
 
/** @def crcREG
*   @brief CRC Register Frame Pointer
*
*   This pointer is used by the CRC driver to access the CRC registers.
*/
#define crcREG ((crcBASE_t *)0xFE000000U)

/* USER CODE BEGIN (1) */
/* USER CODE END */


#endif
