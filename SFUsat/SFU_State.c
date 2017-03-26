/*
 * SFU_State.c
 *
 *  Created on: Mar 25, 2017
 *      Author: steven
 */

#include "SFU_State.h"

State_t do_state_initial(InstanceData_t *data) {
	return STATE_INITIAL;
}

State_t do_state_foo(InstanceData_t *data) {
	return STATE_FOO;
}

State_t do_state_bar(InstanceData_t *data) {
	return STATE_BAR;
}

State_Func_t* const STATE_TABLE[NUM_STATES] = {
	do_state_initial,
	do_state_foo,
	do_state_bar
};

void do_initial_to_foo(InstanceData_t *data);
void do_foo_to_bar(InstanceData_t *data);
void do_bar_to_initial(InstanceData_t *data);
void do_bar_to_foo(InstanceData_t *data);
void do_bar_to_bar(InstanceData_t *data);

TransitionFunc_t * const TRANSITION_TABLE[NUM_STATES][NUM_STATES] = {
    { NULL,              do_initial_to_foo, NULL },
    { NULL,              NULL,              do_foo_to_bar },
    { do_bar_to_initial, do_bar_to_foo,     do_bar_to_bar }
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

