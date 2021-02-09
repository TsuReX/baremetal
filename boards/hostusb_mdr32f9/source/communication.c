/*
 * communication.c
 *
 *  Created on: Sep 8, 2020
 *      Author: user
 */


#include "communication.h"
#include "config.h"
#include "drivers.h"

static size_t	buffer_size;
static void*	buffer;

void comm_init(void* dst_buffer, size_t dst_buffer_size)
{
	buffer_size = dst_buffer_size;
	buffer = dst_buffer;

#if 1
	/* USART 1,2 GPIO */
	RST_CLK_PCLKcmd(RST_CLK_PCLK_PORTB, ENABLE);

	PORT_InitTypeDef port_descriptor;
	PORT_StructInit(&port_descriptor);

	port_descriptor.PORT_PULL_UP = PORT_PULL_UP_OFF;
	port_descriptor.PORT_PULL_DOWN = PORT_PULL_DOWN_OFF;
	port_descriptor.PORT_PD_SHM = PORT_PD_SHM_OFF;
	port_descriptor.PORT_PD = PORT_PD_DRIVER;
	port_descriptor.PORT_GFEN = PORT_GFEN_OFF;
	port_descriptor.PORT_FUNC = PORT_FUNC_ALTER;
	port_descriptor.PORT_SPEED = PORT_SPEED_MAXFAST;
	port_descriptor.PORT_MODE = PORT_MODE_DIGITAL;

	/* Configure PORTB pins 5 (UART1_TX) as output */
	port_descriptor.PORT_OE = PORT_OE_OUT;
	port_descriptor.PORT_Pin = PORT_Pin_5;
	PORT_Init(MDR_PORTB, &port_descriptor);

	/* Configure PORTB pins 6 (UART1_RX) as input */
	port_descriptor.PORT_OE = PORT_OE_IN;
	port_descriptor.PORT_Pin = PORT_Pin_6;
	PORT_Init(MDR_PORTB, &port_descriptor);

	UART_InitTypeDef UART_InitStructure;
	/* Enables the CPU_CLK clock on UART1, UART2 */
	RST_CLK_PCLKcmd(RST_CLK_PCLK_UART1, ENABLE);

	/* Set the HCLK division factor = 1 for UART1,UART2*/
	UART_BRGInit(MDR_UART1, UART_HCLKdiv1);

	/* Initialize UART_InitStructure */
	UART_InitStructure.UART_BaudRate            = 1500000;
	UART_InitStructure.UART_WordLength         	= UART_WordLength8b;
	UART_InitStructure.UART_StopBits           	= UART_StopBits1;
	UART_InitStructure.UART_Parity           	= UART_Parity_No;
	UART_InitStructure.UART_FIFOMode           	= UART_FIFO_OFF;
	UART_InitStructure.UART_HardwareFlowControl	= UART_HardwareFlowControl_RXE | UART_HardwareFlowControl_TXE;

	/* Configure UART1 parameters*/
	UART_Init(MDR_UART1,&UART_InitStructure);

#endif

#if 1
	RST_CLK_PCLKcmd(RST_CLK_PCLK_PORTD, ENABLE);

	port_descriptor.PORT_PULL_UP = PORT_PULL_UP_OFF;
	port_descriptor.PORT_PULL_DOWN = PORT_PULL_DOWN_OFF;
	port_descriptor.PORT_PD_SHM = PORT_PD_SHM_OFF;
	port_descriptor.PORT_PD = PORT_PD_DRIVER;
	port_descriptor.PORT_GFEN = PORT_GFEN_OFF;
	port_descriptor.PORT_FUNC = PORT_FUNC_ALTER;
	port_descriptor.PORT_SPEED = PORT_SPEED_MAXFAST;
	port_descriptor.PORT_MODE = PORT_MODE_DIGITAL;

	/* Configure PORTD pins 1 (UART2_TX) as output */
	port_descriptor.PORT_OE = PORT_OE_OUT;
	port_descriptor.PORT_Pin = PORT_Pin_1;
	PORT_Init(MDR_PORTD, &port_descriptor);

	/* Configure PORTD pins 0 (UART2_RX) as input */
	port_descriptor.PORT_OE = PORT_OE_IN;
	port_descriptor.PORT_Pin = PORT_Pin_0;
	PORT_Init(MDR_PORTD, &port_descriptor);

	RST_CLK_PCLKcmd(RST_CLK_PCLK_UART2, ENABLE);

	UART_BRGInit(MDR_UART2, UART_HCLKdiv1);


	/* Initialize UART_InitStructure */
	UART_InitStructure.UART_BaudRate            = 1500000;
	UART_InitStructure.UART_WordLength			= UART_WordLength8b;
	UART_InitStructure.UART_StopBits            = UART_StopBits1;
	UART_InitStructure.UART_Parity              = UART_Parity_No;
	UART_InitStructure.UART_FIFOMode            = UART_FIFO_OFF;
	UART_InitStructure.UART_HardwareFlowControl	= UART_HardwareFlowControl_RXE | UART_HardwareFlowControl_TXE;

	/* Configure UART2 parameters*/
	UART_Init(MDR_UART2, &UART_InitStructure);
#endif

}

void comm_start(void)
{
#if 1
	/* Enables UART1 peripheral */
	UART_Cmd(MDR_UART1, ENABLE);
#endif

#if 1
	/* Enables UART2 peripheral */
	UART_Cmd(MDR_UART2, ENABLE);
#endif
}

void comm_stop(void)
{

}
