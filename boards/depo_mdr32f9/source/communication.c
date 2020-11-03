/*
 * communication.c
 *
 *  Created on: Sep 8, 2020
 *      Author: user
 */


#include "communication.h"
#include "config.h"


static size_t	buffer_size;
static void*	buffer;

void comm_init(void* dst_buffer, size_t dst_buffer_size)
{

	buffer_size = dst_buffer_size;
	buffer = dst_buffer;

	UART_InitTypeDef UART_InitStructure;
	/* Enables the CPU_CLK clock on UART1,UART2 */
	RST_CLK_PCLKcmd(RST_CLK_PCLK_UART1, ENABLE);
	RST_CLK_PCLKcmd(RST_CLK_PCLK_UART2, ENABLE);

	/* Set the HCLK division factor = 1 for UART1,UART2*/
	UART_BRGInit(MDR_UART1, UART_HCLKdiv1);
	UART_BRGInit(MDR_UART2, UART_HCLKdiv1);

	/* Initialize UART_InitStructure */
	UART_InitStructure.UART_BaudRate                = 1500000;
	UART_InitStructure.UART_WordLength              = UART_WordLength8b;
	UART_InitStructure.UART_StopBits                = UART_StopBits1;
	UART_InitStructure.UART_Parity                  = UART_Parity_No;
	UART_InitStructure.UART_FIFOMode                = UART_FIFO_OFF;
	UART_InitStructure.UART_HardwareFlowControl     = UART_HardwareFlowControl_RXE | UART_HardwareFlowControl_TXE;

	/* Configure UART1 parameters*/
	UART_Init (MDR_UART1,&UART_InitStructure);

	/* Enables UART1 peripheral */
	UART_Cmd(MDR_UART1, ENABLE);

	/* Configure UART2 parameters*/
	UART_Init (MDR_UART2, &UART_InitStructure);

	/* Enables UART2 peripheral */
	UART_Cmd(MDR_UART2, ENABLE);
}

void comm_start(void)
{

}

void comm_stop(void)
{

}
