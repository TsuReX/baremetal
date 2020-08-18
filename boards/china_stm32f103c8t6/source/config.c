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
#define HCLK_FREQ	48000000

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
//static void sysclk_init_24mhz()
//{
//	/* Включение внутреннего источника тактирования HSI.
//	 * HSI активен по умолчанию. */
//	LL_RCC_HSI_Enable();
//	/* Ожидание активации источника HSI. */
//	while (LL_RCC_HSI_IsReady() != 1);
//
//	/* Установка источника сигнала для PLL - PLLSRC.
//	 * Установка множителя PLL - PLLMUL.*/
//	LL_RCC_PLL_ConfigDomain_SYS(LL_RCC_PLLSOURCE_HSI_DIV_2, LL_RCC_PLL_MUL_6);
//
//	/* Включение PLL. */
//	LL_RCC_PLL_Enable();
//	/* Ожидание активации PLL. */
//	while (LL_RCC_PLL_IsReady() != 1);
//
//	/* Настройка источника тактирования для SYSCLK. */
//	LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL);
//	/* Ожидание активации переключателя. */
//	while (LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_PLL);
//}

/**
 * @brief TODO
 */
static void ahbclk_init_48mhz()
{
	/* Настройка делителя для шины AHB - HCLK. */
	LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
}

/**
 * @brief TODO
 */
static void apb1clk_init_24mhz()
{
	/* Настройка делителя для шины APB1 - PCLK1. */
	LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_2);
}

/**
 * @brief TODO
 */
static void apb2clk_init_24mhz()
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
//	sysclk_init_24mhz();
	sysclk_init_48mhz_hse();

	ahbclk_init_48mhz();

	apb1clk_init_24mhz();

	apb2clk_init_24mhz();

	rtcclk_init();

	mco_init_sysclk();
}

/**
 * @brief	Наcтраивает системный таймер ядра (SysTick)
 *
 * @param	hclk_freq	частота шины HCLK в герцах
 */
static void systick_init(uint32_t hclk_freq)
{
	/* Производится настройка системного таймера ядра для определения интервала времени равного 1 миллисекунде.  */
	LL_Init1msTick(hclk_freq);
}

/**
 * @brief	Настройка устройств платформы(платы)
 */
void board_init(void)
{
	/** Fire D2 led. */
	LL_GPIO_SetOutputPin(GPIOA, LL_GPIO_PIN_0);
	LL_GPIO_ResetOutputPin(GPIOC, LL_GPIO_PIN_13);
	LL_GPIO_ResetOutputPin(GPIOC, LL_GPIO_PIN_14);
	LL_GPIO_SetOutputPin(GPIOC, LL_GPIO_PIN_15);
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
	systick_init(SystemCoreClock);

	/** Configuring GPIO. */
	LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOA);
	LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOC);
	LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_0, LL_GPIO_MODE_OUTPUT);
	LL_GPIO_SetPinMode(GPIOC, LL_GPIO_PIN_13, LL_GPIO_MODE_OUTPUT);
	LL_GPIO_SetPinMode(GPIOC, LL_GPIO_PIN_14, LL_GPIO_MODE_OUTPUT);
	LL_GPIO_SetPinMode(GPIOC, LL_GPIO_PIN_15, LL_GPIO_MODE_OUTPUT);

	/* TODO: USB check*/
	LL_RCC_SetUSBClockSource(LL_RCC_USB_CLKSOURCE_PLL);
	LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_USB);

}

