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

typedef State_t State_Func_t(InstanceData_t *data);
typedef void TransitionFunc_t(InstanceData_t *data);


#endif /* SFUSAT_SFU_STATE_H_ */
