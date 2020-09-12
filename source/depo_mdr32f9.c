#include "drivers.h"
#include "console.h"

#define BufferSize         32

#define ALL_PORTS_CLK (RST_CLK_PCLK_PORTA | RST_CLK_PCLK_PORTB | \
                       RST_CLK_PCLK_PORTC | RST_CLK_PCLK_PORTD | \
                       RST_CLK_PCLK_PORTE | RST_CLK_PCLK_PORTF)

/* Max delay can be used in LL_mDelay */
#define LL_MAX_DELAY                  0xFFFFFFFFU

#define HCLKFrequency 16000000

__STATIC_INLINE void LL_InitTick(uint32_t HCLKFreq, uint32_t Ticks)
{
  /* Configure the SysTick to have interrupt in 1ms time base */
  SysTick->LOAD  = (uint32_t)((HCLKFreq / Ticks) - 1UL);  /* set reload register */
  SysTick->VAL   = 0UL;                                       /* Load the SysTick Counter Value */
  SysTick->CTRL  = SysTick_CTRL_CLKSOURCE_Msk |
                   SysTick_CTRL_ENABLE_Msk;                   /* Enable the Systick Timer */
}

void LL_mDelay(uint32_t Delay)
{
  __IO uint32_t  tmp = SysTick->CTRL;  /* Clear the COUNTFLAG first */
  /* Add this code to indicate that local variable is not used */
  ((void)tmp);

  /* Add a period to guaranty minimum wait */
  if (Delay < LL_MAX_DELAY)
  {
    Delay++;
  }

  while (Delay)
  {
    if ((SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk) != 0U)
    {
      Delay--;
    }
  }
}

void spi_init()
{
	SSP_DeInit(MDR_SSP1);

	SSP_BRGInit(MDR_SSP1, SSP_HCLKdiv16);

	SSP_InitTypeDef ssp_descriptor;
	SSP_StructInit (&ssp_descriptor);

	ssp_descriptor.SSP_SCR  = 0x10;
	ssp_descriptor.SSP_CPSDVSR = 2;
	ssp_descriptor.SSP_Mode = SSP_ModeMaster;
	ssp_descriptor.SSP_WordLength = SSP_WordLength16b;
	ssp_descriptor.SSP_SPH = SSP_SPH_2Edge;
	ssp_descriptor.SSP_SPO = SSP_SPO_High;
	ssp_descriptor.SSP_FRF = SSP_FRF_SPI_Motorola;
//	sSSP.SSP_HardwareFlowControl = SSP_HardwareFlowControl_SSE;
	ssp_descriptor.SSP_HardwareFlowControl = SSP_HardwareFlowControl_None;

	SSP_Init (MDR_SSP1,&ssp_descriptor);

	/* Enable SSP1 */
	SSP_Cmd(MDR_SSP1, ENABLE);

}

static uint32_t spi_transmit_byte(uint8_t data)
{
	size_t i = 1000;

	while (SSP_GetFlagStatus(MDR_SSP1, SSP_FLAG_TFE) == RESET && i-- != 0);

	if (i == 0)
		return 0;
	SSP_SendData(MDR_SSP1, data);

	return 1;
}

static uint32_t spi_receive_byte(uint8_t *data)
{
	size_t i = 1000;

	while (SSP_GetFlagStatus(MDR_SSP1, SSP_FLAG_RNE) == RESET && i-- != 0);

	if (i == 0)
		return 0;
	*data = SSP_ReceiveData(MDR_SSP1);

	return 1;
}

void spi_test(void)
{
#define USB_REV_BYTES 1
#define SPI_CMD_RDREV 1

	uint8_t	buffer[USB_REV_BYTES + 1] = {0xFF, 0xFF};
	size_t	byte_idx;

	buffer[0] = SPI_CMD_RDREV;

	PORT_ResetBits(MDR_PORTF, PORT_Pin_2);
	for (byte_idx = 0; byte_idx < sizeof(buffer) / sizeof(buffer[0]); ++byte_idx) {

		if (spi_transmit_byte(buffer[byte_idx]) == 1)
			if (spi_receive_byte(&buffer[byte_idx]) == 0)
				break;

	}
	PORT_SetBits(MDR_PORTF, PORT_Pin_2);

	d_print("MC_SPI_FLASH_ID (Hex) ");
	for (byte_idx = 1; byte_idx < sizeof(buffer) / sizeof(buffer[0]); ++byte_idx)
		d_print("%02X ", buffer[byte_idx]);
}

void gpio_init(void)
{
	/*General initialization*/
	RST_CLK_PCLKcmd(ALL_PORTS_CLK, ENABLE);

	PORT_InitTypeDef port_descriptor;
	PORT_StructInit(&port_descriptor);

	PORT_Init(MDR_PORTA, &port_descriptor);
	PORT_Init(MDR_PORTB, &port_descriptor);
	PORT_Init(MDR_PORTC, &port_descriptor);
	PORT_Init(MDR_PORTD, &port_descriptor);
	PORT_Init(MDR_PORTE, &port_descriptor);
	PORT_Init(MDR_PORTF, &port_descriptor);

	RST_CLK_PCLKcmd(ALL_PORTS_CLK, DISABLE);

	/****************************************************/
	/*Control*/
	RST_CLK_PCLKcmd(RST_CLK_PCLK_PORTA | RST_CLK_PCLK_PORTD, ENABLE);

	port_descriptor.PORT_Pin   = (PORT_Pin_7);
	port_descriptor.PORT_OE    = PORT_OE_OUT;
	port_descriptor.PORT_FUNC  = PORT_FUNC_PORT;
	port_descriptor.PORT_MODE  = PORT_MODE_DIGITAL;
	port_descriptor.PORT_SPEED = PORT_SPEED_SLOW;

	PORT_Init(MDR_PORTA, &port_descriptor);
	PORT_Init(MDR_PORTD, &port_descriptor);

	PORT_SetBits(MDR_PORTA, PORT_Pin_7);

	/****************************************************/
	/*SPI*/
	RST_CLK_PCLKcmd(RST_CLK_PCLK_PORTF, ENABLE);

	port_descriptor.PORT_Pin   = (PORT_Pin_3);
	port_descriptor.PORT_OE    = PORT_OE_IN;
	port_descriptor.PORT_FUNC  = PORT_FUNC_ALTER;
	port_descriptor.PORT_MODE  = PORT_MODE_DIGITAL;
	port_descriptor.PORT_SPEED = PORT_SPEED_FAST;

	PORT_Init(MDR_PORTF, &port_descriptor);

	port_descriptor.PORT_Pin   = (PORT_Pin_0 | PORT_Pin_1);
	port_descriptor.PORT_OE    = PORT_OE_OUT;

	PORT_Init(MDR_PORTF, &port_descriptor);

	port_descriptor.PORT_Pin   = (PORT_Pin_2);
	port_descriptor.PORT_OE    = PORT_OE_OUT;
	port_descriptor.PORT_FUNC  = PORT_FUNC_PORT;
	port_descriptor.PORT_MODE  = PORT_MODE_DIGITAL;
	port_descriptor.PORT_SPEED = PORT_SPEED_SLOW;

	PORT_Init(MDR_PORTF, &port_descriptor);

	/****************************************************/
	/*USART 1,2*/
	RST_CLK_PCLKcmd(RST_CLK_PCLK_PORTB,ENABLE);

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

	/* Configure PORTD pins 1 (UART2_TX) as output */
	port_descriptor.PORT_OE = PORT_OE_OUT;
	port_descriptor.PORT_Pin = PORT_Pin_1;
	PORT_Init(MDR_PORTD, &port_descriptor);

	/* Configure PORTD pins 0 (UART1_RX) as input */
	port_descriptor.PORT_OE = PORT_OE_IN;
	port_descriptor.PORT_Pin = PORT_Pin_0;
	PORT_Init(MDR_PORTD, &port_descriptor);
}

/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */

int main(void)
{
	RST_CLK_CPU_PLLconfig (RST_CLK_CPU_PLLsrcHSIdiv1, RST_CLK_CPU_PLLmul16);

	LL_InitTick(HCLKFrequency, 1000U);
/*****************************************************************************************/
	gpio_init();

	console_init();
/*****************************************************************************************/

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
	{
		PORT_SetBits(MDR_PORTD, PORT_Pin_7);
		LL_mDelay(500);
		PORT_ResetBits(MDR_PORTD, PORT_Pin_7);
		LL_mDelay(500);
	}

	return 0;
}
