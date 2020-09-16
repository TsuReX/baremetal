#include "drivers.h"
#include "console.h"

#define BufferSize         32

#define ALL_PORTS_CLK (RST_CLK_PCLK_PORTA | RST_CLK_PCLK_PORTB | \
                       RST_CLK_PCLK_PORTC | RST_CLK_PCLK_PORTD | \
                       RST_CLK_PCLK_PORTE | RST_CLK_PCLK_PORTF)

/* Max delay can be used in LL_mDelay */
#define LL_MAX_DELAY                  0xFFFFFFFFU

#define HCLKFrequency 8000000

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
	RST_CLK_PCLKcmd((RST_CLK_PCLK_RST_CLK | RST_CLK_PCLK_SSP1), ENABLE);

	SSP_DeInit(MDR_SSP1);

	uint32_t temp= MDR_RST_CLK->SSP_CLOCK;

	temp |= RST_CLK_SSP_CLOCK_SSP1_CLK_EN;
	temp &= ~RST_CLK_SSP_CLOCK_SSP1_BRG_Msk;
	temp |= SSP_HCLKdiv1;

	MDR_RST_CLK->SSP_CLOCK = temp;

	/* SSPx CPSR Configuration */
	MDR_SSP1->CPSR = 0x08;

	/* SSPx CR0 Configuration */
	MDR_SSP1->CR0 = (0x00 << SSP_CR0_SCR_Pos)
					| SSP_SPH_1Edge
					| SSP_SPO_Low
					| SSP_FRF_SPI_Motorola
					| SSP_WordLength8b;

	MDR_SSP1->CR1 = SSP_HardwareFlowControl_None | SSP_ModeMaster;

#define CR1_EN_Set	((uint16_t)0x0002)  /*!< SSP Enable Mask */
	MDR_SSP1->CR1 |= CR1_EN_Set;
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

void spi_fullduplex(void)
{
#define USB_REV_BYTES		0x01
#define SPI_USB_REVREG		0x12
#define SPI_USB_PINCTLREG	0x11
#define SPI_USB_REGNUMOFF	0x03
#define SPI_USB_WROP		(0x01 << 1)
#define SPI_USB_RDOP		(0x00 << 1)
#define SPI_CMD_RDREV		((SPI_USB_REVREG << SPI_USB_REGNUMOFF) | SPI_USB_RDOP)
#define SPI_CMD_WRPINCTL	((SPI_USB_PINCTLREG << SPI_USB_REGNUMOFF) | SPI_USB_WROP)

	uint8_t	buffer[2] = {SPI_CMD_WRPINCTL, 0x1F};
	size_t	byte_idx;


//	PORT_ResetBits(MDR_PORTE, PORT_Pin_0);
	PORT_ResetBits(MDR_PORTB, PORT_Pin_8);

	for (byte_idx = 0; byte_idx < sizeof(buffer) / sizeof(buffer[0]); ++byte_idx) {

		if (spi_transmit_byte(buffer[byte_idx]) == 0) {
			d_print("spi transmit error\r\n");
			break;
		}
	}
//	PORT_SetBits(MDR_PORTE, PORT_Pin_0);
	PORT_SetBits(MDR_PORTB, PORT_Pin_8);
}

void spi_test(void)
{
#define USB_REV_BYTES		0x01
#define SPI_USB_REVREG		0x12
#define SPI_USB_PINCTLREG	0x11
#define SPI_USB_REGNUMOFF	0x03
#define SPI_USB_WROP		(0x01 << 1)
#define SPI_USB_RDOP		(0x00 << 1)
#define SPI_CMD_RDREV		((SPI_USB_REVREG << SPI_USB_REGNUMOFF) | SPI_USB_RDOP)
#define SPI_CMD_WRPINCTL	((SPI_USB_PINCTLREG << SPI_USB_REGNUMOFF) | SPI_USB_WROP)

	uint8_t	buffer[2] = {SPI_CMD_RDREV, 0xFF};
	size_t	byte_idx;

//	PORT_ResetBits(MDR_PORTE, PORT_Pin_0);
	PORT_ResetBits(MDR_PORTB, PORT_Pin_8);

	for (byte_idx = 0; byte_idx < sizeof(buffer) / sizeof(buffer[0]); ++byte_idx) {

		if (spi_transmit_byte(buffer[byte_idx]) == 0) {
			d_print("spi transmit error\r\n");
			break;
		}
		buffer[byte_idx] = 0xFF;
		if (spi_receive_byte(&buffer[byte_idx]) == 0) {
			d_print("spi receive error\r\n");
			break;
		}

	}
//	PORT_SetBits(MDR_PORTE, PORT_Pin_0);
	PORT_SetBits(MDR_PORTB, PORT_Pin_8);

	d_print("MC_USB_REVISION (Hex) ");
	for (byte_idx = 0; byte_idx < sizeof(buffer) / sizeof(buffer[0]); ++byte_idx)
		d_print("%02X ", buffer[byte_idx]);
	d_print("\r\n");
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
	RST_CLK_PCLKcmd(RST_CLK_PCLK_PORTA | RST_CLK_PCLK_PORTB | RST_CLK_PCLK_PORTD | RST_CLK_PCLK_PORTE, ENABLE);
	port_descriptor.PORT_Pin   = (PORT_Pin_7);
	port_descriptor.PORT_OE    = PORT_OE_OUT;
	port_descriptor.PORT_FUNC  = PORT_FUNC_PORT;
	port_descriptor.PORT_MODE  = PORT_MODE_DIGITAL;
	port_descriptor.PORT_SPEED = PORT_SPEED_SLOW;

	/*PA7 PPS_ON*/
	PORT_Init(MDR_PORTA, &port_descriptor);
	/*PD7 DEBUG_LED*/
	PORT_Init(MDR_PORTD, &port_descriptor);

	port_descriptor.PORT_Pin   = (PORT_Pin_0);
	/*PE0 KM.KB_CS*/
	PORT_Init(MDR_PORTE, &port_descriptor);

	port_descriptor.PORT_Pin   = (PORT_Pin_8);
	/*PB8 KM.MS_CS*/
	PORT_Init(MDR_PORTB, &port_descriptor);

	PORT_SetBits(MDR_PORTA, PORT_Pin_7);
	PORT_SetBits(MDR_PORTE, PORT_Pin_0);
	PORT_SetBits(MDR_PORTB, PORT_Pin_8);

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

//	port_descriptor.PORT_Pin   = (PORT_Pin_2);
//	port_descriptor.PORT_OE    = PORT_OE_OUT;
//	port_descriptor.PORT_FUNC  = PORT_FUNC_PORT;
//	port_descriptor.PORT_MODE  = PORT_MODE_DIGITAL;
//	port_descriptor.PORT_SPEED = PORT_SPEED_SLOW;
//
//	PORT_Init(MDR_PORTF, &port_descriptor);

	/****************************************************/
	/*USART 1,2*/

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
//	RST_CLK_CPU_PLLconfig (RST_CLK_CPU_PLLsrcHSIdiv1, RST_CLK_CPU_PLLmul8);

	  /* Select CPUPLL source */
	uint32_t temp = MDR_RST_CLK->CPU_CLOCK;
	/* Clear CPU_C1_SEL bits */
	temp &= ~RST_CLK_CPU_CLOCK_CPU_C1_SEL_Msk;
	/* Set the CPU_C1_SEL bits */
	temp |= RST_CLK_CPU_PLLsrcHSIdiv1;
	/* Store the new value */
	MDR_RST_CLK->CPU_CLOCK = temp;

	/* Set CPUPLL multiplier */
	temp = MDR_RST_CLK->PLL_CONTROL;
	/* Clear PLLMUL[3:0] bits */
	temp &= ~RST_CLK_PLL_CONTROL_PLL_CPU_MUL_Msk;
	/* Set the PLLMUL[3:0] bits */
	temp |= (RST_CLK_CPU_PLLmul8 << RST_CLK_PLL_CONTROL_PLL_CPU_MUL_Pos);
	/* Store the new value */
	MDR_RST_CLK->PLL_CONTROL = temp;

	if( (MDR_RST_CLK->PLL_CONTROL & RST_CLK_PLL_CONTROL_PLL_CPU_ON) == RST_CLK_PLL_CONTROL_PLL_CPU_ON ) {
		temp = MDR_RST_CLK->PLL_CONTROL;
		temp |= RST_CLK_PLL_CONTROL_PLL_CPU_PLD;
		MDR_RST_CLK->PLL_CONTROL = temp;
		temp &= ~RST_CLK_PLL_CONTROL_PLL_CPU_PLD;
		MDR_RST_CLK->PLL_CONTROL = temp;
	}
/*****************************************************************************************/
	LL_InitTick(HCLKFrequency, 1000U);
/*****************************************************************************************/
	gpio_init();

	console_init();

	spi_init();
	spi_fullduplex();
	spi_test();
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
		spi_test();
	}

	return 0;
}
