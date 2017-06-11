/*
 * sfu_scheduler.c
 *
 *  Created on: Mar 26, 2017
 *      Author: steven
 */

#include "sfu_scheduler.h"
#include "sfu_uart.h"

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

int removeEvent(int eventIdx) {
	if (eventIdx < 0 || eventIdx >= MAX_EVENTS || !schedule.events[eventIdx]._status.active) {
		return 0;
	}
	schedule.numActiveEvents--;
	schedule.events[eventIdx]._status.active = 0;
	return 1;
}

void showActiveEvents() {
    char buffer[32] = {0};
    int i;
    for(i = 0; i < MAX_EVENTS; i++) {
    	const Event_t e = schedule.events[i];
    	if (e._status.active) {
        	snprintf(buffer, 32, "#%d ctime: %d ttime: %d", i, e.creation_time, e.target_time);
        	serialSendln(buffer);
    	}
    }
}

Schedule_t schedule = {
		.numActiveEvents = 0,
		.events = {0},
};

