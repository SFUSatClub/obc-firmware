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

void scidmaInit(sciBASE_t *sci);
void scidmaSend(char *source_address);
void Update_DMA_Comp_Flag();
void number_string(char *string, uint32 count);

void uart_dma_test();
/* dma control packet configuration stack */
g_dmaCTRL dma_config;
g_dmaCTRL g_dmaCTRLPKT;
uint32 DMA_Comp_Flag;

char buffer[4 * 500];

#endif /* SFUSAT_OBC_SCI_DMA_H_ */
