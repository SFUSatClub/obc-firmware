/* Contains the available command line arguments
 *
 * Created: May 30 2017
 * Refactored code from sfu_uart.* by Seleena
 * State command added by Richard
 *
 * */

#ifndef SFUSAT_SFU_CMD_LINE_H_
#define SFUSAT_SFU_CMD_LINE_H_

#include <stdint.h>
#include "map.h"
#include "sfu_state.h" // so we can have the state enum


/**
 * Magic numbers to identify a command.
 * - Definition order does not matter.
 * - Exact values do not matter, only that:
 *		- they are unique among commands
 *		- 0xFF is not used (reserved for CMD_UNDEFINED to denote an error, e.g., in determining a cmd id)
 * - Must fit within a uint8_t (0x00 to 0xFF).
 */
#define CMD_HELP		0x00
#define CMD_GET			0x02
#define CMD_EXEC		0x04
#define CMD_RF			0x06
#define CMD_TASK		0x08
#define CMD_SCHED		0x0A
#define CMD_STATE		0x0C

/**
 * Magic numbers to identify the sub-commands of a command.
 * - Definition order does not matter.
 * - Exact value does not matter, only that:
 *		- they are unique among sub-commands
 *		- 0xFF is not used (reserved for CMD_UNDEFINED to denote an error, e.g., in determining a cmd id)
 *		- 0x00 is used for default behavior (e.g., when a command is called with no sub-command, subcmd id 0x00 will be used)
 * - Must fit within a uint8_t (0x00 to 0xFF).
 */
#define CMD_DEFAULT			0x00
#define CMD_UNDEFINED		0xFF

#define CMD_HELP_NONE		0x00
#define CMD_HELP_GET		0x02
#define CMD_HELP_EXEC		0x04
#define CMD_HELP_RF			0x06
#define CMD_HELP_TASK		0x08
#define CMD_HELP_SCHED		0x0A
#define CMD_HELP_STATE		0x0C

#define CMD_GET_NONE		0x00
#define CMD_GET_TASKS		0x02
#define CMD_GET_RUNTIME		0x04
#define CMD_GET_HEAP		0x06
#define CMD_GET_MINHEAP		0x08
#define CMD_GET_TYPES		0x0A

#define CMD_EXEC_NONE		0x00
#define CMD_EXEC_RADIO		0x02

#define CMD_RF_NONE			0x00
#define CMD_RF_TX			0x02

#define CMD_TASK_NONE		0x00
#define CMD_TASK_CREATE		0x02
#define CMD_TASK_DELETE		0x04
#define CMD_TASK_RESUME		0x06
#define CMD_TASK_SUSPEND	0x08
#define CMD_TASK_STATUS		0x0A
#define CMD_TASK_SHOW		0x0C

#define CMD_SCHED_NONE		0x00
#define CMD_SCHED_ADD		0x02
#define CMD_SCHED_REMOVE	0x04
#define CMD_SCHED_SHOW		0x06

#define CMD_STATE_NONE		0x00
#define CMD_STATE_GET		0x02
#define CMD_STATE_PREV		0x04
#define CMD_STATE_SET		0x06


/**
 * Maximum command argument size.
 * The number of bytes in particular is determined by being...
 * 		- sufficiently large enough to accommodate most demanding command.
 * 		- maximized to next word alignment to reduce slop in CMD_t/CMD_SCHED_DATA_t
 * 		and increase effective argument size at no additional memory cost.
 */
#define CMD_DATA_MAX_SIZE (14)

typedef enum TASK_IDS {
	TASK_MAIN,
	TASK_RADIO,
	TASK_TICKLE,
	/**
	 * The following tasks are used only in development and not in release.
	 * They may be #ifdef'd out or removed in the future.
	 */
	TASK_SERIAL,
	TASK_BLINKY,
	TASK_NUM_IDS,
} TASK_ID;

typedef struct CMD_TASK_DATA {
	TASK_ID task_id : 4;
	uint8_t unused[CMD_DATA_MAX_SIZE - 1];
} CMD_TASK_DATA_t;


typedef struct CMD_STATE_DATA {
	State_t state_id : 4;	// number of bits
	uint8_t unused[CMD_DATA_MAX_SIZE - 1];
} CMD_STATE_DATA_t;

/**
 * CMD_SCHED_MISC_DATA_t provides structured access to miscellaneous data when specifying a CMD_SCHED_DATA_t.
 * CMD_SCHED_MISC_DATA_t is only ever used in this situation.
 *
 * When the sub-command of CMD_SCHED_DATA_t is ADD, the scheduled_cmd_data field will be reserved for the
 * arguments of the scheduled command. This struct, CMD_SCHED_MISC_DATA_t, will therefore be inapplicable
 * in this case.
 *
 * When the sub-command of CMD_SCHED_DATA_t is not ADD (e.g., REMOVE, etc), this struct will be used to
 * type-pun scheduled_cmd_data into the following fields.
 */
typedef struct CMD_SCHED_MISC_DATA {
	uint8_t event_idx_to_remove : 4;
	uint8_t unused[CMD_DATA_MAX_SIZE - 1];
} CMD_SCHED_MISC_DATA_t;

/**
 * Must match structure shape of CMD_t, with the following exceptions...
 * 		- CMD_SCHED_DATA_t must not be self-referential. That is to say, there must
 * 		not be scheduling-related fields in the cmd_data and subcmd_id unions.
 * 		Consequently, commands that itself schedule other commands are not supported.
 * 		- cmd_data union size is CMD_SIZE_DATA.
 * 		- there is a time field.
 *
 * Current size: 20 bytes
 * Current slop: 0 bytes
 */
typedef struct CMD_SCHED_DATA {
	union {
		uint8_t scheduled_cmd_data[CMD_DATA_MAX_SIZE];
		CMD_TASK_DATA_t scheduled_cmd_task_data;
		CMD_STATE_DATA_t scheduled_cmd_state_data;
		CMD_SCHED_MISC_DATA_t cmd_sched_misc_data;
	};
	uint8_t scheduled_cmd_id;
	uint8_t scheduled_subcmd_id;
	unsigned int seconds_from_now;
} CMD_SCHED_DATA_t;

/**
 * Compact representation of a command and its arguments.
 * Be wary of padding from alignment.
 * Make sure additional command fields are also added to CMD_SCHED_DATA_t.
 *
 * Current size: 24 bytes
 * Current slop: 2 bytes (trailing padding; struct alignment)
 */
typedef struct CMD {
	union {
		uint8_t cmd_data[sizeof(CMD_SCHED_DATA_t)];
		CMD_TASK_DATA_t cmd_task_data;
		CMD_STATE_DATA_t cmd_state_data;
		CMD_SCHED_DATA_t cmd_sched_data;
	};
	uint8_t cmd_id;
	uint8_t subcmd_id;
	/* 2 bytes slop here */
} CMD_t;

/**
* Checks if a string is a valid command, and if so, invokes it.
*
* A command is valid if the first word exists in CMD_OPTS.
* A command can be invoked with 0 to a maximum of 10 arguments.
* Each command determines the requirements of their own parameters.
* Commands are space delimited.
*
* @param cmd A command string
* @return 1 if the command is valid and invoked, 0 if the command is invalid
*/
int8_t checkAndRunCommandStr(char *cmd);
int8_t checkAndRunCommand(const CMD_t *cmd);

#endif /*SFUSAT_SFU_CMD_LINE_H_*/
