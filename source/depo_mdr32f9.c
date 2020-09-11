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

void Init_All_Ports(void)
{
	PORT_InitTypeDef PORT_InitStructure;
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

void spi_init()
{
	PORT_InitTypeDef PORT_InitStructure;
	SSP_InitTypeDef sSSP;
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
	Init_All_Ports();

	PORT_InitTypeDef PORT_InitStructure;
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

//	PORT_SetBits(MDR_PORTD, PORT_Pin_7);


/*****************************************************************************************/
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
