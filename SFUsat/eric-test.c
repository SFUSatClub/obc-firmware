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

QueueHandle_t xQueue;
QueueHandle_t xSerialTXQueue;
QueueHandle_t xSerialRXQueue;

void tickleWatchDog(void *pvParameters) { // this is the sanity checker task, blinks LED at 10Hz
	dwwdInit(Generate_NMI, 4095, Size_100_Percent);
	dwdCounterEnable();
}
