/* Contains the available command line arguments
 *
 * Created: May 30 2017
 * Refactored code from sfu_uart.* by Seleena
 *
 * */
#include <assert.h>

#include "sfu_uart.h"
#include "sfu_cmds.h"
#include "sfu_scheduler.h"
#include "sfu_state.h"
#include "sfu_utils.h"

struct subcmd_opt {
	const char *name;
	const uint8_t subcmd_id;
	const char *info;
};
struct cmd_opt {
	int8_t (*const func)(const CMD_t *cmd);
	const uint8_t cmd_id;
#ifdef _DEBUG
	const char *name;
	const struct subcmd_opt *subcmds;
	const size_t num_subcmds;
#endif
};

int sentinel = 0x89abcdef;
int test_const_size = HASH("test");

/**
 * Help command
 */
static const struct subcmd_opt CMD_HELP_OPTS[] = {
	{
		.subcmd_id   = CMD_HELP_NONE,
		.name = "",
		.info = "help  -- Show this help\n"
				"get   -- Get various metrics\n"
				"exec  -- Execute various actions\n"
				"rf    -- RF-related commands\n"
				"task  -- Task-related commands\n"
				"sched -- Schedule-related commands\n"
				"state -- State-related commands\n"
	},
	{
		.subcmd_id   = CMD_HELP_GET,
		.name = "get",
		.info = "Get various metrics"
	},
	{
		.subcmd_id   = CMD_HELP_EXEC,
		.name = "exec",
		.info = "Execute various actions"
	},
	{
		.subcmd_id   = CMD_HELP_RF,
		.name = "rf",
		.info = "RF-related commands"
	},
	{
		.subcmd_id   = CMD_HELP_TASK,
		.name = "task",
		.info = "Task-related commands"
	},
	{
		.subcmd_id   = CMD_HELP_SCHED,
		.name = "sched",
		.info = "Schedule-related commands"
	},
	{
		.subcmd_id   = CMD_HELP_STATE,
		.name = "state",
		.info = "State-related commands"
	},
};

int8_t cmdHelp(const CMD_t *cmd) {
	const struct subcmd_opt *subcmd_opt = NULL;
	FOR_EACH(subcmd_opt, CMD_HELP_OPTS) {
		if (cmd->subcmd_id == subcmd_opt->subcmd_id) {
			serialSendQ(subcmd_opt->info);
			return 1;
		}
	}
	if (IS_OUT_OF_BOUNDS(subcmd_opt, CMD_HELP_OPTS)) {
		serialSendQ("help: unknown sub-command");
	}
	return 0;
}


/**
 * Get command
 */
static const struct subcmd_opt CMD_GET_OPTS[] = {
	{
		.subcmd_id   = CMD_GET_NONE,
		.name = "",
	},
	{
		.subcmd_id   = CMD_GET_RUNTIME,
		.name = "runtime",
	},
	{
		.subcmd_id   = CMD_GET_HEAP,
		.name = "heap",
	},
};
char buffer[250];
int8_t cmdGet(const CMD_t *cmd) {
	switch (cmd->subcmd_id) {
		case CMD_GET_NONE:
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
	serialSendQ("get: unknown sub-command");
	return 0;
}

int8_t cmdExec(const CMD_t *cmd) {
	switch (cmd->subcmd_id) {
		case CMD_EXEC_RADIO: {
			initRadio();
			return 1;
		}
	}
	return 0;
}

int8_t cmdRF(const CMD_t *cmd) {
	switch (cmd->subcmd_id) {
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
	switch (cmd->subcmd_id) {
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
	switch (cmd->subcmd_id) {
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
 * Handle all state related commands.
 * @param cmd
 * @return
 */
int8_t cmdState(const CMD_t *cmd) {
	switch (cmd->subcmd_id) {
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
		 * Get previous state
		 */
		case CMD_STATE_PREV: {
			printPrevState(cur_state,&state_persistent_data);
			return 1;
		}
	}

	return 0;
}

/**
 * Command table
 */
static const struct cmd_opt CMD_OPTS[] = {
		{
				.cmd_id = CMD_HELP,
				.func = cmdHelp,
#ifdef _DEBUG
				.name = "help",
				.subcmds = CMD_HELP_OPTS,
				.num_subcmds = LEN(CMD_HELP_OPTS),
#endif
		},
		{
				.cmd_id = CMD_GET,
				.func = cmdGet,
#ifdef _DEBUG
				.name = "get",
				.subcmds = CMD_GET_OPTS,
				.num_subcmds = LEN(CMD_GET_OPTS),
#endif
		},
		{
				.cmd_id = CMD_EXEC,
				.func = cmdExec,
#ifdef _DEBUG
				.name = "exec",
				.subcmds = NULL,
				.num_subcmds = 0,
#endif
		},
		{
				.cmd_id = CMD_RF,
				.func = cmdRF,
#ifdef _DEBUG
				.name = "rf",
				.subcmds = NULL,
				.num_subcmds = 0,
#endif
		},
		{
				.cmd_id = CMD_TASK,
				.func = cmdTask,
#ifdef _DEBUG
				.subcmds = NULL,
				.num_subcmds = 0,
#endif
		},
		{
				.cmd_id = CMD_SCHED,
				.name = "sched",
				.func = cmdSched,
#ifdef _DEBUG
				.subcmds = NULL,
				.num_subcmds = 0,
#endif
		},
		{
				.cmd_id = CMD_STATE,
				.name = "state",
				.func = cmdState,
#ifdef _DEBUG
				.subcmds = NULL,
				.num_subcmds = 0,
#endif
		}
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
	const struct cmd_opt *cmd_opt = NULL;
	FOR_EACH(cmd_opt, CMD_OPTS) {
		if (cmd->cmd_id == cmd_opt->cmd_id) {
			cmd_opt->func(cmd);
			return 1;
		}
	}
	return 0;
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
	 * commands. The iterator cmd_opt is set on each iteration.
	 */
	const struct cmd_opt *cmd_opt;
	FOR_EACH(cmd_opt, CMD_OPTS) {
		if(strcmp(intendedCmd, cmd_opt->name) == 0) {
			break;
		}
	}
	/**
	 * Exit if the command does not exist.
	 */
	if (IS_OUT_OF_BOUNDS(cmd_opt, CMD_OPTS)) return 0;

	/**
	 * Compare the second word if it exists (which is the user's intended sub-command)
	 * with all known sub-commands for that command. Save the sub-command ID if a match is found.
	 */
	intendedCmd = strtok(NULL, delim);
	const struct subcmd_opt *subcmd_opt = NULL;
	uint8_t subcmd_id = CMD_UNDEFINED;
	for (subcmd_opt = cmd_opt->subcmds; subcmd_opt != NULL &&
										intendedCmd != NULL &&
										subcmd_opt < &cmd_opt->subcmds[cmd_opt->num_subcmds]; subcmd_opt++)
	{
		if(strcmp(intendedCmd, subcmd_opt->name) == 0) {
			subcmd_id = subcmd_opt->subcmd_id;
			break;
		}
	}

	CMD_t cmd_t = {.cmd_id = cmd_opt->cmd_id, .subcmd_id = subcmd_id};
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
	unsigned int i = 0;
	if (data != NULL) {
		const int data_len = strlen(data);
		for (i = 0; i < CMD_DATA_MAX_SIZE * 2 && i < data_len; i += 2) {
			char c[3] = {NULL};
			c[0] = *(data + i);
			// TODO: fix dereference beyond null char
			const char n = *(data + i + 1);
			c[1] = n == '\0' ? '0' : n;
			cmd_t.cmd_data[i / 2] = strtol(c, NULL, 16);
		}
	}
	serialSend((char *)cmd_t.cmd_data);

	/**
	 * Invoke the intended command with the command struct created above.
	 */
	cmd_opt->func(&cmd_t);

	return 1;
}
