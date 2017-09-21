/*
 * sfu_triumf.c
 *
 *  Created on: Sep 21, 2017
 *      Author: Richard
 */

#include "sfu_triumf.h"
#include "sfu_uart.h"
void triumf_init(){
	  if(flash_test_JEDEC()){
	    	serialSendln("Passed flash JEDEC test!");
	    }

#if ERASE_START
	    flash_erase_chip();
	    addressWritten = getEmptySector();
	    lastRead = addressWritten;

#endif

#if !ERASE_START
	    addressWritten = getEmptySector();
	    lastRead = addressWritten;
#endif



}


