/*
 * obc_sci_dma.h
 *
 *  Created on: Aug 8, 2018
 *      Author: sbork
 */

#ifndef SFUSAT_OBC_SCI_DMA_H_
#define SFUSAT_OBC_SCI_DMA_H_

#include "sci.h"
#include "lin.h"
#include "sys_dma.h"
#include "sys_vim.h"

#define Enable_SCI_Out

void uart_dma_init();
void uart_dma_send(char* string);
void uart_dma_sendln(char* string);
void uart_dma_test();

//void Update_DMA_Comp_Flag();
/* dma control packet configuration stack */
extern g_dmaCTRL g_dmaCTRLPKT;
extern uint32 DMA_Comp_Flag;

#endif /* SFUSAT_OBC_SCI_DMA_H_ */
