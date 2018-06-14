/*
 * deployables.c
 *
 *  Created on: May 7, 2018
 *      Author: Richard
 */

#include "deployables.h"
#include "sfu_rtc.h"
#include "sfu_uart.h"
#include "FreeRTOS.h"
#include "rtos_task.h"

#define DEPLOY_PREARM 0xAA		/* this is how we start */
#define DEPLOY_DISARM 0xBB		/* set this from the ground */
#define DEPLOY_ARMED 0xCC		/* danger mode. this will shoot things out */
#define DEPLOY_SUCCESS	0x55

static uint8_t deploy_arm_status = DEPLOY_PREARM;
TaskHandle_t deployTaskHandle;

/* deployment sequence functions */
uint16_t deploy();				/* execute the deploy sequence */
void deploy_current_enable();
void deploy_current_disable();
uint8_t deploy_plusZ();
uint8_t deploy_minusZ();
uint8_t deploy_read(); 			/* read the status of the deploy switches */


/* deploy task
 * 	wait around until the DEPLOY_DELAY timeout expires
 * 		- this is for DIETR compliance (30 minutes)
 * 		- setting DEPLOY_TESTMODE in hwdefs will make this 2 minutes instead
 *
 * 	when timeout expires, execute deployment if we haven't received a manual inhibit command. Then delete the task.
 *
 * 	This will run each time we start up (from a total power loss). However, by reading deployment status, we only burn
 * 	if we actually need to.
 */
void deploy_task(void *pvParameters){
	deploy_current_disable();
	uint32_t start_time = getCurrentTime();
	while(1){
		while(getCurrentTime() < (start_time + DEPLOY_DELAY)){		/* wait the DIETR-mandated time */
			serialSendQ("Deploy check");
			vTaskDelay(pdMS_TO_TICKS(DEPLOY_CHECK_INTERVAL));
		}

		if(deploy_arm_status == DEPLOY_PREARM){
			deploy_arm_status = DEPLOY_ARMED;	/* only ARM if we haven't manually DISARM'ed */
			deploy();
		}
		else{
			serialSendQ("Deploy DISARMED: deploy trigger time.");
		}
		vTaskDelete(NULL);	/* delete the task since we're all good and deployed */
	}
}

/* deploy
 * - the deployment sequence
 */

uint16_t deploy(){
	if(deploy_arm_status != DEPLOY_DISARM && deploy_arm_status != DEPLOY_PREARM){
		if(deploy_plusZ() != DEPLOY_SUCCESS){
			vTaskDelay(pdMS_TO_TICKS(1000));	/* cool down */
			deploy_plusZ(); 					/* try once more */
		}
		if(deploy_minusZ() != DEPLOY_SUCCESS){
			vTaskDelay(pdMS_TO_TICKS(1000));	/* cool down */
			deploy_minusZ();
		}
	}
	deploy_inhibit();
	return 0;
}

/* deploy_inhibit
 * - mapped to the 'deploy disarm' command, used to disable actual deployment but still show the correct timeline over UART.
 * - for bench testing so we don't lose our fingers
 */
uint16_t deploy_inhibit(){
	deploy_current_disable();
	deploy_arm_status = DEPLOY_DISARM;
	serialSendQ("Deployment DISARMED.");
	return 0;
}

void deploy_current_enable(){
	gioSetBit(DEPLOY_EN_PORT, DEPLOY_EN_PIN, 1); /* active high */
}

void deploy_current_disable(){
	gioSetBit(DEPLOY_EN_PORT, DEPLOY_EN_PIN, 0);
}

/* deploy read
 * - check the status of the deployments
 */

uint8_t deploy_read(){
	uint8_t res;
	res = 0;
	res = gioGetBit(RF_DEPLOY0_PORT, RF_DEPLOY0_PIN) | (gioGetBit(RF_DEPLOY1_PORT, RF_DEPLOY1_PIN) << 1) | (gioGetBit(RF_DEPLOY2_PORT, RF_DEPLOY2_PIN) << 2);
	return res;
}

uint8_t deploy_plusZ(){
	if(deploy_arm_status != DEPLOY_DISARM && deploy_arm_status != DEPLOY_PREARM){		/* deploy is armed */
		serialSendQ("DEPLOY plusZ");
		uint32_t deploy_start = getCurrentTime();
		gioSetBit(DEPLOY_SELECT_PORT, DEPLOY_SELECT_PIN, 0);	/* set the deploy side */
		deploy_current_enable();

		/* while any or both of the deploy sensors are high (not deployed) and timeout has not occurred */
		while(((deploy_read() & 0x03) != 0) && (getCurrentTime() < DEPLOY_MAX_BURN_SECONDS + deploy_start)){ /* check bits 0 and 1 (deploy sensors 0 and 1) */
			vTaskDelay(pdMS_TO_TICKS(500));
		}

		deploy_current_disable();		/* stop burning */

		switch(deploy_read() & 0x03){
			case 0:
				serialSendQ("DEPLOY plusZ successful!");
				return DEPLOY_SUCCESS;
			case 1:
				serialSendQ("RF_DEPLOY0 failed");
				return 0;
			case 2:
				serialSendQ("RF_DEPLOY1 failed");
				return 2;
			default:
				serialSendQ("+Z DEPLOY ERROR");
				return 3;
		}
	}
	return DEPLOY_SUCCESS;
}
uint8_t deploy_minusZ(){
	if(deploy_arm_status != DEPLOY_DISARM && deploy_arm_status != DEPLOY_PREARM){		/* deploy is armed */
			serialSendQ("DEPLOY minusZ");
			uint32_t deploy_start = getCurrentTime();
			gioSetBit(DEPLOY_SELECT_PORT, DEPLOY_SELECT_PIN, 1);	/* set the deploy side */
			deploy_current_enable();

			/* while any or both of the deploy sensors are high (not deployed) and timeout has not occurred */
			while(((deploy_read() & 0x04) != 0) && (getCurrentTime() < DEPLOY_MAX_BURN_SECONDS + deploy_start)){ /* check bit 2 (deploy sensor 2) */
				vTaskDelay(pdMS_TO_TICKS(500));
			}

			deploy_current_disable();		/* stop burning */

			switch(deploy_read() & 0x04){
				case 0:
					serialSendQ("DEPLOY minusZ successful!");
					return DEPLOY_SUCCESS;
				case 4:
					serialSendQ("RF_DEPLOY2 failed");
					return 4;
				default:
					serialSendQ("-Z DEPLOY ERROR");
					return 3;
			}
	}
	return DEPLOY_SUCCESS;
}

