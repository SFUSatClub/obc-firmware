/*
 * SFU_State.h
 *
 *  Created on: Mar 25, 2017
 *      Author: steven
 *      Updated: Aug 25 ish, Richard
 */

#ifndef SFUSAT_SFU_STATE_H_
#define SFUSAT_SFU_STATE_H_

#include "sys_common.h"


typedef enum {
	STATE_SAFE,
	STATE_READY,
	STATE_LOW_POWER,
	NUM_STATES
} State_t;

extern char const* stateNameString[NUM_STATES]; // this should match above, excluding NUM_STATES

uint32_t state_tick; // for testing

typedef struct Instance_Data {
	State_t previous_state; // keep this so we can correctly go to the last state
	uint32_t enter_time; // time we enter current state. Use with current time to determine how long we've been in a state
	uint8_t in_RTOS; // for printing, we need to keep track of whether we're in the RTOS or not
	uint8_t manual_state_switch; // set this via the command interface to manually change states. NUM_STATES = don't change states. Other = state index (same as state table)
} InstanceData_t;

typedef State_t StateFunc_t(InstanceData_t *data);
typedef void TransitionFunc_t(InstanceData_t *data);

extern StateFunc_t* const STATE_TABLE[NUM_STATES];
extern TransitionFunc_t * const TRANSITION_TABLE[NUM_STATES][NUM_STATES];
State_t runState(State_t currstate, InstanceData_t *data);

void do_SAFE_to_READY(InstanceData_t *data);
void do_SAFE_to_LOWPOWER(InstanceData_t *data);
void do_READY_to_SAFE(InstanceData_t *data);
void do_READY_to_LOWPOWER(InstanceData_t *data);
void do_LOWPOWER_to_SAFE(InstanceData_t *data);
void do_LOWPOWER_to_READY(InstanceData_t *data);

void stateMachineInit(void);
void printStateInfo(State_t currstate, InstanceData_t *data);
void setStateRTOS_mode(InstanceData_t *data);
void printPrevState(State_t currstate, InstanceData_t *data);
uint8_t setStateManual(InstanceData_t *data,  uint8_t state_to_set);

State_t cur_state;
InstanceData_t state_persistent_data; // contains things such as the previous state. REVIEW: IS THIS THE BEST PLACE TO CREATE THIS? WE WANT IT TO STICK AROUND.

uint8_t stateCheckPowerGood(InstanceData_t *data);
uint8_t stateCheckEnterSafe(InstanceData_t *data); // enter safe on some large error or from ground command



#endif /* SFUSAT_SFU_STATE_H_ */
