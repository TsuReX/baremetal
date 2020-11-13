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

	SYSCTL->RCC2 |= (0x1 << 31);

	SYSCTL_RCC_MOSCDIS;
	SYSCTL->RCC &= ~(0x1 << 0);

	SYSCTL_RIS_MOSCPUPRIS;
	while (SYSCTL->RIS & (0x1 << 8) == 0);

/* 1. */
	SYSCTL_RCC_BYPASS;
	SYSCTL->RCC |= (0x1 << 11);
	SYSCTL->RCC2 |= (0x1 << 11);

	SYSCTL_RCC_USESYSDIV;
	SYSCTL->RCC |= (0x1 << 22);

/* 2.*/
	SYSCTL_RCC_XTAL_16MHZ;
	SYSCTL->RCC |= (0x15 << 6);

	SYSCTL_RCC_OSCSRC_MAIN;
	SYSCTL->RCC &= ~(0x3 << 4);
	SYSCTL->RCC2 &= ~(0x3 << 4);

	SYSCTL_RCC_PWRDN;
	SYSCTL->RCC &= ~(0x1 << 13);
	SYSCTL->RCC2 &= ~(0x1 << 13);

/* 2.1 */
	/* 400MHz = (16MHz * (49 + (1024 / 1204) ) / ((0 + 1) * (0 + 1))); */

	SYSCTL_PLLFREQ0_MFRAC_S;
	SYSCTL_PLLFREQ0_MINT_S;
	SYSCTL->PLLFREQ0 = (1024 << 10) | (49 << 0);

	SYSCTL_PLLFREQ1_Q_S;
	SYSCTL_PLLFREQ1_N_S;
	SYSCTL->PLLFREQ1 = (0 << 8) | (0 << 0);

	SYSCTL_PLLSTAT;
	while (SYSCTL->PLLSTAT == 0);

/* 3.*/
	SYSCTL_RCC_USESYSDIV;
	SYSCTL->RCC &= ~(0x1 << 22);
//
//	SYSCTL_RCC_SYSDIV_M;
//	SYSCTL->RCC &= ~(0xF << 23);
//	SYSCTL->RCC |= (0x1 << 23);
	SYSCTL_RCC2_DIV400;
	SYSCTL->RCC2 |= (0x1 << 30);

	SYSCTL->RCC2 &= ~(0x7F << 22);
	SYSCTL->RCC2 |= (0x4 << 22);

/* 4.*/
	SYSCTL_RIS_PLLLRIS;
	while (SYSCTL->RIS & (0x1 << 6) == 0);

/* 5.*/
	SYSCTL_RCC_BYPASS;
	SYSCTL->RCC &= ~(0x1 << 11);
	SYSCTL->RCC2 &= ~(0x1 << 11);

/* 6.*/
	SYSCTL_RCC_ACG;
	SYSCTL->RCC &= ~(0x1 << 27);

/* 7.*/


}

/**
 * @brief	Наcтраивает системный таймер ядра (SysTick)
 *
 * @param	freq	частота шины HCLK в герцах
 */
static void systick_config(uint32_t main_clk)
{
	/* Производится настройка системного таймера ядра для определения интервала времени равного 0.1 миллисекунде.  */
	SysTick_Config(main_clk / 10000);
	SysTick->CTRL  &= ~SysTick_CTRL_TICKINT_Msk;
}

/**
 * @brief	Настройка устройств платформы(платы)
 */
void board_init(void)
{
	/* 1. */
	SYSCTL->RCGCGPIO |= (0x1 << 5);
	/* 2. */
	GPIOF->DIR = (0x1 << 3) | (0x1 << 2) | (0x1 << 1);
	/* 3. */
	GPIOF->AFSEL = 0;
	GPIOF->PCTL = 0;
	GPIOF->DEN = (0x1 << 3) | (0x1 << 2) | (0x1 << 1);
	/* 4. */
	GPIOF->DR2R = (0x1 << 3) | (0x1 << 2) | (0x1 << 1);
	/* 5. */
	GPIOF->PUR = (0x1 << 3) | (0x1 << 2) | (0x1 << 1);
	GPIOF->PDR = 0;
	GPIOF->ODR = 0;
	GPIOF->SLR = 0;
	/* 6. */
	GPIOF->IS = 0;
	GPIOF->IBE = 0;
	GPIOF->IEV = 0;
	GPIOF->IM = 0;

	SYSCTL->RCGCUSB = 1;
}

/**
 * @brief	Настройка внутренних подсистем системы на кристалле.
 */
void soc_init(void)
{
	/* Настройка внутренней флеш памяти. */
	flash_config();
	/* Настройка подсистемы тактирования. */
	rcc_config();
	/* Настраивает системный таймер ядра. */
	systick_config(CORE_FREQ);

}

