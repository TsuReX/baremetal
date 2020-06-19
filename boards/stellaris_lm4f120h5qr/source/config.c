/**
 * @file	config.h
 *
 * @brief	Определения функций инициализации SoC и периферии.
 *
 * @author	Vasily Yurchenko <vasily.v.yurchenko@yandex.ru>
 */
#include "TIVA.h"
#include "config.h"
#include "hw_sysctl.h"

/* Частота ядра процессора.*/
#define CORE_FREQ 80000000

/**
 * @brief	Настраивает внутреннюю флеш память для корректного взаимодействия с ядром,
 * 			работающем на частоте 80 МГц.
 */
static void flash_config(void)
{
	/* Настройка времени задержки доступа к флешке.
	 * Это необходимо для корректной работы флешки при различных частотах HCLK.
	 */
//	LL_FLASH_SetLatency(LL_FLASH_LATENCY_2);
}

/**
 * @brief	Производит настройку источников тактирования
 */
static void rcc_config(void)
{
	SYSCTL->RCC = 0;
	SYSCTL_RCC_MOSCDIS;
	SYSCTL_RCC_OSCSRC_MAIN;
	SYSCTL_RCC_XTAL_16MHZ;
	SYSCTL_RCC_BYPASS;
	SYSCTL_RCC_PWRDN;
	SYSCTL_RCC_USESYSDIV;
	SYSCTL_RCC_SYSDIV_S;
	SYSCTL_RCC_ACG;

	SYSCTL_RCC2_OSCSRC2_MO;
	SYSCTL_RCC2_BYPASS2;
	SYSCTL_RCC2_PWRDN2;
	SYSCTL_RCC2_USBPWRDN;
	SYSCTL_RCC2_SYSDIV2LSB;
	SYSCTL_RCC2_SYSDIV2_S;
	SYSCTL_RCC2_DIV400;
	SYSCTL_RCC2_USERCC2;

	SYSCTL->PLLSTAT;
	SYSCTL_PLLSTAT;

	SYSCTL->PLLFREQ0;
	SYSCTL_PLLFREQ0_MINT_S;
	SYSCTL_PLLFREQ0_MFRAC_S;

	SYSCTL->PLLFREQ1;
	SYSCTL_PLLFREQ1_N_S;
	SYSCTL_PLLFREQ1_Q_S;

}

/**
 * @brief	Наcтраивает системный таймер ядра (SysTick)
 *
 * @param	freq	частота шины HCLK в герцах
 */
static void systick_config(uint32_t freq)
{
	/* Производится настройка системного таймера ядра для определения интервала времени равного 1 миллисекунде.  */
//	LL_Init1msTick(hclk_freq);
}

/**
 * @brief	Настройка устройств платформы(платы)
 */
void board_config(void)
{

}

/**
 * @brief	Настройка внутренних подсистем системы на кристалле.
 */
void soc_config(void)
{
	/* Настройка внутренней флеш памяти. */
	flash_config();
	/* Настройка подсистемы тактирования. */
	rcc_config();
	/* Настраивает системный таймер ядра. */
	systick_config(CORE_FREQ);

}

