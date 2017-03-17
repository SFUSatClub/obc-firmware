/*----------------------------------------------------------------------------*/
/* sys_link.cmd                                                               */
/*                                                                            */
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

/*                                                                            */
/*----------------------------------------------------------------------------*/
/* USER CODE BEGIN (0) */
#define SAT_RTOS 1
// Richard: this is a nice little hack to work around the Halcogen limitations. The way this is structured, all of the dedaults from halcogen will be skipped (sat = 1)
// Halcogen isn't letting us change the defaults correctly, so they are set manually using the define at the end of this file :)

/* USER CODE END */


/*----------------------------------------------------------------------------*/
/* Linker Settings                                                            */

--retain="*(.intvecs)"

/* USER CODE BEGIN (1) */
#ifndef SAT_RTOS // if we don't define sat RTOS, use the default memory stuff from halcogen
/* USER CODE END */

/*----------------------------------------------------------------------------*/
/* Memory Map                                                                 */

MEMORY
{
    VECTORS (X)  : origin=0x00000000 length=0x00000020
    FLASH0  (RX) : origin=0x00000020 length=0x0005FFE0
    STACKS  (RW) : origin=0x08000000 length=0x00001500
    RAM     (RW) : origin=0x08001500 length=0x00006b00

/* USER CODE BEGIN (2) */
    KRAM    (RW) : origin=0x08000800 length=0x00000800
	RAM     (RW) : origin=(0x08000800+0x00000800) length=(0x00007800 - 0x00000800)
/* USER CODE END */
}

/* USER CODE BEGIN (3) */

/* USER CODE END */


/*----------------------------------------------------------------------------*/
/* Section Configuration                                                      */

SECTIONS
{
    .intvecs : {} > VECTORS
    .text    : {} > FLASH0
    .const   : {} > FLASH0
    .cinit   : {} > FLASH0
    .pinit   : {} > FLASH0
    .bss     : {} > RAM
    .data    : {} > RAM
	.sysmem  : {} > RAM

/* USER CODE BEGIN (4) */

/* USER CODE END */
}

/* USER CODE BEGIN (5) */
#else
// -------------- THIS WILL BE USED IF SAT_RTOS = 1 ----------------- //
// RICHARD ARTHURS MARCH 3, 2017. ADAPTED FROM WORKING HALCOGEN RTOS IMPLEMENTATION
MEMORY
{
    VECTORS (X)  : origin=0x00000000 length=0x00000020
    KERNEL  (RX) : origin=0x00000020 length=0x00008000
    FLASH0  (RX) : origin=0x00008020 length=0x00057FE0
    STACKS  (RW) : origin=0x08000000 length=0x00000800
    KRAM    (RW) : origin=0x08000800 length=0x00000800
    RAM     (RW) : origin=(0x08000800+0x00000800) length=(0x00007800 - 0x00000800)
}

/* Section Configuration                                                      */
SECTIONS
{
    .intvecs : {} > VECTORS
    /* FreeRTOS Kernel in protected region of Flash */
    .kernelTEXT   : {} > KERNEL
    .cinit        : {} > KERNEL
    .pinit        : {} > KERNEL
	/* Rest of code to user mode flash region */
    .text         : {} > FLASH0
    .const        : {} > FLASH0
	/* FreeRTOS Kernel data in protected region of RAM */
    .kernelBSS    : {} > KRAM
    .kernelHEAP   : {} > RAM
    .bss          : {} > RAM
    .data         : {} > RAM
    .sysmem  : {} > RAM // Richard: this wasn't being included with freeRTOS from halcogen, so I copied it back in.
}

#endif
/* USER CODE END */


/*----------------------------------------------------------------------------*/
/* Misc                                                                       */

/* USER CODE BEGIN (6) */
/* USER CODE END */
/*----------------------------------------------------------------------------*/



