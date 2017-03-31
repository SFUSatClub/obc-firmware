/*
 * SFU_State.c
 *
 *  Created on: Mar 25, 2017
 *      Author: steven
 */

#include <sfu_state.h>

State_t doStatePrimed(InstanceData_t *data) {
	return STATE_PRIMED;
}

State_t doStateReady(InstanceData_t *data) {
	return STATE_READY;
}

State_t doStateLowPower(InstanceData_t *data) {
	return STATE_LOW_POWER;
}

StateFunc_t* const STATE_TABLE[NUM_STATES] = {
	doStatePrimed,
	doStateReady,
	doStateLowPower
};

void do_PRIMED_to_READY(InstanceData_t *data);
void do_READY_to_LOWPOWER(InstanceData_t *data);
void do_LOWPOWER_to_PRIMED(InstanceData_t *data);
void do_LOWPOWER_to_READY(InstanceData_t *data);
void do_LOWPOWER_to_LOWPOWER(InstanceData_t *data);

/**
 * do_X_to_Y
 * rows: X
 * cols: Y
 */
TransitionFunc_t * const TRANSITION_TABLE[NUM_STATES][NUM_STATES] = {
    { NULL,              		do_PRIMED_to_READY, 	NULL },
    { NULL,              		NULL,              		do_READY_to_LOWPOWER },
    { do_LOWPOWER_to_PRIMED, 	do_LOWPOWER_to_READY,	do_LOWPOWER_to_LOWPOWER }
};

State_t runState(State_t currstate, InstanceData_t *data) {
	State_t newState = STATE_TABLE[currstate](data);

	TransitionFunc_t *transition = TRANSITION_TABLE[currstate][newState];
	if (transition) {
		transition(data);
	}

    return newState;
};

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

