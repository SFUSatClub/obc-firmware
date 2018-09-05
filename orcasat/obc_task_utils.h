/*
 * sfu_task_utils.h
 *
 *  Created on: Jun 28, 2018
 *      Author: steven
 */

#ifndef SFUSAT_OBC_TASK_UTILS_H_
#define SFUSAT_OBC_TASK_UTILS_H_

#define UART_RF_MUX_TARGET_MASK		(0xf0000000)
#define UART_RF_MUX_TARGET_RF			(0b0010)
#define UART_RF_MUX_TARGET_UART			(0b0100)
#define UART_RF_MUX_TARGET_BOTH			(0b0110)
#define UART_RF_MUX_TARGET_NONE			(0b0000)
#define UART_RF_MUX_LOG_LVL_MASK	(0x0000ffff)
#define UART_RF_MUX_DEFAULT			(0x00000000) /* E.g., set to (UART_RF_MUX_TARGET_BOTH << 28) to log to both */

#define IS_UART_RF_MUX( X ) 	 ( ( (__UART_RF_MUX_CONF & UART_RF_MUX_TARGET_MASK) >> 28 ) & (X) )
#define SET_UART_RF_MUX( X ) 		( __UART_RF_MUX_CONF |=  ( (X << 28) & UART_RF_MUX_TARGET_MASK ) )
#define UNSET_UART_RF_MUX( X ) 		( __UART_RF_MUX_CONF &= ~( (X << 28) & UART_RF_MUX_TARGET_MASK ) )

#define UART_RF_MUX_INIT() 										\
	static uint32_t __UART_RF_MUX_CONF = UART_RF_MUX_DEFAULT;

#define UART_RF_MUX_SENDQ(str_buffer)							\
	if ( IS_UART_RF_MUX(UART_RF_MUX_TARGET_UART) ) {			\
	    serialSendQ(str_buffer);								\
	} 															\
	if ( IS_UART_RF_MUX(UART_RF_MUX_TARGET_RF) ) {				\
		RadioDAT_t currQueuedPacket = { 0 };					\
		strcpy((char *)currQueuedPacket.data, str_buffer);		\
		currQueuedPacket.size = strlen(str_buffer);				\
		currQueuedPacket.unused = 0xAA;							\
		xQueueSendToBack(xRadioTXQueue, &currQueuedPacket, 0);	\
	}

#endif /* SFUSAT_OBC_TASK_UTILS_H_ */
