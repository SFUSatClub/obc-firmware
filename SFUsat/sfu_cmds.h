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
* The following macros allow us to construct the enums CMD_IDS, CMD_X_SUBCMDS and
* arrays CMD_NAMES, CMD_FUNCS by only modifying the macro CMD_TABLE. This allows us
* to reliably use the same CMD_ID index to reference the components of a command
* such as their name and function without the maintenance problems of constructing
* the arrays manually.
*
* The CMD_TABLE below should be the only place you'd need to edit to add/remove/change commands.
*
* Some explanation:
*
* 	_(    A    ,    B    ,    C    ,    D...    ) \
*
* 	Symbols in column A make up the CMD_IDS enum.
* 	Symbols in column B denote the name of the current row's command.
* 	Symbols in column C denote the function to handle the current row's command.
* 	Symbols in column D onwards make up the sub-command enum of the current row's command.
*
* Usage:
*
* 	Get the name of the task command and call it
* 		const char *name = CMD_NAMES[CMD_GET]; // "get"
* 		(*CMD_FUNCS[CMD_GET])(numArgs, args); // invokes get command with numArgs and args
*
*/
#define CMD_TABLE(_) \
	_(CMD_HELP	, "help", 	cmdHelp,	NONE) \
	_(CMD_GET	, "get", 	cmdGet, 	NONE, TASKS, RUNTIME, HEAP, MINHEAP, TYPES) \
	_(CMD_EXEC	, "exec", 	cmdExec, 	NONE, RADIO) \
	_(CMD_TASK	, "task", 	cmdTask, 	NONE, CREATE, DELETE, RESUME, SUSPEND, STATUS, SHOW) \
	_(CMD_SCHED	, "sched", 	cmdSched, 	NONE, ADD, REMOVE, SHOW) \
	_(CMD_STATE	, "state", 	cmdState, 	NONE, GET, PREV, SET)

#define CMD_ENUM_SELECTOR(a, b, c, d...) \
	a,
#define CMD_NAME_SELECTOR(a, b, c, d...) \
	b,
#define CMD_FUNC_SELECTOR(a, b, c, d...) \
	c,
#define CMD_SUBCMD_ENUM_SELECTOR0(a, b) \
	a ## _ ## b,
#define CMD_SUBCMD_ENUM_SELECTOR(a, b, c, d...) \
	typedef enum a ## _SUBCMDS { \
	MAP(CMD_SUBCMD_ENUM_SELECTOR0,a,d) \
	a ## _NUM_SUBCMDS \
} a ## _SUBCMD;

typedef enum CMD_IDS {
	CMD_TABLE(CMD_ENUM_SELECTOR)
	NUM_CMDS
} CMD_ID;
CMD_TABLE(CMD_SUBCMD_ENUM_SELECTOR)


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
 * When the sub-command of CMD_SCHED is ADD, the cmd_data field will be reserved for the
 * arguments of the scheduled command. This struct is therefore inapplicable in this case.
 *
 * When the sub-command of CMD_SCHED is not ADD (e.g., REMOVE, etc), this struct will be used to
 * type-pun cmd_data into the following fields.
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
	CMD_ID scheduled_cmd_id;
	union {
		uint8_t scheduled_subcmd_id;
		CMD_HELP_SUBCMD scheduled_subcmd_help_id;
		CMD_GET_SUBCMD scheduled_subcmd_get_id;
		CMD_EXEC_SUBCMD scheduled_subcmd_exec_id;
		CMD_TASK_SUBCMD scheduled_subcmd_task_id;
		CMD_STATE_SUBCMD scheduled_subcmd_state_id;
	};
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
	CMD_ID cmd_id;
	union {
		uint8_t subcmd_id;
		CMD_HELP_SUBCMD subcmd_help_id;
		CMD_GET_SUBCMD subcmd_get_id;
		CMD_EXEC_SUBCMD subcmd_exec_id;
		CMD_TASK_SUBCMD subcmd_task_id;
		CMD_STATE_SUBCMD subcmd_state_id;
		CMD_SCHED_SUBCMD subcmd_sched_id;
	};
	/* 2 bytes slop here */
} CMD_t;


extern const char *CMD_NAMES[];
extern int8_t (*const CMD_FUNCS[])(const CMD_t *cmd);


/**
* Checks if a string is a valid command, and if so, invokes it.
*
* A command is valid if the first word exists in CMD_NAMES.
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
