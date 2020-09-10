#include "drivers.h"
#include "console.h"

#define ALL_PORTS_CLK (RST_CLK_PCLK_PORTA | RST_CLK_PCLK_PORTB | \
                       RST_CLK_PCLK_PORTC | RST_CLK_PCLK_PORTD | \
                       RST_CLK_PCLK_PORTE | RST_CLK_PCLK_PORTF)


PORT_InitTypeDef PORT_InitStructure;

void Init_All_Ports(void)
{
	/* Enable the RTCHSE clock on all ports */
	RST_CLK_PCLKcmd(ALL_PORTS_CLK, ENABLE);

	/* Configure all ports to the state as after reset, i.e. low power consumption */
	PORT_StructInit(&PORT_InitStructure);
	PORT_Init(MDR_PORTA, &PORT_InitStructure);
	PORT_Init(MDR_PORTB, &PORT_InitStructure);
	PORT_Init(MDR_PORTC, &PORT_InitStructure);
	PORT_Init(MDR_PORTD, &PORT_InitStructure);
	PORT_Init(MDR_PORTE, &PORT_InitStructure);
	PORT_Init(MDR_PORTF, &PORT_InitStructure);

	/* Disable the RTCHSE clock on all ports */
	RST_CLK_PCLKcmd(ALL_PORTS_CLK, DISABLE);
}


/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */

int main(void)

{
	/*!< Usually, reset is done before the program is to be loaded into microcontroller,
	   and there is no need to perform any special operations to switch the ports
	   to low power consumption mode explicitly. So, the function Init_All_Ports
	   is used here for demonstration purpose only.
	*/
	Init_All_Ports();

	/* Enables the RTCHSE clock on PORTE and PORTF */
	RST_CLK_PCLKcmd(RST_CLK_PCLK_PORTA | RST_CLK_PCLK_PORTD, ENABLE);

	/* Configure PORTF pins 0,1 for output to switch LEDs on/off */

	PORT_InitStructure.PORT_Pin   = (PORT_Pin_7);
	PORT_InitStructure.PORT_OE    = PORT_OE_OUT;
	PORT_InitStructure.PORT_FUNC  = PORT_FUNC_PORT;
	PORT_InitStructure.PORT_MODE  = PORT_MODE_DIGITAL;
	PORT_InitStructure.PORT_SPEED = PORT_SPEED_SLOW;

	PORT_Init(MDR_PORTA, &PORT_InitStructure);

	PORT_InitStructure.PORT_Pin   = (PORT_Pin_7);
	PORT_InitStructure.PORT_OE    = PORT_OE_OUT;
	PORT_InitStructure.PORT_FUNC  = PORT_FUNC_PORT;
	PORT_InitStructure.PORT_MODE  = PORT_MODE_DIGITAL;
	PORT_InitStructure.PORT_SPEED = PORT_SPEED_SLOW;

	PORT_Init(MDR_PORTD, &PORT_InitStructure);

	PORT_SetBits(MDR_PORTA, PORT_Pin_7);

	PORT_SetBits(MDR_PORTD, PORT_Pin_7);

/*****************************************************************************************/

/*****************************************************************************************/

	PORT_InitTypeDef PortInit;
	UART_InitTypeDef UART_InitStructure;
	uint8_t DataByte=0x00;

	/* Enables the HSI clock on PORTB,PORTD */
	RST_CLK_PCLKcmd(RST_CLK_PCLK_PORTB,ENABLE);
	RST_CLK_PCLKcmd(RST_CLK_PCLK_PORTD,ENABLE);

	/* Fill PortInit structure*/
	PortInit.PORT_PULL_UP = PORT_PULL_UP_OFF;
	PortInit.PORT_PULL_DOWN = PORT_PULL_DOWN_OFF;
	PortInit.PORT_PD_SHM = PORT_PD_SHM_OFF;
	PortInit.PORT_PD = PORT_PD_DRIVER;
	PortInit.PORT_GFEN = PORT_GFEN_OFF;
	PortInit.PORT_FUNC = PORT_FUNC_ALTER;
	PortInit.PORT_SPEED = PORT_SPEED_MAXFAST;
	PortInit.PORT_MODE = PORT_MODE_DIGITAL;

	/* Configure PORTB pins 5 (UART1_TX) as output */
	PortInit.PORT_OE = PORT_OE_OUT;
	PortInit.PORT_Pin = PORT_Pin_5;
	PORT_Init(MDR_PORTB, &PortInit);

	/* Configure PORTB pins 6 (UART1_RX) as input */
	PortInit.PORT_OE = PORT_OE_IN;
	PortInit.PORT_Pin = PORT_Pin_6;
	PORT_Init(MDR_PORTB, &PortInit);

	/* Configure PORTD pins 1 (UART2_TX) as output */
	PortInit.PORT_OE = PORT_OE_OUT;
	PortInit.PORT_Pin = PORT_Pin_1;
	PORT_Init(MDR_PORTD, &PortInit);

	/* Configure PORTD pins 0 (UART1_RX) as input */
	PortInit.PORT_OE = PORT_OE_IN;
	PortInit.PORT_Pin = PORT_Pin_0;
	PORT_Init(MDR_PORTD, &PortInit);

	/* Select HSI/2 as CPU_CLK source*/
	RST_CLK_CPU_PLLconfig (RST_CLK_CPU_PLLsrcHSIdiv2,0);

	/* Enables the CPU_CLK clock on UART1,UART2 */
	RST_CLK_PCLKcmd(RST_CLK_PCLK_UART1, ENABLE);
	RST_CLK_PCLKcmd(RST_CLK_PCLK_UART2, ENABLE);

	/* Set the HCLK division factor = 1 for UART1,UART2*/
	UART_BRGInit(MDR_UART1, UART_HCLKdiv1);
	UART_BRGInit(MDR_UART2, UART_HCLKdiv1);

	/* Initialize UART_InitStructure */
	UART_InitStructure.UART_BaudRate                = 1500000;
	UART_InitStructure.UART_WordLength              = UART_WordLength8b;
	UART_InitStructure.UART_StopBits                = UART_StopBits2;
	UART_InitStructure.UART_Parity                  = UART_Parity_Even;
	UART_InitStructure.UART_FIFOMode                = UART_FIFO_OFF;
	UART_InitStructure.UART_HardwareFlowControl     = UART_HardwareFlowControl_RXE | UART_HardwareFlowControl_TXE;

	/* Configure UART1 parameters*/
	UART_Init (MDR_UART1,&UART_InitStructure);

	/* Enables UART1 peripheral */
	UART_Cmd(MDR_UART1,ENABLE);

	/* Configure UART2 parameters*/
	UART_Init (MDR_UART2,&UART_InitStructure);

	/* Enables UART2 peripheral */
	UART_Cmd(MDR_UART2,ENABLE);

	while (1) {
		/* Check TXFE flag*/
		while (UART_GetFlagStatus (MDR_UART1, UART_FLAG_TXFE)!= SET) {
		}

		/* Send Data from UART2 */
		UART_SendData (MDR_UART1,DataByte);

		/* Check RXFF flag*/
		while (UART_GetFlagStatus (MDR_UART1, UART_FLAG_RXFF)!= SET) {
		}

		/* Recive data*/
		DataByte = UART_ReceiveData (MDR_UART1);

	}
/*****************************************************************************************/
#define BufferSize         32
	SSP_InitTypeDef sSSP;
	PORT_InitTypeDef PORT_InitStructure;
	uint16_t spi_buf[BufferSize];

	uint8_t TxIdx = 0, RxIdx = 0;

	PORT_DeInit(MDR_PORTF);

	/* Configure SSP1 pins: FSS, CLK, RXD, TXD */

	/* Configure PORTF pins 0, 1, 2, 3 */
	PORT_InitStructure.PORT_Pin   = (PORT_Pin_3);
	PORT_InitStructure.PORT_OE    = PORT_OE_IN;
	PORT_InitStructure.PORT_FUNC  = PORT_FUNC_ALTER;
	PORT_InitStructure.PORT_MODE  = PORT_MODE_DIGITAL;
	PORT_InitStructure.PORT_SPEED = PORT_SPEED_FAST;

	PORT_Init(MDR_PORTF, &PORT_InitStructure);

	PORT_InitStructure.PORT_Pin   = (PORT_Pin_0 | PORT_Pin_1);
	PORT_InitStructure.PORT_OE    = PORT_OE_OUT;

	PORT_Init(MDR_PORTF, &PORT_InitStructure);

	PORT_InitStructure.PORT_Pin   = (PORT_Pin_2);
	PORT_InitStructure.PORT_OE    = PORT_OE_OUT;
	PORT_InitStructure.PORT_FUNC  = PORT_FUNC_PORT;
	PORT_InitStructure.PORT_MODE  = PORT_MODE_DIGITAL;
	PORT_InitStructure.PORT_SPEED = PORT_SPEED_SLOW;

	PORT_Init(MDR_PORTF, &PORT_InitStructure);

//	/* Init RAM */
//	Init_RAM (DstBuf1, BufferSize);
//	Init_RAM (SrcBuf1, BufferSize);

	/* Reset all SSP settings */
	SSP_DeInit(MDR_SSP1);

	SSP_BRGInit(MDR_SSP1,SSP_HCLKdiv16);

	/* SSP1 MASTER configuration ------------------------------------------------*/
	SSP_StructInit (&sSSP);

	sSSP.SSP_SCR  = 0x10;
	sSSP.SSP_CPSDVSR = 2;
	sSSP.SSP_Mode = SSP_ModeMaster;
	sSSP.SSP_WordLength = SSP_WordLength16b;
	sSSP.SSP_SPH = SSP_SPH_2Edge;
	sSSP.SSP_SPO = SSP_SPO_High;
	sSSP.SSP_FRF = SSP_FRF_SPI_Motorola;
//	sSSP.SSP_HardwareFlowControl = SSP_HardwareFlowControl_SSE;
	sSSP.SSP_HardwareFlowControl = SSP_HardwareFlowControl_None;
	SSP_Init (MDR_SSP1,&sSSP);

	/* Enable SSP1 */
	SSP_Cmd(MDR_SSP1, ENABLE);

	/* Transfer procedure */
	while (TxIdx < BufferSize) {
		PORT_ResetBits(MDR_PORTF, PORT_Pin_2);

		/* Wait for SPI1 Tx buffer empty */
		while (SSP_GetFlagStatus(MDR_SSP1, SSP_FLAG_TFE) == RESET) {
		}
		/* Send SPI1 data */
		SSP_SendData(MDR_SSP1, spi_buf[TxIdx++]);

		/* Wait for SPI1 data reception */
		while (SSP_GetFlagStatus(MDR_SSP1, SSP_FLAG_RNE) == RESET) {
		}

		/* Read SPI1 received data */
		spi_buf[RxIdx++] = SSP_ReceiveData(MDR_SSP1);

		PORT_SetBits(MDR_PORTF, PORT_Pin_2);
	}

/*****************************************************************************************/
	/*
	1. REVISON
	2. CHIPRES, OSCOKIRQ
	3. HOST, DPPULLDN, DMPULLDN
	4. BUSRST, SOFKAENAB, FRAMEIRQ
	5. CONDETIRQ, SAMPLEBUS, JSTATUS, KTATUS
	6. SETUP HS-IN
	7. BULK-IN
	*/
/*****************************************************************************************/

	while(1)
	{}

	return 0;
}
