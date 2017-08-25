/*
 * SFU_State.h
 *
 *  Created on: Mar 25, 2017
 *      Author: steven
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
	int x;
} InstanceData_t;

typedef State_t StateFunc_t(InstanceData_t *data);
typedef void TransitionFunc_t(InstanceData_t *data);

extern StateFunc_t* const STATE_TABLE[NUM_STATES];
extern TransitionFunc_t * const TRANSITION_TABLE[NUM_STATES][NUM_STATES];

void do_SAFE_to_READY(InstanceData_t *data);
void do_SAFE_to_LOWPOWER(InstanceData_t *data);
void do_READY_to_SAFE(InstanceData_t *data);
void do_READY_to_LOWPOWER(InstanceData_t *data);
void do_LOWPOWER_to_SAFE(InstanceData_t *data);
void do_LOWPOWER_to_READY(InstanceData_t *data);

void stateMachineInit(void);

State_t cur_state;
volatile InstanceData_t state_persistent_data; // contains things such as the previous state.

uint8_t stateCheckPowerGood(void);
uint8_t stateCheckEnterSafe(void); // enter safe on some large error or from ground command
uint8_t stateCheckLeaveSafe(void); // leave safe from ground command



#endif /* SFUSAT_SFU_STATE_H_ */
