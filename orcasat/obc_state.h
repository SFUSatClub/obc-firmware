/*
 * SFU_State.h
 *
 *  Created on: Mar 25, 2017
 *      Author: steven
 *      Updated: Aug 25 ish, Richard
 */

#ifndef SFUSAT_OBC_STATE_H_
#define SFUSAT_OBC_STATE_H_

#include "sys_common.h"
#include "FreeRTOS.h"
#include "obc_utils.h"
#include "rtos_task.h"

extern TaskHandle_t xStateEntryHandle;

// create the list of state enums, and strings so we can print out the state. https://stackoverflow.com/questions/9907160/how-to-convert-enum-names-to-string-in-c
#define FOREACH_STATE(state) \
        state(STATE_SAFE)   \
        state(STATE_READY)  \
        state(STATE_LOW_POWER)   \
        state(NUM_STATES)  \

typedef enum {
    FOREACH_STATE(GENERATE_ENUM)
} State_t;

static const char *stateNameString[] = {
    FOREACH_STATE(GENERATE_STRING)
};

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
void setStateRTOS_mode();
void printPrevState(State_t currstate, InstanceData_t *data);
uint8_t setStateManual(InstanceData_t *data,  uint8_t state_to_set);
bool getStateRTOS_mode();
uint32_t stateEntryTime();
void printStateEntryTime();
extern State_t cur_state;
extern InstanceData_t state_persistent_data;


uint8_t stateCheckPowerGood(InstanceData_t *data);
uint8_t stateCheckEnterSafe(InstanceData_t *data); // enter safe on some large error or from ground command


#endif /* SFUSAT_OBC_STATE_H_ */
