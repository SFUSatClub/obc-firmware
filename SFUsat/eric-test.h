/*
 * eric-test.h
 *
 *  Created on: Nov 7, 2017
 *      Author: Eric
 */

#include "sys_common.h"
#include "system.h"

/* USER CODE BEGIN (1) */
#include "rti.h"
#include "het.h"
#include "esm.h"

#include "adc.h"
#include "sys_pmu.h"

#include "sfu_tasks.h"
#include "sfu_cmds.h"
#include "sfu_hardwaredefs.h"

// rtos task tickles the watchdog
void tickleWatchDog(void *pvParameters);
