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
extern const char *CMD_NAMES[];
extern int (*const CMD_FUNCS[])(int args, char **argv);

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

typedef struct CMD {
	CMD_ID cmd_id : 4;
	unsigned int subcmd_id : 4;
	char args[10];
} CMD_t;

#endif /*SFUSAT_SFU_CMD_LINE_H_*/
