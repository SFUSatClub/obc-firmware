/*
 * sfu_notification.c
 *
 *  Created on: Feb 19, 2018
 *      Author: Richard
 *
 *      This file contains our modifications to the x_notification functions provided by HALCoGen. Keeping them all in here makes it easy to track our changes.
 */
#include "flash_mibspi.h"
#include "sfu_hardwaredefs.h"
#include "unit_tests/unit_tests.h"
#include "examples/sfusat_examples.h"

void gioNotification(gioPORT_t *port, uint32 bit){
	gio_notification_example(port, bit); // for example
}

void mibspiGroupNotification(mibspiBASE_t *mibspi, uint32 group){
    /* This is the callback from the ISR. We use it to signal that a transfer has completed.
     */
    switch (group){
    case 0 :
        /* Enable TG1 to start, SW Trigger */
        //            mibspiTransfer(mibspiREG1,1);
        TG0_IS_Complete = 0xA5;
        break;
    case 1:
        TG1_IS_Complete = 0xA5;
        break;
    case 2:
        TG2_IS_Complete = 0xA5;
        break;
    case 3:
        TG3_IS_Complete = 0xA5;
        break;
    default :
        while(1);
        break;
    }
}


