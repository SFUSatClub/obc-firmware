/* Contains the available command line arguments
 *
 * Created: May 30 2017
 * Refactored code from sfu_uart.* by Seleena
 *
 * */

#ifndef SFUSAT_SFU_CMD_LINE_H_
#define SFUSAT_SFU_CMD_LINE_H_

extern const char *CMD_NAMES[];
extern int (*const CMD_FUNCS[])(int args, char **argv);
BaseType_t checkAndRunCommand(char *cmd);


#endif /*SFUSAT_SFU_CMD_LINE_H_*/
