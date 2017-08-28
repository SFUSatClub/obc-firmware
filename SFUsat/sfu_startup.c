/*
 * sfu_startup.c
 *
 * This file contains our pre-main checks. For example, we can query the registers and determine what kind
 * of reset was asserted before the current run. This will be a good indicator of what's going on when we get up
 * into space. The code came from HALCoGen and using #defines in sys_startup.c, disables existing code and inserts itself after
 * the system performs self tests and clears out the RAM.
 *
 *  Created on: Aug 27, 2017
 *      Author: Richard
 */

#include "sfu_startup.h"
#include "sfu_uart.h"

Startup_Data_t startData;

void startupInit(){
	startData.resetSrc = DEFAULT_START;
}

void printStartupType(void){
	serialSendln(STARTUP_STRING[startData.resetSrc]); // print the type of reset that was triggered.
}

void startupCheck(void){

	/* USER CODE END */

	/* Workaround for Errata CORTEXR4 66 */
	_errata_CORTEXR4_66_();

	/* Reset handler: the following instructions read from the system exception status register
	 * to identify the cause of the CPU reset.
	 */

	/* check for power-on reset condition */
	/*SAFETYMCUSW 139 S MR:13.7 <APPROVED> "Hardware status bit read check" */
	if ((SYS_EXCEPTION & POWERON_RESET) != 0U)
	{
		startData.resetSrc = PORRST_START;  // RA
		SYS_EXCEPTION = 0xFFFFU; /* clear all reset status flags */
		/* continue with normal start-up sequence */
	}
	/*SAFETYMCUSW 139 S MR:13.7 <APPROVED> "Hardware status bit read check" */
	else if ((SYS_EXCEPTION & OSC_FAILURE_RESET) != 0U)
	{
		/* Reset caused due to oscillator failure.
        Add user code here to handle oscillator failure */

		startData.resetSrc = OSCFAIL_START;  // RA
	}
	/*SAFETYMCUSW 139 S MR:13.7 <APPROVED> "Hardware status bit read check" */
	else if ((SYS_EXCEPTION & WATCHDOG_RESET) !=0U)
	{
		/* Reset caused due
		 *  1) windowed watchdog violation - Add user code here to handle watchdog violation.
		 *  2) ICEPICK Reset - After loading code via CCS / System Reset through CCS
		 */
		/* Check the WatchDog Status register */
		if(WATCHDOG_STATUS != 0U)
		{
			/* Add user code here to handle watchdog violation. */
			startData.resetSrc = WATCHDOG_START;  // RA

			/* Clear the Watchdog reset flag in Exception Status register */
			SYS_EXCEPTION = WATCHDOG_RESET;
		}
		else
		{
			/* Clear the ICEPICK reset flag in Exception Status register */
			SYS_EXCEPTION = ICEPICK_RESET;
			startData.resetSrc = DEBUG_START;
		}
	}
	/*SAFETYMCUSW 139 S MR:13.7 <APPROVED> "Hardware status bit read check" */
	else if ((SYS_EXCEPTION & CPU_RESET) !=0U)
	{
		/* Reset caused due to CPU reset.
        CPU reset can be caused by CPU self-test completion, or
        by toggling the "CPU RESET" bit of the CPU Reset Control Register. */

		startData.resetSrc = CPU_RESET_START;

		/* clear all reset status flags */
		SYS_EXCEPTION = CPU_RESET;
	}
	/*SAFETYMCUSW 139 S MR:13.7 <APPROVED> "Hardware status bit read check" */
	else if ((SYS_EXCEPTION & SW_RESET) != 0U)
	{
		/* Reset caused due to software reset.
		 */
		startData.resetSrc = SOFT_RESET_INTERNAL_START;
	}
	else
	{
		/* Reset caused by nRST being driven low externally.
        Add user code to handle external reset. */

		startData.resetSrc = SOFT_RESET_EXTERNAL_START;
	}


	/* Check if there were ESM group3 errors during power-up.
	 * These could occur during eFuse auto-load or during reads from flash OTP
	 * during power-up. Device operation is not reliable and not recommended
	 * in this case.
	 * An ESM group3 error only drives the nERROR pin low. An external circuit
	 * that monitors the nERROR pin must take the appropriate action to ensure that
	 * the system is placed in a safe state, as determined by the application.
	 */
	if ((esmREG->SR1[2]) != 0U)
	{
		/* USER CODE BEGIN (24) */
		/* USER CODE END */
		/*SAFETYMCUSW 5 C MR:NA <APPROVED> "for(;;) can be removed by adding "# if 0" and "# endif" in the user codes above and below" */
		/*SAFETYMCUSW 26 S MR:NA <APPROVED> "for(;;) can be removed by adding "# if 0" and "# endif" in the user codes above and below" */
		/*SAFETYMCUSW 28 D MR:NA <APPROVED> "for(;;) can be removed by adding "# if 0" and "# endif" in the user codes above and below" */
		for(;;)
		{
		}/* Wait */
		/* USER CODE BEGIN (25) */
		/* USER CODE END */
	}
}

