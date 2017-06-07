/*
 * sfu_scheduler.h
 *
 *  Created on: Mar 26, 2017
 *      Author: steven
 */

#ifndef SFUSAT_SFU_SCHEDULER_H_
#define SFUSAT_SFU_SCHEDULER_H_

/**
 * Bit field flags for various event states.
 */
typedef struct EventStatus {
	/*
	 * An event is active if its its arguments are valid and it's ready to be invoked.
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
	 * Time when event is created.
	 */
	unsigned int creation_time;
	/**
	 * Time when action should be invoked.
	 */
	unsigned int target_time;
	/**
	 * The action (function pointer) to invoke once target_time is reached.
	 */
	void *action;

	/**
	 * Used internally by the scheduler.
	 */
	EventStatus_t _status;
} Event_t;

/**
 * Keeps track of events.
 */
#define MAX_EVENTS 15
typedef struct Schedule {
	int numActiveEvents;
	Event_t events[MAX_EVENTS];
} Schedule_t;

extern Schedule_t schedule;

/**
 * Adds an event to the schedule.
 * Copies the event to the first free (inactive) index by scanning from 0 to MAX_EVENTS - 1.
 * @param event the event to add
 * @return 0 if event is not added to the schedule, 1 if event is added to the schedule.
 */
int addEvent(Event_t event);

#endif /* SFUSAT_SFU_SCHEDULER_H_ */
