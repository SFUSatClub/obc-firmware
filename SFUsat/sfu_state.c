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

	if (stateCheckLeaveSafe()){
		return STATE_READY;
	}
	else if (!stateCheckPowerGood()){
		return STATE_LOW_POWER;
	}
	return STATE_SAFE;
}

State_t doStateReady(InstanceData_t *data) {
	// leave READY for SAFE if something is wrong. Do this first since we can enter LOW_POWER from SAFE
	// leave READY for LOW_POWER if required
	if (stateCheckEnterSafe()){
		return STATE_SAFE;
	}
	else if (!stateCheckPowerGood()){
		return STATE_LOW_POWER;
	}
	return STATE_READY;
}

State_t doStateLowPower(InstanceData_t *data) {
	// exit LOW_POWER for the previous state if power level is good
	if (stateCheckPowerGood()){
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
	serialSend("Current state: ");
	serialSendln(strdup(stateNameString[currstate])); // state name
	serialSend("Previous state: ");
	serialSendln(strdup(stateNameString[data->previous_state])); // prev state name

	State_t newState = STATE_TABLE[currstate](data);

	TransitionFunc_t *transition = TRANSITION_TABLE[currstate][newState];
	if (transition) {
		transition(data);
		data->previous_state = currstate;
	}
	state_tick ++;
    return newState;
};

void stateMachineInit(){
	serialSendln("STARTING STATE MACHINE");
	cur_state = STATE_SAFE;
	state_persistent_data.previous_state = STATE_SAFE;
	state_tick = 0;
}

/* -------------- State Checks -----------------------
These functions query whatever data is required to determine the particular conditions we switch on.
Transition Logic is handled by the doStateX functions above. */

uint8_t stateCheckPowerGood(){
	return 1; // power is good
}
uint8_t stateCheckEnterSafe(){
	return 0; // no need to enter SAFE mode
}
uint8_t stateCheckLeaveSafe(){
	return 1; // just run in READY mode (therefore leave SAFE immediately)
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

