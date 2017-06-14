/*
 * sfu_scheduler.c
 *
 *  Created on: Mar 26, 2017
 *      Author: steven
 */

#include "sfu_scheduler.h"
#include "sfu_uart.h"
#include "sfu_rtc.h"


void shift(int idx, int dir);

int addEvent(Event_t event) {
	if (schedule.numActiveEvents >= MAX_EVENTS) {
		return -1;
	}
	event.creation_time = getTime();
	event.target_time = event.creation_time + event.seconds_from_now;
	event._status.active = 1;
	int i;
	for (i = 0; i < MAX_EVENTS; i++) {
		if (schedule.events[i]._status.active) {
			if (event.target_time < schedule.events[i].target_time) {
				shift(i, 1);
				schedule.events[i] = event;
				break;
			}
		} else {
			schedule.events[i] = event;
			break;
		}
	}
	schedule.numActiveEvents++;
	return i;
}

int removeEventIdx(int eventIdx) {
	if (eventIdx < 0 || eventIdx >= MAX_EVENTS || !schedule.events[eventIdx]._status.active) {
		return 0;
	}
	schedule.events[eventIdx]._status.active = 0;
	shift(eventIdx + 1, -1);
	schedule.numActiveEvents--;
	return 1;
}

int getAction(CMD_t *action) {
	if (!schedule.events[0]._status.active || schedule.events[0].target_time > getTime()) {
		return 0;
	}
	(*action) = schedule.events[0].action;
	schedule.events[0]._status.active = 0;
	shift(1, -1);
	schedule.numActiveEvents--;
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

/**
 * Shifts all elements from range idx to boundary a certain amount dir.
 * If dir is -1, shifts left by 1, dir is 3, shifts right 3, etc.
 * Elements in original holes will be set inactive after shift operation.
 * Shifts preserve ordering (active elements precede inactive elements).
 * @param idx left bound inclusive
 * @param dir amount to shift
 */
void shift(int idx, int dir) {
	/*
	 * Boundary is the index of the rightmost active event in the array.
	 */
	const int boundary = schedule.numActiveEvents - 1;
	/*
	 * If shifting left, we need to start at the left boundary, stop at right boundary, and increment i.
	 * If shifting right, we need to start at the right boundary, stop at left boundary, and decrement i.
	 * Otherwise, we will overwrite what we want to shift.
	 */
	int i;
	// TODO: make this prettier...
	for (i = dir < 0 ? idx : boundary; dir < 0 ? (i <= boundary) : (i >= idx); i = dir < 0 ? i + 1 : i - 1) {
		const int dest = i + dir;
		if (dest >= 0 && dest < MAX_EVENTS) {
			schedule.events[dest] = schedule.events[i];
		}
		schedule.events[i]._status.active = 0;
	}
}
