/* Contains the available command line arguments
 *
 * Created: May 30 2017
 * Refactored code from sfu_uart.* by Seleena
 *
 * */

#ifndef SFUSAT_SFU_CMD_LINE_H_
#define SFUSAT_SFU_CMD_LINE_H_


/**
* The following macros allow us to construct the enum CMD_IDS and arrays CMD_NAMES and CMD_FUNCS by only modifying
* the macro CMD_TABLE. This allows us to reliably use the same CMD_ID index to reference a command's name and
* function without the maintenance problems of constructing the arrays manually.
*
* The CMD_TABLE below should be the only place you'd need to edit to add/remove/change commands.
*/
#define CMD_TABLE(_) \
	_(CMD_HELP, "help", cmdHelp) \
	_(CMD_GET, "get", cmdGet) \
	_(CMD_EXEC, "exec", cmdExec) \
	_(CMD_TASK, "task", cmdTask)

#define CMD_ENUM_SELECTOR(a, b, c) \
	a,
#define CMD_NAME_SELECTOR(a, b, c) \
	b,
#define CMD_FUNC_SELECTOR(a, b, c) \
	c,
typedef enum CMD_IDS {
	CMD_TABLE(CMD_ENUM_SELECTOR)
	NUM_CMDS
} CMD_ID;

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
int checkAndRunCommand(char *cmd);

typedef struct CMD {
	CMD_ID cmd_id : 4;
	unsigned int subcmd_id : 4;
	char args[10];
} CMD_t;

#endif /*SFUSAT_SFU_CMD_LINE_H_*/
