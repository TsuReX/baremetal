/**
 * @file	src/config.c
 *
 * @brief	Определения функций инициализации SoC и периферии.
 *
 * @author	Vasily Yurchenko <vasily.v.yurchenko@yandex.ru>
 */

#include "init.h"
#include "drivers.h"
#include "config.h"

/* Счетчик интервалов системного времени. */
/* TODO: Продумать, куда можно перенести переменную */
volatile uint64_t system_ticks;

/* Период в микросекундах между отсчетами системного таймера. */
/* TODO: Продумать, куда можно перенести переменную */
uint32_t system_period;

void get_system_ticks(uint64_t *sys_ticks)
{
	*sys_ticks = system_ticks;
}

void get_system_tick_period(uint32_t *sys_period)
{
	*sys_period = system_period;
}

void systick_handler(void)
{
	++system_ticks;
	(void)SysTick->CTRL;
}

/**
 * @brief	Настраивает внутреннюю флеш память для корректного взаимодействия с ядром,
 * 			работающем на частоте 48 МГц.
 */
static void flash_init(void)
{
	/* Настройка времени задержки доступа к флешке.
	 * Это необходимо для корректной работы флешки при различных частотах HCLK.
	 */
	LL_FLASH_SetLatency(LL_FLASH_LATENCY_2);
}

/**
 * @brief TODO
 */
static void sysclk_init_48mhz_hse()
{
	/* Включение внутреннего источника тактирования HSE.
	 * HSI активен по умолчанию. */
	LL_RCC_HSE_Enable();
	/* Ожидание активации источника HSE. */
	while (LL_RCC_HSE_IsReady() != 1);

	/* Установка источника сигнала для PLL - PLLSRC.
	 * Установка множителя PLL - PLLMUL.*/
	LL_RCC_PLL_ConfigDomain_SYS(LL_RCC_PLLSOURCE_HSE, LL_RCC_PLL_MUL_6);

	/* Включение PLL. */
	LL_RCC_PLL_Enable();
	/* Ожидание активации PLL. */
	while (LL_RCC_PLL_IsReady() != 1);

	/* Настройка источника тактирования для SYSCLK. */
	LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL);
	/* Ожидание активации переключателя. */
	while (LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_PLL);
}

/**
 * @brief TODO
 */
#if 0
static void hsi_sysclk_24mhz_init()
{
	/* Включение внутреннего источника тактирования HSI.
	 * HSI активен по умолчанию. */
	LL_RCC_HSI_Enable();
	/* Ожидание активации источника HSI. */
	while (LL_RCC_HSI_IsReady() != 1);

	/* Установка источника сигнала для PLL - PLLSRC.
	 * Установка множителя PLL - PLLMUL.*/
	LL_RCC_PLL_ConfigDomain_SYS(LL_RCC_PLLSOURCE_HSI_DIV_2, LL_RCC_PLL_MUL_9);

	/* Включение PLL. */
	LL_RCC_PLL_Enable();
	/* Ожидание активации PLL. */
	while (LL_RCC_PLL_IsReady() != 1);

	/* Настройка источника тактирования для SYSCLK. */
	LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL);
	/* Ожидание активации переключателя. */
	while (LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_PLL);
}
#endif
/**
 * @brief TODO
 */
static void ahbclk_48mhz_init()
{
	/* Настройка делителя для шины AHB - HCLK. */
	LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
}

/**
 * @brief TODO
 */
static void apb1clk_24mhz_init()
{
	/* Настройка делителя для шины APB1 - PCLK1. */
	LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_2);
}

/**
 * @brief TODO
 */
static void apb2clk_24mhz_init()
{
	/* Настройка делителя для шины APB2 - PCLK2. */
	LL_RCC_SetAPB2Prescaler(LL_RCC_APB2_DIV_2);

	LL_RCC_SetADCClockSource(LL_RCC_ADC_CLKSRC_PCLK2_DIV_2);
}

/**
 * @brief TODO
 */
static void rtcclk_init()
{
	LL_RCC_SetRTCClockSource(LL_RCC_RTC_CLKSOURCE_LSI);
}

/**
 * @brief TODO
 */
static void mco_init_sysclk()
{
	LL_RCC_ConfigMCO(LL_RCC_MCO1SOURCE_SYSCLK);
}

/**
 * @brief	Производит настройку источников тактирования:
 * 				SYSCLK, AHBCLK, APB1CLK, APB2CLK, RTCCLK, MCO;
 *
 *			The system Clock is configured as follow :
 *				System Clock source		= PLL (HSE)
 *				SYSCLK (MHz)			= 48
 *				AHBCLK/HCLK (MHz)		= 48
 *				APB1CLK	(MHz)			= 24
 *				APB2CLK	(MHz)			= 24
 *				RTCCLK (KHz)			= 40
 */
static void rcc_init(void)
{
//	sysclk_init_24mhz_hsi();
	sysclk_init_48mhz_hse();

	ahbclk_48mhz_init();

	apb1clk_24mhz_init();

	apb2clk_24mhz_init();

	rtcclk_init();

	mco_init_sysclk();
}

/**
 * @brief	Наcтраивает системный таймер ядра (SysTick)
 *
 * @param[in]	hclk_freq	частота шины HCLK в герцах
 * @param[in]	period		период сребетывания таймера в микросекундах, кратный 10
 */
static void systick_init(uint32_t hclk_freq, uint32_t period)
{
	period = ((period + 5) / 10) * 10;
	if (period < 10)
		period = 10;
	if (period > 1000)
		period = 1000;
	system_ticks = 0;
	/* Производится настройка системного таймера ядра для определения интервала времени равного 100 микросекундам.  */

	uint32_t counter_value = (uint32_t)(((hclk_freq / 1000000) * period));

	while (counter_value < 50) {
		period *= 2;
		counter_value = (uint32_t)(((hclk_freq / 1000000) * period));
	}
	system_period = period;

	SysTick->LOAD  =	counter_value;/* Set reload register */
	SysTick->VAL   =	0UL;                                       	/* Load the SysTick Counter Value */
	SysTick->CTRL  =	SysTick_CTRL_CLKSOURCE_Msk |
						SysTick_CTRL_ENABLE_Msk |					/* Enable the Systick Timer */
						SysTick_CTRL_TICKINT_Msk;					/* Enable the Systick Interrupt */
}

/**
 * @brief	Настройка устройств платформы(платы)
 */
void board_init(void)
{
	/** Configuring GPIO. */
	LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOA);
	LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOB);
	LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOC);

	/* Green led. */
	LL_GPIO_SetPinSpeed(GPIOC, LL_GPIO_PIN_13, LL_GPIO_SPEED_FREQ_HIGH);
	LL_GPIO_SetPinMode(GPIOC, LL_GPIO_PIN_13, LL_GPIO_MODE_OUTPUT);

	/* Configure MAX3421E_0 SS# */
	LL_GPIO_SetPinSpeed(GPIOB, LL_GPIO_PIN_0, LL_GPIO_SPEED_FREQ_HIGH);
	LL_GPIO_SetPinMode(GPIOB, LL_GPIO_PIN_0, LL_GPIO_MODE_OUTPUT);

	/* Configure MAX3421E_1 SS# */
	LL_GPIO_SetPinSpeed(GPIOB, LL_GPIO_PIN_1, LL_GPIO_SPEED_FREQ_HIGH);
	LL_GPIO_SetPinMode(GPIOB, LL_GPIO_PIN_1, LL_GPIO_MODE_OUTPUT);

	/* Configure SPI FLASH SS# */
	LL_GPIO_SetPinSpeed(GPIOB, LL_GPIO_PIN_2, LL_GPIO_SPEED_FREQ_HIGH);
	LL_GPIO_SetPinMode(GPIOB, LL_GPIO_PIN_2, LL_GPIO_MODE_OUTPUT);

	/* MAX3421E_0 RESET_N */
	LL_GPIO_SetPinSpeed(GPIOA, LL_GPIO_PIN_3, LL_GPIO_SPEED_FREQ_HIGH);
	LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_3, LL_GPIO_MODE_OUTPUT);
	LL_GPIO_SetPinPull(GPIOA, LL_GPIO_PIN_3, LL_GPIO_PULL_UP);

	/* MAX3421E_1 RESET_N */
	LL_GPIO_SetPinSpeed(GPIOA, LL_GPIO_PIN_4, LL_GPIO_SPEED_FREQ_HIGH);
	LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_4, LL_GPIO_MODE_OUTPUT);
	LL_GPIO_SetPinPull(GPIOA, LL_GPIO_PIN_4, LL_GPIO_PULL_UP);



	/* Deassert MAX3421E_0 SS#. */
	LL_GPIO_SetOutputPin(GPIOB, LL_GPIO_PIN_0);

	/* Deassert MAX3421E_1 SS#. */
	LL_GPIO_SetOutputPin(GPIOB, LL_GPIO_PIN_1);

	/* Deassert SPI FLASH SS#. */
	LL_GPIO_SetOutputPin(GPIOB, LL_GPIO_PIN_2);

	/* Assert RESET_N from MAX3421E_0. */
	LL_GPIO_ResetOutputPin(GPIOA, LL_GPIO_PIN_3);

	/* Assert RESET_N from MAX3421E_1. */
	LL_GPIO_ResetOutputPin(GPIOA, LL_GPIO_PIN_4);

	/* Fire green led. */
	LL_GPIO_ResetOutputPin(GPIOC, LL_GPIO_PIN_13);

	/* MCO output enable. */
	LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_8, LL_GPIO_MODE_ALTERNATE);
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
	/* Настройка вспомогательных параметров. */
	LL_SetSystemCoreClock(HCLK_FREQ);
	/* Настраивает системный таймер ядра. */
	systick_init(HCLK_FREQ, 10);
}

