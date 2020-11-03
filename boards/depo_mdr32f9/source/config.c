/**
 * @file	src/config.c
 *
 * @brief	Определения функций инициализации SoC и периферии.
 *
 * @author	Vasily Yurchenko <vasily.v.yurchenko@yandex.ru>
 */

#include "drivers.h"
#include "config.h"

/** Частота шины HCLK (работы ядра процессора). */
#define HCLK_FREQ	64000000

/**
 * @brief	Настраивает внутреннюю флеш память для корректного взаимодействия с ядром,
 * 			работающем на частоте 48 МГц.
 */
static void flash_init(void)
{
	/* Настройка времени задержки доступа к флешке.
	 * Это необходимо для корректной работы флешки при различных частотах HCLK.
	 */
//	LL_FLASH_SetLatency(LL_FLASH_LATENCY_2);
}

/**
 * @brief TODO
 */
static void sysclk_init_64mhz()
{
	RST_CLK_HSEconfig(RST_CLK_HSE_ON);
	if (RST_CLK_HSEstatus() == SUCCESS) {
		/* Good HSE clock */
		/* Select HSE clock as CPU_PLL input clock source */

		/* Set PLL multiplier to 8 */
		RST_CLK_CPU_PLLconfig(RST_CLK_CPU_PLLsrcHSEdiv1, RST_CLK_CPU_PLLmul8);

		/* Enable CPU_PLL */
		RST_CLK_CPU_PLLcmd(ENABLE);

		if (RST_CLK_HSEstatus() == SUCCESS) {
			/* Good CPU PLL */
			/* Set CPU_C3_prescaler to 1 */
			RST_CLK_CPUclkPrescaler(RST_CLK_CPUclkDIV1);

			/* Set CPU_C2_SEL to CPU_PLL output instead of CPU_C1 clock */
			RST_CLK_CPU_PLLuse(ENABLE);

			/* Select CPU_C3 clock on the CPU clock MUX */
			RST_CLK_CPUclkSelection(RST_CLK_CPUclkCPU_C3);

		} else {
			/* CPU_PLL timeout */
//			IndicateError();
		}

	} else {
		/* HSE timeout */
//		IndicateError();
	}
}

/**
 * @brief	Производит настройку источников тактирования:
 * 				SYSCLK, AHBCLK, APB1CLK, APB2CLK, RTCCLK, MCO;
 *
 *			The system Clock is configured as follow :
 *				System Clock source		= PLL (HSI)
 *				SYSCLK (MHz)			= 64
 *				AHBCLK/HCLK (MHz)		= 64
 *				APB1CLK	(MHz)			= 32
 *				APB2CLK	(MHz)			= 64
 *				ADCCLK (MHz)			= 8
 *				RTCCLK (KHz)			= 40
 */
static void rcc_init(void)
{
	sysclk_init_64mhz();
}

/**
 * @brief	Наcтраивает системный таймер ядра (SysTick)
 *
 * @param	hclk_freq	частота шины HCLK в герцах
 */
static void systick_init(uint32_t hclk_freq)
{
	/* Производится настройка системного таймера ядра для определения интервала времени равного 100 микросекундам.  */

	SysTick->LOAD  = (uint32_t)((hclk_freq / 10000) - 1UL);  /* set reload register */
	SysTick->VAL   = 0UL;                                       /* Load the SysTick Counter Value */
	SysTick->CTRL  = SysTick_CTRL_CLKSOURCE_Msk |
				   SysTick_CTRL_ENABLE_Msk;                   /* Enable the Systick Timer */
}

/**
 * @brief	Настройка устройств платформы(платы)
 */
void board_init(void)
{
	//	/*General initialization*/
	//	RST_CLK_PCLKcmd(ALL_PORTS_CLK, ENABLE);
	//
		PORT_InitTypeDef port_descriptor;
		PORT_StructInit(&port_descriptor);
	//
	//	PORT_Init(MDR_PORTA, &port_descriptor);
	//	PORT_Init(MDR_PORTB, &port_descriptor);
	//	PORT_Init(MDR_PORTC, &port_descriptor);
	//	PORT_Init(MDR_PORTD, &port_descriptor);
	//	PORT_Init(MDR_PORTE, &port_descriptor);
	//	PORT_Init(MDR_PORTF, &port_descriptor);
	//
	//	RST_CLK_PCLKcmd(ALL_PORTS_CLK, DISABLE);
		RST_CLK_PCLKcmd(RST_CLK_PCLK_PORTA, ENABLE);
		RST_CLK_PCLKcmd(RST_CLK_PCLK_PORTB, ENABLE);
		RST_CLK_PCLKcmd(RST_CLK_PCLK_PORTD, ENABLE);
		RST_CLK_PCLKcmd(RST_CLK_PCLK_PORTE, ENABLE);
		RST_CLK_PCLKcmd(RST_CLK_PCLK_PORTF, ENABLE);

		/****************************************************/
		/*Control*/
		port_descriptor.PORT_Pin   = (PORT_Pin_7);
		port_descriptor.PORT_OE    = PORT_OE_OUT;
		port_descriptor.PORT_FUNC  = PORT_FUNC_PORT;
		port_descriptor.PORT_MODE  = PORT_MODE_DIGITAL;
		port_descriptor.PORT_SPEED = PORT_SPEED_SLOW;

		/*PA7 PPS_ON*/
		PORT_Init(MDR_PORTA, &port_descriptor);
		/*PD7 DEBUG_LED*/
		PORT_Init(MDR_PORTD, &port_descriptor);

		/*PE0 KM.KB_CS*/
		port_descriptor.PORT_PULL_UP = PORT_PULL_UP_ON;
		port_descriptor.PORT_Pin   = (PORT_Pin_0);
		PORT_Init(MDR_PORTE, &port_descriptor);
		PORT_SetBits(MDR_PORTE, PORT_Pin_0);
	//	/*PB8 KM.MS_CS*/
		port_descriptor.PORT_PULL_UP = PORT_PULL_UP_ON;
		port_descriptor.PORT_Pin   = (PORT_Pin_8);
		PORT_Init(MDR_PORTB, &port_descriptor);
		PORT_SetBits(MDR_PORTB, PORT_Pin_8);

		/*PB6 DEBUG*/
		port_descriptor.PORT_Pin   = (PORT_Pin_6);
		PORT_Init(MDR_PORTB, &port_descriptor);
		PORT_ResetBits(MDR_PORTB, PORT_Pin_6);


		PORT_SetBits(MDR_PORTA, PORT_Pin_7);

	//	PORT_SetBits(MDR_PORTB, PORT_Pin_8);

		/****************************************************/
		/*SPI*/

		port_descriptor.PORT_Pin   = (PORT_Pin_3);
		port_descriptor.PORT_OE    = PORT_OE_IN;
		port_descriptor.PORT_FUNC  = PORT_FUNC_ALTER;
		port_descriptor.PORT_MODE  = PORT_MODE_DIGITAL;
		port_descriptor.PORT_SPEED = PORT_SPEED_FAST;

		PORT_Init(MDR_PORTF, &port_descriptor);

		port_descriptor.PORT_Pin   = (PORT_Pin_0 | PORT_Pin_1);
		port_descriptor.PORT_OE    = PORT_OE_OUT;

		PORT_Init(MDR_PORTF, &port_descriptor);
	//
	//	port_descriptor.PORT_Pin   = (PORT_Pin_2);
	//	port_descriptor.PORT_OE    = PORT_OE_OUT;
	//	port_descriptor.PORT_FUNC  = PORT_FUNC_PORT;
	//	port_descriptor.PORT_MODE  = PORT_MODE_DIGITAL;
	//	port_descriptor.PORT_SPEED = PORT_SPEED_SLOW;
	//
	//	PORT_Init(MDR_PORTF, &port_descriptor);
	//
	//	/****************************************************/
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
}

/**
 * @brief	Настройка внутренних подсистем системы на кристалле.
 */
void soc_init(void)
{
	/* Настройка внутренней флеш памяти. */
	flash_init();
	/* Настройка подсистемы тактирования. */
	rcc_init();
	/* Настраивает системный таймер ядра. */
	systick_init(HCLK_FREQ);
}

