/*
    FreeRTOS V8.2.0 - Copyright (C) 2015 Real Time Engineers Ltd.
    All rights reserved

    VISIT http://www.FreeRTOS.org TO ENSURE YOU ARE USING THE LATEST VERSION.

    This file is part of the FreeRTOS distribution.

    FreeRTOS is free software; you can redistribute it and/or modify it under
    the terms of the GNU General Public License (version 2) as published by the
    Free Software Foundation >>!AND MODIFIED BY!<< the FreeRTOS exception.

	***************************************************************************
    >>!   NOTE: The modification to the GPL is included to allow you to     !<<
    >>!   distribute a combined work that includes FreeRTOS without being   !<<
    >>!   obliged to provide the source code for proprietary components     !<<
    >>!   outside of the FreeRTOS kernel.                                   !<<
	***************************************************************************

    FreeRTOS is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  Full license text is available on the following
    link: http://www.freertos.org/a00114.html

    ***************************************************************************
     *                                                                       *
     *    FreeRTOS provides completely free yet professionally developed,    *
     *    robust, strictly quality controlled, supported, and cross          *
     *    platform software that is more than just the market leader, it     *
     *    is the industry's de facto standard.                               *
     *                                                                       *
     *    Help yourself get started quickly while simultaneously helping     *
     *    to support the FreeRTOS project by purchasing a FreeRTOS           *
     *    tutorial book, reference manual, or both:                          *
     *    http://www.FreeRTOS.org/Documentation                              *
     *                                                                       *
    ***************************************************************************

    http://www.FreeRTOS.org/FAQHelp.html - Having a problem?  Start by reading
	the FAQ page "My application does not run, what could be wrong?".  Have you
	defined configASSERT()?

	http://www.FreeRTOS.org/support - In return for receiving this top quality
	embedded software for free we request you assist our global community by
	participating in the support forum.

	http://www.FreeRTOS.org/training - Investing in training allows your team to
	be as productive as possible as early as possible.  Now you can receive
	FreeRTOS training directly from Richard Barry, CEO of Real Time Engineers
	Ltd, and the world's leading authority on the world's leading RTOS.

    http://www.FreeRTOS.org/plus - A selection of FreeRTOS ecosystem products,
    including FreeRTOS+Trace - an indispensable productivity tool, a DOS
    compatible FAT file system, and our tiny thread aware UDP/IP stack.

    http://www.FreeRTOS.org/labs - Where new FreeRTOS products go to incubate.
    Come and try FreeRTOS+TCP, our new open source TCP/IP stack for FreeRTOS.

    http://www.OpenRTOS.com - Real Time Engineers ltd. license FreeRTOS to High
    Integrity Systems ltd. to sell under the OpenRTOS brand.  Low cost OpenRTOS
    licenses offer ticketed support, indemnification and commercial middleware.

    http://www.SafeRTOS.com - High Integrity Systems also provide a safety
    engineered and independently SIL3 certified version for use in safety and
    mission critical applications that require provable dependability.

    1 tab == 4 spaces!
*/

#define MPU_WRAPPERS_INCLUDED_FROM_API_FILE

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "rtos_task.h"
#include "sys_core.h"


#undef MPU_WRAPPERS_INCLUDED_FROM_API_FILE

/*-----------------------------------------------------------*/

/* Registers required to configure the RTI. */
#define portRTI_GCTRL_REG  		( * ( ( volatile uint32_t * ) 0xFFFFFC00 ) )
#define portRTI_TBCTRL_REG  	( * ( ( volatile uint32_t * ) 0xFFFFFC04 ) )
#define portRTI_COMPCTRL_REG  	( * ( ( volatile uint32_t * ) 0xFFFFFC0C ) )
#define portRTI_CNT0_FRC0_REG  	( * ( ( volatile uint32_t * ) 0xFFFFFC10 ) )
#define portRTI_CNT0_UC0_REG  	( * ( ( volatile uint32_t * ) 0xFFFFFC14 ) )
#define portRTI_CNT0_CPUC0_REG  ( * ( ( volatile uint32_t * ) 0xFFFFFC18 ) )
#define portRTI_CNT0_COMP0_REG  ( * ( ( volatile uint32_t * ) 0xFFFFFC50 ) )
#define portRTI_CNT0_UDCP0_REG  ( * ( ( volatile uint32_t * ) 0xFFFFFC54 ) )
#define portRTI_SETINTENA_REG  	( * ( ( volatile uint32_t * ) 0xFFFFFC80 ) )
#define portRTI_CLEARINTENA_REG ( * ( ( volatile uint32_t * ) 0xFFFFFC84 ) )
#define portRTI_INTFLAG_REG  	( * ( ( volatile uint32_t * ) 0xFFFFFC88 ) )


/* Constants required to set up the initial stack of each task. */
#define portINITIAL_SPSR	   	( ( StackType_t ) 0x1F )
#define portINITIAL_SPSR_IF_PRIVILEGED   ( ( StackType_t ) 0x1F )
#define portINITIAL_SPSR_IF_UNPRIVILEGED ( ( StackType_t ) 0x10 )
#define portINITIAL_FPSCR	  	( ( StackType_t ) 0x00 )
#define portINSTRUCTION_SIZE   	( ( StackType_t ) 0x04 )
#define portTHUMB_MODE_BIT		( ( StackType_t ) 0x20 )

/* The number of words on the stack frame between the saved Top Of Stack and
R0 (in which the parameters are passed. */
#define portSPACE_BETWEEN_TOS_AND_PARAMETERS	( 12 )


/*-----------------------------------------------------------*/

/* vPortStartFirstSTask() is defined in portASM.asm */
extern void vPortStartFirstTask( void );

///* MPU access routines defined in portASM.asm */
extern void prvMpuEnable( void );
extern void prvMpuDisable( void );
extern void prvMpuSetRegion( unsigned region, unsigned base, unsigned size, unsigned access );


/*-----------------------------------------------------------*/

/* Count of the critical section nesting depth. */
PRIVILEGED_DATA unsigned portLONG ulCriticalNesting = 9999;

/*-----------------------------------------------------------*/

/* Saved as part of the task context.  Set to pdFALSE if the task does not
require an FPU context. */
uint32_t ulTaskHasFPUContext = 0;

/*-----------------------------------------------------------*/

/*-----------------------------------------------------------*/

/*-----------------------------------------------------------*/
/*
 * See header file for description.
 */
#if( portUSING_MPU_WRAPPERS == 1 )
StackType_t *pxPortInitialiseStack( StackType_t *pxTopOfStack, TaskFunction_t pxCode, void *pvParameters, BaseType_t xRunPrivileged )
{
StackType_t *pxOriginalTOS;

    pxOriginalTOS = pxTopOfStack;


    /* Setup the initial stack of the task.  The stack is set exactly as
    expected by the portRESTORE_CONTEXT() macro. */

    /* First on the stack is the return address - which is the start of the as
    the task has not executed yet.  The offset is added to make the return
    address appear as it would within an IRQ ISR. */
    *pxTopOfStack = ( StackType_t ) pxCode + portINSTRUCTION_SIZE;
    pxTopOfStack--;

    *pxTopOfStack = ( StackType_t ) 0x00000000; /* R14 */
    pxTopOfStack--;
    *pxTopOfStack = ( StackType_t ) pxOriginalTOS; /* Stack used when task starts goes in R13. */
    pxTopOfStack--;

    #ifdef portPRELOAD_TASK_REGISTERS
    {
        *pxTopOfStack = ( StackType_t ) 0x12121212; /* R12 */
        pxTopOfStack--;
        *pxTopOfStack = ( StackType_t ) 0x11111111; /* R11 */
        pxTopOfStack--;
        *pxTopOfStack = ( StackType_t ) 0x10101010; /* R10 */
        pxTopOfStack--;
        *pxTopOfStack = ( StackType_t ) 0x09090909; /* R9 */
        pxTopOfStack--;
        *pxTopOfStack = ( StackType_t ) 0x08080808; /* R8 */
        pxTopOfStack--;
        *pxTopOfStack = ( StackType_t ) 0x07070707; /* R7 */
        pxTopOfStack--;
        *pxTopOfStack = ( StackType_t ) 0x06060606; /* R6 */
        pxTopOfStack--;
        *pxTopOfStack = ( StackType_t ) 0x05050505; /* R5 */
        pxTopOfStack--;
        *pxTopOfStack = ( StackType_t ) 0x04040404; /* R4 */
        pxTopOfStack--;
        *pxTopOfStack = ( StackType_t ) 0x03030303; /* R3 */
        pxTopOfStack--;
        *pxTopOfStack = ( StackType_t ) 0x02020202; /* R2 */
        pxTopOfStack--;
        *pxTopOfStack = ( StackType_t ) 0x01010101; /* R1 */
        pxTopOfStack--;
    }
    #else
    {
        pxTopOfStack -= portSPACE_BETWEEN_TOS_AND_PARAMETERS;
    }
    #endif

    /* Function parameters are passed in R0. */
    *pxTopOfStack = ( StackType_t ) pvParameters; /* R0 */
    pxTopOfStack--;

    /* Set the status register for system or user mode, with interrupts enabled. */
    if( xRunPrivileged == pdTRUE )
    {
        *pxTopOfStack = (portSTACK_TYPE) ((_getCPSRValue_() & ~0xFF) | portINITIAL_SPSR_IF_PRIVILEGED);
    }
    else
    {
        *pxTopOfStack = (portSTACK_TYPE) ((_getCPSRValue_() & ~0xFF) | portINITIAL_SPSR_IF_UNPRIVILEGED);
    }

    if( ( ( uint32_t ) pxCode & 0x01UL ) != 0x00 )
    {
        /* The task will start in thumb mode. */
        *pxTopOfStack |= portTHUMB_MODE_BIT;
    }

    return pxTopOfStack;
}
#else // definition without xRunprivileged
StackType_t *pxPortInitialiseStack( StackType_t *pxTopOfStack, TaskFunction_t pxCode, void *pvParameters )
{
StackType_t *pxOriginalTOS;

    pxOriginalTOS = pxTopOfStack;

    #if __TI_VFP_SUPPORT__
    {
        /* Ensure the stack is correctly aligned on exit. */
        pxTopOfStack--;
    }
    #endif

    /* Setup the initial stack of the task.  The stack is set exactly as
    expected by the portRESTORE_CONTEXT() macro. */

    /* First on the stack is the return address - which is the start of the as
    the task has not executed yet.  The offset is added to make the return
    address appear as it would within an IRQ ISR. */
    *pxTopOfStack = ( StackType_t ) pxCode + portINSTRUCTION_SIZE;
    pxTopOfStack--;

    *pxTopOfStack = ( StackType_t ) 0x00000000; /* R14 */
    pxTopOfStack--;
    *pxTopOfStack = ( StackType_t ) pxOriginalTOS; /* Stack used when task starts goes in R13. */
    pxTopOfStack--;

    #ifdef portPRELOAD_TASK_REGISTERS
    {
        *pxTopOfStack = ( StackType_t ) 0x12121212; /* R12 */
        pxTopOfStack--;
        *pxTopOfStack = ( StackType_t ) 0x11111111; /* R11 */
        pxTopOfStack--;
        *pxTopOfStack = ( StackType_t ) 0x10101010; /* R10 */
        pxTopOfStack--;
        *pxTopOfStack = ( StackType_t ) 0x09090909; /* R9 */
        pxTopOfStack--;
        *pxTopOfStack = ( StackType_t ) 0x08080808; /* R8 */
        pxTopOfStack--;
        *pxTopOfStack = ( StackType_t ) 0x07070707; /* R7 */
        pxTopOfStack--;
        *pxTopOfStack = ( StackType_t ) 0x06060606; /* R6 */
        pxTopOfStack--;
        *pxTopOfStack = ( StackType_t ) 0x05050505; /* R5 */
        pxTopOfStack--;
        *pxTopOfStack = ( StackType_t ) 0x04040404; /* R4 */
        pxTopOfStack--;
        *pxTopOfStack = ( StackType_t ) 0x03030303; /* R3 */
        pxTopOfStack--;
        *pxTopOfStack = ( StackType_t ) 0x02020202; /* R2 */
        pxTopOfStack--;
        *pxTopOfStack = ( StackType_t ) 0x01010101; /* R1 */
        pxTopOfStack--;
    }
    #else
    {
        pxTopOfStack -= portSPACE_BETWEEN_TOS_AND_PARAMETERS;
    }
    #endif

    /* Function parameters are passed in R0. */
    *pxTopOfStack = ( StackType_t ) pvParameters; /* R0 */
    pxTopOfStack--;

    /* Set the status register for system mode, with interrupts enabled. */
    *pxTopOfStack = ( StackType_t ) ( ( _get_CPSR() & ~0xFF ) | portINITIAL_SPSR );

    if( ( ( uint32_t ) pxCode & 0x01UL ) != 0x00 )
    {
        /* The task will start in thumb mode. */
        *pxTopOfStack |= portTHUMB_MODE_BIT;
    }

    #ifdef __TI_VFP_SUPPORT__
    {
        pxTopOfStack--;

        /* The last thing on the stack is the tasks ulUsingFPU value, which by
        default is set to indicate that the stack frame does not include FPU
        registers. */
        *pxTopOfStack = pdFALSE;
    }
    #endif

    return pxTopOfStack;
}

#endif
/*----------------------------------------------------------------------------*/

static unsigned long prvGetMPURegionSizeSetting( unsigned long ulActualSizeInBytes )
{
    unsigned long ulRegionSize, ulReturnValue = 4;

    /* 32 is the smallest region size, 31 is the largest valid value for
    ulReturnValue. */
    for( ulRegionSize = 32UL; ulReturnValue < 31UL; ( ulRegionSize <<= 1UL ) )
    {
        if( ulActualSizeInBytes <= ulRegionSize )
        {
            break;
        }
        else
        {
            ulReturnValue++;
        }
    }

    /* Shift the code by one before returning so it can be written directly
    into the the correct bit position of the attribute register. */
    return ulReturnValue << 1UL;
}



/*----------------------------------------------------------------------------*/

void vPortStoreTaskMPUSettings( xMPU_SETTINGS *xMPUSettings, const struct xMEMORY_REGION * const xRegions, portSTACK_TYPE *pxBottomOfStack, unsigned short usStackDepth ){
	long lIndex;
	unsigned long ul;

	if( xRegions == NULL )
	{
		/* No MPU regions are specified so allow access to all of the RAM. */
		xMPUSettings->xRegion[0].ulRegionBaseAddress = 0x08000000;
		xMPUSettings->xRegion[0].ulRegionSize        = portMPU_SIZE_512KB | portMPU_REGION_ENABLE;
		xMPUSettings->xRegion[0].ulRegionAttribute   = portMPU_PRIV_RW_USER_RW_EXEC | portMPU_NORMAL_OIWTNOWA_SHARED;

		/* Re-instate the privileged only RAM region as xRegion[ 0 ] will have
		just removed the privileged only parameters. */
		xMPUSettings->xRegion[1].ulRegionBaseAddress = 0x08000000;
		xMPUSettings->xRegion[1].ulRegionSize        = portMPU_SIZE_4KB | portMPU_REGION_ENABLE;
		xMPUSettings->xRegion[1].ulRegionAttribute   = portMPU_PRIV_RW_USER_NA_NOEXEC | portMPU_NORMAL_OIWTNOWA_SHARED;

		/* Invalidate all other regions. */
		for( ul = 2; ul <= portNUM_CONFIGURABLE_REGIONS; ul++ )
		{
			xMPUSettings->xRegion[ ul ].ulRegionBaseAddress = 0x00000000UL;
			xMPUSettings->xRegion[ ul ].ulRegionSize        = 0UL;
			xMPUSettings->xRegion[ ul ].ulRegionAttribute   = 0UL;
		}
	}
	else
	{
		/* This function is called automatically when the task is created - in
		which case the stack region parameters will be valid.  At all other
		times the stack parameters will not be valid and it is assumed that the
		stack region has already been configured. */
		if( usStackDepth > 0 )
		{
			/* Define the region that allows access to the stack. */
			xMPUSettings->xRegion[0].ulRegionBaseAddress = (unsigned)pxBottomOfStack;
			xMPUSettings->xRegion[0].ulRegionSize        = prvGetMPURegionSizeSetting( (unsigned long)usStackDepth * (unsigned long) sizeof(portSTACK_TYPE) ) | portMPU_REGION_ENABLE;
			xMPUSettings->xRegion[0].ulRegionAttribute   = portMPU_REGION_READ_WRITE | portMPU_NORMAL_OIWTNOWA_SHARED;

		}
		lIndex = 0;

		for( ul = 1; ul <= portNUM_CONFIGURABLE_REGIONS; ul++ )
		{
			if( ( xRegions[ lIndex ] ).ulLengthInBytes > 0UL )
			{
				/* Translate the generic region definition contained in
				xRegions into the R4 specific MPU settings that are then
				stored in xMPUSettings. */
				xMPUSettings->xRegion[ul].ulRegionBaseAddress = (unsigned long) xRegions[lIndex].pvBaseAddress;
				xMPUSettings->xRegion[ul].ulRegionSize        = prvGetMPURegionSizeSetting( xRegions[ lIndex ].ulLengthInBytes ) | portMPU_REGION_ENABLE;
				xMPUSettings->xRegion[ul].ulRegionAttribute   = xRegions[ lIndex ].ulParameters;
			}
			else
			{
				/* Invalidate the region. */
				xMPUSettings->xRegion[ ul ].ulRegionBaseAddress = 0x00000000UL;
				xMPUSettings->xRegion[ ul ].ulRegionSize        = 0UL;
				xMPUSettings->xRegion[ ul ].ulRegionAttribute   = 0UL;
			}
			lIndex++;
		}
	}
}

/*----------------------------------------------------------------------------*/

static void prvSetupDefaultMPU( void )
{
	/* make sure MPU is disabled */
	prvMpuDisable();

	/* First setup the entire flash for unprivileged read only access. */
	prvMpuSetRegion(portUNPRIVILEGED_FLASH_REGION,  0x00000000, portMPU_SIZE_4MB | portMPU_REGION_ENABLE, portMPU_PRIV_RO_USER_RO_EXEC | portMPU_NORMAL_OIWTNOWA_SHARED);

	/* Setup the first 32K for privileged only access.  This is where the kernel code is
	placed. */
	prvMpuSetRegion(portPRIVILEGED_FLASH_REGION,  0x00000000, portMPU_SIZE_32KB | portMPU_REGION_ENABLE, portMPU_PRIV_RO_USER_NA_EXEC | portMPU_NORMAL_OIWTNOWA_SHARED);

	/* Setup the the entire RAM region for privileged read-write and unprivileged read only access */
	prvMpuSetRegion(portPRIVILEGED_RAM_REGION,  0x08000000, portMPU_SIZE_512KB | portMPU_REGION_ENABLE, portMPU_PRIV_RW_USER_RO_EXEC | portMPU_NORMAL_OIWTNOWA_SHARED);

	/* Default peripherals setup */
	prvMpuSetRegion(portGENERAL_PERIPHERALS_REGION,  0xF0000000,
					portMPU_SIZE_256MB | portMPU_REGION_ENABLE | portMPU_SUBREGION_1_DISABLE | portMPU_SUBREGION_2_DISABLE | portMPU_SUBREGION_3_DISABLE | portMPU_SUBREGION_4_DISABLE,
					portMPU_PRIV_RW_USER_RW_NOEXEC | portMPU_DEVICE_NONSHAREABLE);

	/* Privilege System Region setup */
	prvMpuSetRegion(portPRIVILEGED_SYSTEM_REGION,  0xFFF80000, portMPU_SIZE_512KB | portMPU_REGION_ENABLE, portMPU_PRIV_RW_USER_RO_NOEXEC | portMPU_DEVICE_NONSHAREABLE);
	
	/* Enable MPU */
	prvMpuEnable();
}


/*-----------------------------------------------------------*/

static void prvSetupTimerInterrupt(void)
{
	/* Disable timer 0. */
	portRTI_GCTRL_REG &= 0xFFFFFFFEUL;

	/* Use the internal counter. */
	portRTI_TBCTRL_REG = 0x00000000U;

	/* COMPSEL0 will use the RTIFRC0 counter. */
	portRTI_COMPCTRL_REG = 0x00000000U;

	/* Initialise the counter and the prescale counter registers. */
	portRTI_CNT0_UC0_REG  =  0x00000000U;
	portRTI_CNT0_FRC0_REG =  0x00000000U;

	/* Set Prescalar for RTI clock. */
	portRTI_CNT0_CPUC0_REG = 0x00000001U;
	portRTI_CNT0_COMP0_REG = ( configCPU_CLOCK_HZ / 2 ) / configTICK_RATE_HZ;
	portRTI_CNT0_UDCP0_REG = ( configCPU_CLOCK_HZ / 2 ) / configTICK_RATE_HZ;

	/* Clear interrupts. */
	portRTI_INTFLAG_REG     =  0x0007000FU;
	portRTI_CLEARINTENA_REG	= 0x00070F0FU;

	/* Enable the compare 0 interrupt. */
	portRTI_SETINTENA_REG = 0x00000001U;
	portRTI_GCTRL_REG    |= 0x00000001U;
}
/*-----------------------------------------------------------*/

/*
 * See header file for description.
 */

/*-----------------------------------------------------------*/

/*
 * See header file for description.
 */
BaseType_t xPortStartScheduler(void)
{
    /* Configure the regions in the MPU that are common to all tasks. */
    // Richard: added this to get rid of a warning in v9 port
    prvSetupDefaultMPU();

    /* Start the timer that generates the tick ISR. */
    prvSetupTimerInterrupt();

    /* Reset the critical section nesting count read to execute the first task. */
    ulCriticalNesting = 0;

    /* Start the first task.  This is done from portASM.asm as ARM mode must be
    used. */
    vPortStartFirstTask();

    /* Should not get here! */
    return pdFAIL;
}


void vPortEndScheduler(void)
{
	/* Not implemented in ports where there is nothing to return to.
	Artificially force an assert. */
	configASSERT( ulCriticalNesting == 1000UL );
}
/*-----------------------------------------------------------*/

#if configUSE_PREEMPTION == 0

	/* The cooperative scheduler requires a normal IRQ service routine to
	 * simply increment the system tick. */
	__interrupt void vPortNonPreemptiveTick( void )
	{
		/* clear clock interrupt flag */
		portRTI_INTFLAG_REG = 0x00000001;

		/* Increment the tick count - this may make a delaying task ready
		to run - but a context switch is not performed. */
		xTaskIncrementTick();
	}

 #else

	/*
	 **************************************************************************
	 * The preemptive scheduler ISR is written in assembler and can be found
	 * in the portASM.asm file. This will only get used if portUSE_PREEMPTION
	 * is set to 1 in portmacro.h
	 **************************************************************************
	 */
	void vPortPreemptiveTick( void );

#endif
/*-----------------------------------------------------------*/

/*-----------------------------------------------------------*/
	// RICHARD: I COMMENTED THESE OUT BUT WHYYYYY DOES THAT WORK NOW??? LOL

//	void vPortEnterCritical( void )
//	{
//	    /* Disable interrupts as per portDISABLE_INTERRUPTS(); */
//	    portDISABLE_INTERRUPTS();
//
//	    /* Now interrupts are disabled ulCriticalNesting can be accessed
//	    directly.  Increment ulCriticalNesting to keep a count of how many times
//	    portENTER_CRITICAL() has been called. */
//	    ulCriticalNesting++;
//	}
//	/*-----------------------------------------------------------*/
//
//	/*
//	 * Decrement the critical nesting count, and if it has reached zero, re-enable
//	 * interrupts.
//	 */
//	void vPortExitCritical( void )
//	{
//	    if( ulCriticalNesting > 0 )
//	    {
//	        /* Decrement the nesting count as we are leaving a critical section. */
//	        ulCriticalNesting--;
//
//	        /* If the nesting level has reached zero then interrupts should be
//	        re-enabled. */
//	        if( ulCriticalNesting == 0 )
//	        {
//	            /* Enable interrupts as per portENABLE_INTERRUPTS(). */
//	            portENABLE_INTERRUPTS();
//	        }
//	    }
//	}
	/*-----------------------------------------------------------*/

	#if __TI_VFP_SUPPORT__

	    void vPortTaskUsesFPU( void )
	    {
	    extern void vPortInitialiseFPSCR( void );

	        /* A task is registering the fact that it needs an FPU context.  Set the
	        FPU flag (saved as part of the task context. */
	        ulTaskHasFPUContext = pdTRUE;

	        /* Initialise the floating point status register. */
	        vPortInitialiseFPSCR();
	    }

	#endif /* __TI_VFP_SUPPORT__ */

	/*-----------------------------------------------------------*/


