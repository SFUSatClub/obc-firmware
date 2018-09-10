/*
 * sfu_scheduler.h
 *
 *  Created on: Mar 26, 2017
 *      Author: steven
 */

#ifndef SFUSAT_OBC_SCHEDULER_H_
#define SFUSAT_OBC_SCHEDULER_H_

#include "obc_cmds.h"

/**
 * Bit field flags for various event states.
 */
typedef struct EventStatus {
	/*
	 * An event is active if its arguments are valid and it's ready to be invoked.
	 * An event is flagged active (set to 1) once it is added to the scheduler.
	 * An event is flagged inactive (set to 0) after it has been invoked.
	 */
	unsigned int active : 1;
	/*
	 * An event is fired if the event's action has just been invoked.
	 */
	unsigned int fired : 1;
} EventStatus_t;

/**
 * An event.
 */
typedef struct Event {
	/**
	 * Only either seconds_from_now or target_time are ever used at the same time, so they are made unions.
	 * This saves memory, in addition to providing more semantic meaning when conditions change.
	 *
	 * seconds_from_now
	 * 		- Seconds from now when action should be invoked.
	 * 		- Events are always assumed to be initialized with this variable first.
	 * 		- This variable loses semantic meaning after initialization (will have value of target_time).
	 *
	 * target_time
	 * 		- Once initialized with seconds_from_now, the scheduler will convert this value to a target time.
	 * 		  This is done by retrieving the current time from the RTC and adding to it seconds_from_now.
	 * 		- This variable only has semantic meaning after initialization.
	 */
	union {
		unsigned int seconds_from_now;
		unsigned int target_time;
	};

	/**
	 * The action (command) to invoke once target_time is reached.
	 */
	CMD_t action;

	/**
	 * Time when event is created.
	 * Set automatically by the scheduler.
	 */
	unsigned int creation_time;
	/**
	 * Used internally by the scheduler.
	 */
	EventStatus_t _status;
} Event_t;

/**
 * Keeps track of events.
 * Events are added/removed rarely relative to how often they are scanned, so
 * improving the scanning speed will be good for performance.
 *
 * This is done by sorting the events array when events are added/removed/fired.
 *
 * When the events array is sorted...
 * 		- all active events precede (have lower indices than) inactive events
 * 		- all active events are sorted on their target times from least to greatest
 */
#define MAX_EVENTS 15
typedef struct Schedule {
	/*
	 * Index at (numActiveEvents - 1) demarks the rightmost active event.
	 * Indices after this point are either inactive events or past array bounds.
	 */
	int numActiveEvents;
	Event_t events[MAX_EVENTS];
} Schedule_t;

extern Schedule_t schedule;

/**
 * Adds an event to the schedule.
 * Copies the event to the first free (inactive) index by scanning from 0 to MAX_EVENTS - 1.
 * Returns the index of the added event on success, -1 otherwise.
 * @param event the event to add
 * @return the index of the event if event is added to the schedule, -1 if failed to add event.
 */
int addEvent(Event_t event);

/**
 * Adds an event to the schedule.
 * Same function as addEvent, but accepts a CMD_SCHED command to facilitate direct assignment into
 * the destination event within the scheduler (constructs less objects, so uses less stack).
 * @param cmd the command that did the scheduling (that itself contains the command to be scheduled)
 * @return the index of the event if event is added to the schedule, -1 if failed to add event.
 */
int addEventFromScheduledCommand(const CMD_t *cmd);

/**
 * Removes an event from the schedule.
 * @param eventIdx index of event to remove
 * @return 1 if event is removed from the schedule, 0 if failed to remove event.
 */
int removeEventIdx(int eventIdx);

/**
 * An event is fired when its target time >= current time.
 * When this happens, set action to the fired event's action.
 * Otherwise, action is not modified.
 * @param action
 * @return 1 if target_time reached and action has been populated, 0 otherwise.
 */
int getAction(CMD_t *action);

void showActiveEvents();

#endif /* SFUSAT_OBC_SCHEDULER_H_ */
