/*
 * sfu_scheduler.c
 *
 *  Created on: Mar 26, 2017
 *      Author: steven
 */


#include "sfu_scheduler.h"

int addEvent(Event_t event) {
	if (schedule.numActiveEvents >= MAX_EVENTS) {
		return 0;
	}
	schedule.numActiveEvents++;
	event._status.active = 1;
	int i;
	for (i = 0; i < MAX_EVENTS; i++) {
		if (schedule.events[i]._status.active == 0) {
			schedule.events[i] = event;
			break;
		}
	}
	return 1;
}

Schedule_t schedule = {
		.numActiveEvents = 0,
		.events = {0},
};

