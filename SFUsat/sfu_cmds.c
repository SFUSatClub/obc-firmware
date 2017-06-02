/* Contains the available command line arguments
 *
 * Created: May 30 2017
 * Refactored code from sfu_uart.* by Seleena
 *
 * */

#include "sfu_uart.h"
#include "sfu_cmds.h"

int cmdHelp(int args, char **argv) {
	serialSendQ("Help");
	int i;
	for(i = 0; i < args; i++) {
		serialSendQ(argv[i]);
	}
	return 0;
}

char buffer[250];
int cmdGet(int args, char **argv) {
	if (args < 1) return -1;
	if (strcmp(argv[0], "tasks") == 0) {
	    serialSend("Task\t\tState\tPrio\tStack\tNum\n");
		vTaskList(buffer);
		serialSend(buffer);
	} else if (strcmp(argv[0], "runtime") == 0) {
		serialSend("Task\t\tAbs\t\tPerc\n");
		vTaskGetRunTimeStats(buffer);
		serialSend(buffer);
	} else if (strcmp(argv[0], "heap") == 0) {
		size_t heapSize = xPortGetFreeHeapSize();
		sprintf(buffer, "%lu bytes\n", heapSize);
		serialSend(buffer);
	} else if (strcmp(argv[0], "minheap") == 0) {
		size_t heapSize = xPortGetMinimumEverFreeHeapSize();
		sprintf(buffer, "%lu bytes\n", heapSize);
		serialSend(buffer);
	}
	return 0;
}

int cmdExec(int args, char **argv) {
	if (args < 1) return -1;
	if (strcmp(argv[0], "radio") == 0) {
	    initRadio();
	}
	return 0;
}

/**
* The following macros allow us to construct the arrays CMD_NAMES and CMD_FUNCS by only modifying
* the macro CMD_TABLE. This allows us to reliably use the same index to reference a command's name and
* function without the maintenance problems of constructing the arrays manually.
*
* The CMD_TABLE below should be the only place you'd need to edit to add/remove/change commands.
*/
#define CMD_TABLE(_) \
	_("help", cmdHelp) \
	_("get", cmdGet) \
	_("exec", cmdExec)

#define CMD_NAME_SELECTOR(a, b) \
	a,
#define CMD_FUNC_SELECTOR(a, b) \
	b,
const char *CMD_NAMES[] = {
	CMD_TABLE(CMD_NAME_SELECTOR)
};
int (*const CMD_FUNCS[])(int args, char **argv) = {
	CMD_TABLE(CMD_FUNC_SELECTOR)
};

/**
* Checks if a string is a valid command, and if so, invokes it.
*
* A command is valid if the first word exists in CMD_NAMES.
* A command can be invoked with 0 to a maximum of 10 arguments.
* Each command determines the requirements of their own parameters.
* Commands are space delimited.
*
* @param cmd A line of characters received from UART
* @return pdPASS if the command is found and invoked, pdFAIL if the command does
* not exist.
*/
#define MAX_CMD_ARGS 10
BaseType_t checkAndRunCommand(char *cmd) {
	const char delim[] = " ";
	char *intendedCmd = strtok(cmd, delim);
	/**
	 * Exit if we could not get the first token.
	 */
	if (intendedCmd == NULL) return pdFAIL;

	/**
	 * Compare the first word (which is the user's intended command) with all known
	 * commands. If a match is found, store the index so we can reference it later.
	 */
	int intendedCmdIdx = -1;
	size_t i;
	for (i = 0; i < sizeof(CMD_NAMES) / sizeof(char*); i++) {
		const char *currCmd = CMD_NAMES[i];
		if(strcmp(intendedCmd, currCmd) == 0) {
			intendedCmdIdx = i;
			break;
		}
	}
	/**
	 * Exit if the command does not exist.
	 */
	if (intendedCmdIdx == -1) return pdFAIL;

	/**
	 * Parse for and store every argument so commands can process them easily.
	 */
	char *args[MAX_CMD_ARGS] = {NULL};
	int argsIdx = 0;
	while (intendedCmd != NULL) {
		intendedCmd = strtok(NULL, delim);
		if (intendedCmd == NULL || argsIdx > MAX_CMD_ARGS - 1) break;
		args[argsIdx] = intendedCmd;
		argsIdx++;
	}

	/**
	 * Invoke the intended command with the amount of arguments to expect and the
	 * array of arguments itself.
	 */
	(*CMD_FUNCS[intendedCmdIdx])(argsIdx, args);

	return pdPASS;
}
