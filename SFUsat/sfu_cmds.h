/* Contains the available command line arguments
 *
 * Created: May 30 2017
 * Refactored code from sfu_uart.* by Seleena
 *
 * */

#ifndef SFUSAT_SFU_CMD_LINE_H_
#define SFUSAT_SFU_CMD_LINE_H_

#include "map.h"

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
	_(CMD_TASK	, "task", 	cmdTask, 	NONE, CREATE, DELETE, RESUME, SUSPEND, STATUS, SHOW)

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
	char data[10];
} CMD_TASK_DATA_t;

/**
 * Compact representation of a command and its arguments.
 *
 * Be wary of padding. CMD_t is byte/word/struct-aligned to
 * save memory since a static array of MAX_EVENTS of them is
 * defined in sfu_scheduler.
 *
 * Each CMD_t currently takes up: 12 bytes.
 */
typedef struct CMD {
	union {
		char cmd_data[11];
		CMD_TASK_DATA_t cmd_task_data;
	};
	CMD_ID cmd_id : 4;
	union {
		unsigned int subcmd_id : 4;
		CMD_HELP_SUBCMD subcmd_help_id : 4;
		CMD_GET_SUBCMD subcmd_get_id : 4;
		CMD_EXEC_SUBCMD subcmd_exec_id : 4;
		CMD_TASK_SUBCMD subcmd_task_id : 4;
	};
} CMD_t;

extern const char *CMD_NAMES[];
extern int (*const CMD_FUNCS[])(const CMD_t *cmd);


/**
* Checks if a string is a valid command, and if so, invokes it.
*
* A command is valid if the first word exists in CMD_NAMES.
* A command can be invoked with 0 to a maximum of 10 arguments.
* Each command determines the requirements of their own parameters.
* Commands are space delimited.
*
* @param cmd A command string
* @return 1 if the command is found and invoked, 0 if the command does
* not exist.
*/
int checkAndRunCommandStr(char *cmd);
int checkAndRunCommand(const CMD_t *cmd);

#endif /*SFUSAT_SFU_CMD_LINE_H_*/
