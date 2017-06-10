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
	} else if (strcmp(argv[0], "types") == 0) {
		size_t intSize = sizeof(int);
		size_t intPtrSize = sizeof(int *);
		size_t longSize = sizeof(long);
		size_t longLongSize = sizeof(long long);
		sprintf(buffer, "sizeof(int): %lu bytes\n"
				"sizeof(int *): %lu bytes\n"
				"sizeof(long): %lu bytes\n"
				"sizeof(long long): %lu bytes\n"
				, intSize, intPtrSize, longSize, longLongSize);
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
 * Handle all task related commands.
 * @param args
 * @param argv
 * @return
 */
int cmdTask(int args, char **argv) {
	/**
	 * Show status of all tasks if no arguments are supplied.
	 */
	if (args == 0) {
		return 0;
	}
	/**
	 * Return with error if 1 argument is supplied, as there are no valid 1 argument commands yet.
	 */
	if (args == 1) {
		return -1;
	}
	if (strcmp(argv[0], "create") == 0) {
		/**
		 * Create one or more tasks.
		 */

	} else if (strcmp(argv[0], "delete") == 0) {
		/**
		 * Delete one or more tasks.
		 */
		int i;
		for (i = 1; i < args; i++) {
			if (strcmp(argv[i], "serial") == 0) {
				vTaskDelete(xSerialTaskHandle);
			} else if (strcmp(argv[i], "radio") == 0) {
				vTaskDelete(xRadioTaskHandle);
			} else if (strcmp(argv[i], "blinky") == 0) {
				vTaskDelete(xBlinkyTaskHandle);
			}
		}
	} else if (strcmp(argv[0], "resume") == 0) {
		/**
		 * Resume one or more tasks.
		 */
		int i;
		for (i = 1; i < args; i++) {
			if (strcmp(argv[i], "serial") == 0) {
				vTaskResume(xSerialTaskHandle);
			} else if (strcmp(argv[i], "radio") == 0) {
				vTaskResume(xRadioTaskHandle);
			} else if (strcmp(argv[i], "blinky") == 0) {
				vTaskResume(xBlinkyTaskHandle);
			}
		}
	} else if (strcmp(argv[0], "suspend") == 0) {
		/**
		 * Suspend one or more tasks.
		 */
		int i;
		for (i = 1; i < args; i++) {
			if (strcmp(argv[i], "serial") == 0) {
				vTaskSuspend(xSerialTaskHandle);
			} else if (strcmp(argv[i], "radio") == 0) {
				vTaskSuspend(xRadioTaskHandle);
			} else if (strcmp(argv[i], "blinky") == 0) {
				vTaskSuspend(xBlinkyTaskHandle);
			}
		}
	} else if (strcmp(argv[0], "status") == 0) {
		/**
		 * Show runtime status of one or more tasks.
		 */
		int i;
		for (i = 1; i < args; i++) {
			if (strcmp(argv[i], "serial") == 0) {
				//vTaskGetInfo(xSerialTaskHandle);
			} else if (strcmp(argv[i], "radio") == 0) {
				//vTaskGetInfo(xRadioTaskHandle);
			} else if (strcmp(argv[i], "blinky") == 0) {
				//vTaskGetInfo(xBlinkyTaskHandle);
			}
		}
	} else if (strcmp(argv[0], "show") == 0) {
		/**
		 * Show properties of one or more tasks.
		 */
	}
	return 0;
}

const char *CMD_NAMES[] = {
	CMD_TABLE(CMD_NAME_SELECTOR)
};
int (*const CMD_FUNCS[])(int args, char **argv) = {
	CMD_TABLE(CMD_FUNC_SELECTOR)
};

#define MAX_CMD_ARGS 10
int checkAndRunCommand(char *cmd) {
	const char delim[] = " ";
	char *intendedCmd = strtok(cmd, delim);
	/**
	 * Exit if we could not get the first token.
	 */
	if (intendedCmd == NULL) return 0;

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
	if (intendedCmdIdx == -1) return 0;

	/**
	 * Parse for and store every argument so commands can process them easily.
	 */
	char *args[MAX_CMD_ARGS] = {NULL};
	int argsIdx = 0;
	while (intendedCmd != NULL) {
		intendedCmd = strtok(NULL, delim);
		if (intendedCmd == NULL || argsIdx >= MAX_CMD_ARGS) break;
		args[argsIdx] = intendedCmd;
		argsIdx++;
	}

	/**
	 * Invoke the intended command with the amount of arguments to expect and the
	 * array of arguments itself.
	 */
	(*CMD_FUNCS[intendedCmdIdx])(argsIdx, args);

	return 1;
}
