/* Contains the available command line arguments
 *
 * Created: May 30 2017
 * Refactored code from sfu_uart.* by Seleena
 *
 * */

#include "sfu_uart.h"
#include "sfu_cmds.h"
#include "sfu_scheduler.h"

int8_t cmdHelp(const CMD_t *cmd) {
	switch (cmd->subcmd_help_id) {
		case CMD_HELP_NONE: {
			serialSendQ("help cmd 0 args");
			return 1;
		}
	}
	return 0;
}

char buffer[250];
int8_t cmdGet(const CMD_t *cmd) {
	switch (cmd->subcmd_get_id) {
		case CMD_GET_TASKS: {
			serialSend("Task\t\tState\tPrio\tStack\tNum\n");
			vTaskList(buffer);
			serialSend(buffer);
			return 1;
		}
		case CMD_GET_RUNTIME: {
			serialSend("Task\t\tAbs\t\tPerc\n");
			vTaskGetRunTimeStats(buffer);
			serialSend(buffer);
			return 1;
		}
		case CMD_GET_HEAP: {
			size_t heapSize = xPortGetFreeHeapSize();
			sprintf(buffer, "%lu bytes\n", heapSize);
			serialSend(buffer);
			return 1;
		}
		case CMD_GET_MINHEAP: {
			size_t heapSize = xPortGetMinimumEverFreeHeapSize();
			sprintf(buffer, "%lu bytes\n", heapSize);
			serialSend(buffer);
			return 1;
		}
		case CMD_GET_TYPES: {
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
			return 1;
		}
	}
	return 0;
}

int8_t cmdExec(const CMD_t *cmd) {
	switch (cmd->subcmd_exec_id) {
		case CMD_EXEC_RADIO: {
			initRadio();
			return 1;
		}
	}
	return 0;
}

/**
 * Handle all task related commands.
 * @param cmd
 * @return
 */
int8_t cmdTask(const CMD_t *cmd) {
	switch (cmd->subcmd_task_id) {
		/**
		 * Show status of all tasks if no arguments are supplied.
		 */
		case CMD_TASK_NONE: {
			return 1;
		}
		/**
		 * Create a task.
		 */
		case CMD_TASK_CREATE: {
			return 1;
		}
		/**
		 * Delete a task.
		 */
		case CMD_TASK_DELETE: {
			switch (cmd->cmd_task_data.task_id) {
				case TASK_SERIAL: {
					vTaskDelete(xSerialTaskHandle);
					return 1;
				} case TASK_RADIO: {
					vTaskDelete(xRadioTaskHandle);
					return 1;
				} case TASK_BLINKY: {
					vTaskDelete(xBlinkyTaskHandle);
					return 1;
				}
			}
			return 0;
		}
		/**
		 * Resume a task.
		 */
		case CMD_TASK_RESUME: {
			switch (cmd->cmd_task_data.task_id) {
				case TASK_SERIAL: {
					vTaskResume(xSerialTaskHandle);
					return 1;
				} case TASK_RADIO: {
					vTaskResume(xRadioTaskHandle);
					return 1;
				} case TASK_BLINKY: {
					vTaskResume(xBlinkyTaskHandle);
					return 1;
				}
			}
			return 0;
		}
		/**
		 * Suspend a task.
		 */
		case CMD_TASK_SUSPEND: {
			switch (cmd->cmd_task_data.task_id) {
				case TASK_SERIAL: {
					vTaskSuspend(xSerialTaskHandle);
					return 1;
				} case TASK_RADIO: {
					vTaskSuspend(xRadioTaskHandle);
					return 1;
				} case TASK_BLINKY: {
					vTaskSuspend(xBlinkyTaskHandle);
					return 1;
				}
			}
			return 0;
		}
		/**
		 * Show runtime status of a task.
		 */
		case CMD_TASK_STATUS: {
			switch (cmd->cmd_task_data.task_id) {
				case TASK_SERIAL: {
					//vTaskGetInfo(xSerialTaskHandle);
					return 1;
				} case TASK_RADIO: {
					//vTaskGetInfo(xRadioTaskHandle);
					return 1;
				} case TASK_BLINKY: {
					//vTaskGetInfo(xBlinkyTaskHandle);
					return 1;
				}
			}
			return 0;
		}
		/**
		 * Show properties of a task.
		 */
		case CMD_TASK_SHOW: {
			return 1;
		}
	}

	return 0;
}

/**
 * Handle all command scheduling related commands.
 * @param cmd
 * @return
 */
int8_t cmdSched(const CMD_t *cmd) {
	switch (cmd->subcmd_sched_id) {
		case CMD_SCHED_NONE: {
			return 1;
		}
		case CMD_SCHED_ADD: {
			CMD_t event_cmd = {
					.cmd_id = cmd->cmd_sched_data.scheduled_cmd_id,
					.subcmd_id = cmd->cmd_sched_data.scheduled_subcmd_id,
			};
			memcpy(event_cmd.cmd_data, cmd->cmd_sched_data.scheduled_cmd_data, CMD_DATA_MAX_SIZE);
			Event_t event = {
					.seconds_from_now = cmd->cmd_sched_data.seconds_from_now,
					.action = event_cmd,
			};
			addEvent(event);
			return 1;
		}
		case CMD_SCHED_REMOVE: {
			const int idx = cmd->cmd_sched_data.cmd_sched_misc_data.event_idx_to_remove;
			return removeEventIdx(idx);
		}
		case CMD_SCHED_SHOW: {
			serialSendQ("schedule:\n");
			showActiveEvents();
			return 1;
		}
	}

	return 0;
}

/**
 * Handle all task related commands.
 * @param cmd
 * @return
 */
int8_t cmdState(const CMD_t *cmd) {
	switch (cmd->subcmd_state_id) {
		/**
		 * nothing
		 */
		case CMD_STATE_NONE: {
			return 1;
		}
		/**
		 * Get current state
		 */
		case CMD_STATE_GET: {
			printStateInfo(cur_state,&state_persistent_data);
			return 1;
		}
		/**
		 * Set system state (can be scheduled in the future too)
		 */
		case CMD_STATE_SET: {
			switch (cmd->cmd_state_data.state_id) {
				case STATE_SAFE: {
					if(STATE_SAFE == setStateManual(&state_persistent_data, STATE_SAFE)){
						serialSendQ("SET STATE SAFE");
						return 1;
					}

				} case STATE_READY: {
					if(STATE_READY == setStateManual(&state_persistent_data, STATE_READY)){
						serialSendQ("SET STATE READY");
						return 1;
					}
				} case STATE_LOW_POWER: {
					if(STATE_LOW_POWER == setStateManual(&state_persistent_data, STATE_LOW_POWER)){
						serialSendQ("SET STATE LOW POWER");
						return 1;
					}
				}
			}
			return 0;
		}
		/**
		 * get previous state
		 */
		case CMD_STATE_PREV: {
			printPrevState(cur_state,&state_persistent_data);
			return 1;
		}
	}

	return 0;
}


#define LEN(array) (sizeof((array))/sizeof((array)[0]))
/**
 * CMD_DBG_STRINGS[i][j]
 * 		- Index of commands i (row) must match order defined in CMD_NAMES.
 *  	- Index of sub-commands j (col) must match enum representation defined in CMD_TABLE.
 *  	- Index 0 is reserved by NONE, so it's used as row descriptor.
 */
#define MAX_SUB_CMDS 10
const char *CMD_DBG_STRINGS[][MAX_SUB_CMDS] = {
		{"help"},
		{"get", "tasks", "runtime", "heap", "minheap", "types"},
		{"exec", "radio"},
		{"task", "create", "delete", "resume", "suspend", "status", "show"},
		{"sched", "add", "remove", "show"},
		{"state", "get", "prev", "set"},
};
const char *CMD_NAMES[] = {
		CMD_TABLE(CMD_NAME_SELECTOR)
};
int8_t (*const CMD_FUNCS[])(const CMD_t *cmd) = {
		CMD_TABLE(CMD_FUNC_SELECTOR)
};

/**
 * Quickly get size of arbitrary structures using map file and grep.
 *
 * For example...
 * 		- `$ grep .data:test Debug/OBC.map`
 * 		   > 080056c8    00000018     sfu_cmds.obj (.data:testCMD)
 * 		   > 080056f4    00000014     sfu_cmds.obj (.data:testSCHED)
 */
#ifdef _DEBUG
CMD_t testCMD = {0};
CMD_SCHED_DATA_t testSCHED = {0};
void __unused() {
	testCMD.cmd_id = CMD_GET;
	testSCHED.scheduled_cmd_id = CMD_GET;
}
#endif

int8_t checkAndRunCommand(const CMD_t *cmd) {
	const CMD_ID id = cmd->cmd_id;
	if (id >= NUM_CMDS) return 0;
	(*CMD_FUNCS[id])(cmd);
	return 1;
}

int8_t checkAndRunCommandStr(char *cmd) {
	const char delim[] = " ";
	char *intendedCmd = strtok(cmd, delim);
	/**
	 * Exit if we could not get the first token.
	 */
	if (intendedCmd == NULL) return 0;

	/**
	 * Compare the first word (which is the user's intended command) with all known
	 * commands. Save the index if a match is found.
	 */
	int8_t intendedCmdIdx = -1;
	uint8_t i;
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
	 * Compare the second word if it exists (which is the user's intended sub-command)
	 * with all known sub-commands for that command. Save the index if a match is found.
	 */
	intendedCmd = strtok(NULL, delim);
	uint8_t intendedSubCmdIdx = 0;
	for (i = 1; i < MAX_SUB_CMDS && CMD_DBG_STRINGS[intendedCmdIdx][i] != NULL && intendedCmd != NULL; i++) {
		if (strcmp(CMD_DBG_STRINGS[intendedCmdIdx][i], intendedCmd) == 0) {
			intendedSubCmdIdx = i;
			break;
		}
	}

	CMD_t cmd_t = {.cmd_id = (CMD_ID)intendedCmdIdx, .subcmd_id = intendedSubCmdIdx};
	/**
	 * The third token will be interpreted as a hex string if it exists.
	 * No preceding 0x required.
	 * The command's argument field, cmd_data, will be set to this hex data.
	 *
	 * Data is parsed in pairs of hex characters.
	 *
	 * Example:
	 * 		> task suspend 40
	 *
	 * 		Data will be == "40\0"
	 * 		This will be parsed into the corresponding byte, and then
	 * 		cmd_t.cmd_task_data.task_id will be == TASK_BLINKY (4)
	 */
	const char *data = strtok(NULL, delim);
	if (data != NULL) {
		const int data_len = strlen(data);
		for (i = 0; i < CMD_DATA_MAX_SIZE * 2 && i < data_len; i += 2) {
			char c[3] = {NULL};
			c[0] = *(data + i);
			const char n = *(data + i + 1);
			c[1] = n == NULL ? '0' : n;
			cmd_t.cmd_data[i / 2] = strtol(c, NULL, 16);
		}
	}
	serialSend((char *)cmd_t.cmd_data);

	/**
	 * Invoke the intended command with the command struct created above.
	 */
	(*CMD_FUNCS[intendedCmdIdx])(&cmd_t);

	return 1;
}
