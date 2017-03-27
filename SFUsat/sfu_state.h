/*
 * SFU_State.h
 *
 *  Created on: Mar 25, 2017
 *      Author: steven
 */

#ifndef SFUSAT_SFU_STATE_H_
#define SFUSAT_SFU_STATE_H_

#include <stdio.h>

typedef enum {
	STATE_INITIAL,
	STATE_FOO,
	STATE_BAR,
	NUM_STATES
} State_t;

typedef struct Instance_Data {
	int x;
} InstanceData_t;

typedef State_t StateFunc_t(InstanceData_t *data);
typedef void TransitionFunc_t(InstanceData_t *data);

extern StateFunc_t* const STATE_TABLE[NUM_STATES];
extern TransitionFunc_t * const TRANSITION_TABLE[NUM_STATES][NUM_STATES];

#endif /* SFUSAT_SFU_STATE_H_ */
