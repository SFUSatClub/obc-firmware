/*
 * sfu_triumf.c
 *
 *  Created on: Sep 21, 2017
 *      Author: Richard
 *
 *      Code that deals specifically with the test at TRIUMF.
 *      Primarily, set whether we want to read through the attached flash and start writing in empty space, or just
 *      erase it every time we start up.
 */

#include "sfu_triumf.h"
#include "sfu_uart.h"
void test_triumf_init(){
	  if(flash_test_JEDEC()){
	    	serialSendln("Passed flash JEDEC test!");
	    }

#if ERASE_START
	    flash_erase_chip();
	    addressWritten = 0;
	    lastRead = addressWritten;

#endif

#if !ERASE_START
	    addressWritten = getEmptySector();
	    lastRead = addressWritten;
#endif



}


