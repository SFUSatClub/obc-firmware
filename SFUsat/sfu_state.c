/*
 * SFU_State.c
 *
 *  Created on: Mar 25, 2017
 *  Updated: Aug 25, 2017
 *      Author: Steven, Richard
 */

#include <stdio.h>

#include "sfu_state.h"
#include "sfu_uart.h"

// State strings defined in the same order as State_t enum allowing us to print the state easily
char const* stateNameString[NUM_STATES]= {
	"STATE_SAFE",
	"STATE_READY",
	"STATE_LOW_POWER"
};

/* -------------- doStateX Functions ------------------------
These handle the checks required to transition from one state to the next.
Return to current state is always last so that checks are performed frequently. */

State_t doStateSafe(InstanceData_t *data) {
	// leave SAFE for READY upon receipt of radio command
	// leave SAFE for LOW_POWER if the power is low

	if (stateCheckLeaveSafe(data)){
		return STATE_READY;
	}
	else if (!stateCheckPowerGood(data)){
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
	// exit LOW_POWER for the previous state if power level is good
	if (stateCheckPowerGood(data)){
		return data->previous_state;
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
	printStateInfo(currstate,data);

	State_t newState = STATE_TABLE[currstate](data);

	TransitionFunc_t *transition = TRANSITION_TABLE[currstate][newState];
	if (transition) {
		transition(data);
		data->previous_state = currstate;
		data->enter_time = 0x32; // dummy value, will actually want to read RTC
		// TODO: confirm state switch out over radio
	}
	state_tick ++;
	data->manual_state_switch = NUM_STATES; // don't change states manually (default)
    return newState;
};

void stateMachineInit(){
	serialSendln("STARTING STATE MACHINE");
	cur_state = STATE_SAFE;
	state_persistent_data.previous_state = STATE_SAFE; // will always start in safe. Then automatically go to low power if necessary.
	state_persistent_data.enter_time = 0x55; // dummy value, will actually want to read RTC.
	state_persistent_data.in_RTOS = 0;
	state_tick = 0;
	state_persistent_data.manual_state_switch = NUM_STATES; // don't change states
}

void setStateRTOS_mode(InstanceData_t *data){
	data->in_RTOS = 1;
	serialSendQ("RTOS ON");

}

void printStateInfo(State_t currstate, InstanceData_t *data){
	if (!data->in_RTOS){
		serialSend("Current state: ");
		serialSendln(strdup(stateNameString[currstate])); // state name
		serialSend("Previous state: ");
		serialSendln(strdup(stateNameString[data->previous_state])); // prev state name
		serialSendln("RTOS OFF");
	}
	else{
		// use the queue if we're under RTOS control
		// just print current state
		serialSendQ(strdup(stateNameString[currstate]));
	}
}
void printPrevState(State_t currstate, InstanceData_t *data){
	if (!data->in_RTOS){
		serialSend("Previous state: ");
		serialSendln(strdup(stateNameString[data->previous_state])); // prev state name
	}
	else{
		// use the queue if we're under RTOS control
		// just print current state
		serialSendQ(strdup(stateNameString[data->previous_state]));
	}
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
		return -1;
	}
}
/* -------------- State Checks -----------------------
These functions query whatever data is required to determine the particular conditions we switch on.
Transition Logic is handled by the doStateX functions above.

Currently the manual override takes precedence over any auto switching functionality, which is probably wise.
*/

uint8_t stateCheckPowerGood(InstanceData_t *data){
	/* Low power is a little different. We want to exit it as soon as possible, so we will only stay in it if this KEEPS returning 0
	 * therefore, setting state MANUALLY to LOW POWER will set it for one tick of the state task, after which it will return to previous state
	 * (unless we perform a real power check and power is actually bad). We should debate whether we should be able to KEEP the spacecraft in low power or not,
	 * right now, we can't. Right now, it is only momentary.
	 */
	switch(data->manual_state_switch){
		case STATE_LOW_POWER:
			// the way the logic is set up, will only enter LOW POWER for one run of the state machine task.
			return 0; // returning 0 here means we enter low power mode. A little backwards but semantic with function name :/
		default:
			// perform actual low power check here. Return 0 = enter low power
			return 1; // don't enter low power
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
uint8_t stateCheckLeaveSafe(InstanceData_t *data){
	switch(data->manual_state_switch){
		case STATE_READY:
			return 1;
		case STATE_LOW_POWER:
			return 1;
		default:
			return 0; // stay in safe
	}
	// no errors therefore leave safe
}

//
//int main( void ) {
//    state_t cur_state = STATE_INITIAL;
//    instance_data_t data;
//
//    while ( 1 ) {
//        cur_state = run_state( cur_state, &data );
//
//        // do other program logic, run other state machines, etc
//    }
//}

