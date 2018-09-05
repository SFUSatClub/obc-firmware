/*
 * SFU_State.c
 *
 *  Created on: Mar 25, 2017
 *  Updated: Aug 25, 2017
 *      Author: Steven, Richard
 */

#include "obc_rtc.h"
#include "obc_state.h"
#include "obc_uart.h"
//#include <stdio.h>


State_t cur_state;
InstanceData_t state_persistent_data;
/* -------------- doStateX Functions ------------------------
These handle the checks required to transition from one state to the next.
Return to current state is always last so that checks are performed frequently. */

State_t doStateSafe(InstanceData_t *data) {
	// leave SAFE for READY upon receipt of radio command
	// leave SAFE for LOW_POWER if the power is low

	switch(data->manual_state_switch){
		case STATE_READY:
			return STATE_READY;
		case STATE_LOW_POWER:
			return STATE_LOW_POWER;
	}

	if (!stateCheckPowerGood(data)){
		return STATE_LOW_POWER;
	}
	return STATE_SAFE;
}

State_t doStateReady(InstanceData_t *data) {
	// leave READY for SAFE if something is wrong. Do this first since we can enter LOW_POWER from SAFE
	// leave READY for LOW_POWER if required
	if (stateCheckEnterSafe(data)){
		return STATE_SAFE;
	}
	else if (!stateCheckPowerGood(data)){
		return STATE_LOW_POWER;
	}
	return STATE_READY;
}

State_t doStateLowPower(InstanceData_t *data) {
	// for testing: leave state if requested via command
	// TODO: for flight: leave state if power is ok again
	switch(data->manual_state_switch){
			case STATE_READY:
				return STATE_READY;
			case STATE_SAFE:
				return STATE_SAFE;
		}

	return STATE_LOW_POWER;
}

// ----------------------- State machine core ---------------------------

StateFunc_t* const STATE_TABLE[NUM_STATES] = {
	doStateSafe,
	doStateReady,
	doStateLowPower
};

void do_SAFE_to_READY(InstanceData_t *data){}
void do_SAFE_to_LOWPOWER(InstanceData_t *data){}
void do_READY_to_SAFE(InstanceData_t *data){}
void do_READY_to_LOWPOWER(InstanceData_t *data){}
void do_LOWPOWER_to_SAFE(InstanceData_t *data){}
void do_LOWPOWER_to_READY(InstanceData_t *data){}

/**
 * do_X_to_Y
 * rows: X
 * cols: Y
 */
TransitionFunc_t * const TRANSITION_TABLE[NUM_STATES][NUM_STATES] = {
    { NULL,              		do_SAFE_to_READY, 		do_SAFE_to_LOWPOWER },
    { do_READY_to_SAFE,     		NULL,            		do_READY_to_LOWPOWER },
    { do_LOWPOWER_to_SAFE, 		do_LOWPOWER_to_READY,	NULL }
};


State_t runState(State_t currstate, InstanceData_t *data) {
	State_t newState = STATE_TABLE[currstate](data);

	TransitionFunc_t *transition = TRANSITION_TABLE[currstate][newState];
	if (transition) {
		printStateInfo(currstate,data);
		transition(data);
		data->previous_state = currstate;
		data->enter_time = getCurrentTime(); // dummy value, will actually want to read RTC
		// TODO: confirm state switch out over radio
	}

	data->manual_state_switch = NUM_STATES; // don't change states manually (default)
    return newState;
};

void stateMachineInit(){
//	serialSendln("STARTING STATE MACHINE");
	cur_state = STATE_SAFE;
	state_persistent_data.previous_state = STATE_SAFE; // will always start in safe. Then automatically go to low power if necessary.
	state_persistent_data.enter_time = getCurrentTime();
	state_persistent_data.in_RTOS = 0;
	state_persistent_data.manual_state_switch = NUM_STATES; // don't change states
}

void setStateRTOS_mode(){
	state_persistent_data.in_RTOS = 1;
	serialSendQ("RTOS ON");
}
bool getStateRTOS_mode(){
	return state_persistent_data.in_RTOS;
}

void printStateInfo(State_t currstate, InstanceData_t *data){
	if (!data->in_RTOS){
		serialSend("Current state: ");
		serialSendln(stateNameString[currstate]);
		serialSend("Previous state: ");
		serialSendln(stateNameString[data->previous_state]);
		serialSendln("RTOS OFF");
	}
	else{
		// use the queue if we're under RTOS control
		serialSendQ(stateNameString[currstate]);
	}
}
void printPrevState(State_t currstate, InstanceData_t *data){
	if (!data->in_RTOS){
		serialSend("Previous state: ");
		serialSendln(stateNameString[data->previous_state]); // prev state name
	}
	else{
		// use the queue if we're under RTOS control
		// just print current state
		serialSendQ(stateNameString[data->previous_state]);
	}
}

void printStateEntryTime(){
	char buf[30] = {'\0'};
	snprintf(buf, 30, "ENTER TIME: %i",stateEntryTime());
	serialSendQ((const char *)buf);
}

uint8_t setStateManual(InstanceData_t *data, uint8_t state_to_set){
	if (state_to_set < NUM_STATES){
		data->manual_state_switch = state_to_set;
		return state_to_set;
	}
	else{
		if (!data->in_RTOS){
			serialSendln("Invalid switch state command.");
		}
		else{
			serialSendQ("Invalid switch state command.");
		}
		return NUM_STATES;
	}
}

/* -------------- State Checks -----------------------
These functions query whatever data is required to determine the particular conditions we switch on.
Transition Logic is handled by the doStateX functions above.

Currently the manual override takes precedence over any auto switching functionality, which is probably wise.
*/

uint8_t stateCheckPowerGood(InstanceData_t *data){
	switch(data->manual_state_switch){
		case STATE_LOW_POWER:
			return 0; // returning 0 here means we enter low power mode. A little backwards but semantic with function name :/
		default:
			// perform actual low power check here. Return 0 = enter low power
			return 1; // don't enter low power since we aren't actually checking for it yet
	}
}
uint8_t stateCheckEnterSafe(InstanceData_t *data){
	switch(data->manual_state_switch){
		case STATE_SAFE:
			return 1;
		default:
			return 0;
	}
	// check for bad errors. If so, enter safe as well.
}


/* ------------ Wrappers ----------------------
 * 	- provide nice interfaces to getting some state information
 */

uint32_t stateEntryTime(){
	// return the time we enter the current state
	return state_persistent_data.enter_time;
}



