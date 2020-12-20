/**
 * @file	config.h
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
 * @brief	Производит настройку источников тактирования
 */
static void rcc_init(void)
{
	/* 1. Enable using RCC2 */
	SYSCTL->RCC2 |= SYSCTL_RCC2_USERCC2;

	/* 2. Select MOSC crystal type */
	SYSCTL->RCC |= SYSCTL_RCC_XTAL_16MHZ;

	/* 3. Enable Main oscillator MOSC */
	SYSCTL->RCC &= ~SYSCTL_RCC_MOSCDIS;

	/* 4. Wait for MOSC clock stabilizes */
	while (SYSCTL->RIS & SYSCTL_RIS_MOSCPUPRIS == 0);

	/* 5. Use Precise Internal Oscillator as main clock source */
	SYSCTL->RCC |= SYSCTL_RCC_BYPASS;
	SYSCTL->RCC2 |= SYSCTL_RCC2_BYPASS2;


	/* 6. Use MOSC as main clock source */
	SYSCTL->RCC &= ~SYSCTL_RCC_OSCSRC_M;
	SYSCTL->RCC2 &= ~SYSCTL_RCC2_OSCSRC2_M;

	/* 7. Enable PLL */
	SYSCTL->RCC &= ~SYSCTL_RCC_PWRDN;
	SYSCTL->RCC2 &= ~SYSCTL_RCC2_PWRDN2;

	/* 8. Setup PLL clock frequency */
	/* 400MHz = (16MHz * (49 + (1024 / 1204) ) / ((0 + 1) * (0 + 1))); */

	SYSCTL->PLLFREQ0 =	(1024 << SYSCTL_PLLFREQ0_MFRAC_S) |
						(49 << SYSCTL_PLLFREQ0_MINT_S);

	SYSCTL->PLLFREQ1 =	(0 << SYSCTL_PLLFREQ1_Q_S) |
						(0 << SYSCTL_PLLFREQ1_N_S);

	/* 9. Wait for PLL clock stabilizes */
	while (SYSCTL->PLLSTAT & SYSCTL_PLLSTAT_LOCK == 0);

	/* 10. Ensure that PLL clocks normal*/
	while (SYSCTL->RIS & SYSCTL_RIS_PLLLRIS == 0);

	/* 11. Disable PLL clock division by 2 */
	SYSCTL->RCC2 |= SYSCTL_RCC2_DIV400;

	/* 12. Set up PLL clock divisor for 5 */
	SYSCTL->RCC2 &= ~SYSCTL_RCC2_SYSDIV2_M;
	SYSCTL->RCC2 |= (0x2 << SYSCTL_RCC2_SYSDIV2_S);
	SYSCTL->RCC2 &= ~SYSCTL_RCC2_SYSDIV2LSB;

	/* 13. Use clock divider as source for system clock */
	SYSCTL->RCC &= ~SYSCTL_RCC_USESYSDIV;

	/* 14. Use diveded PLL clock as system clock */
	SYSCTL->RCC &= ~SYSCTL_RCC_BYPASS;
	SYSCTL->RCC2 &= ~SYSCTL_RCC2_BYPASS2;

	/* 15. Disable automatic clock gating */
	SYSCTL->RCC &= ~SYSCTL_RCC_ACG;
}

/**
 * @brief	Наcтраивает системный таймер ядра (SysTick)
 *
 * @param[in]	core_freq	частота шины ядра в герцах
 * @param[in]	period		период сребетывания таймера в микросекундах, кратный 10
 */
static void systick_init(uint32_t core_freq, uint32_t period)
{
	period = ((period + 5) / 10) * 10;
	if (period < 10)
		period = 10;
	if (period > 1000)
		period = 1000;
	system_ticks = 0;
	/* Производится настройка системного таймера ядра для определения интервала времени равного 100 микросекундам.  */

	uint32_t counter_value = (uint32_t)(((core_freq / 1000000) * period));

	while (counter_value < 50) {
		period *= 2;
		counter_value = (uint32_t)(((core_freq / 1000000) * period));
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
	/* 1. General-Purpose Input/Output Run Mode Clock Gating Control */
	SYSCTL->RCGCGPIO |= (0x1 << RCGCGPIO_GPIOF);

	/* 2. GPIO Direction */
	GPIOF->DIR = 	(1 << GPIO_PIN_1) |
					(1 << GPIO_PIN_2) |
					(1 << GPIO_PIN_3);

	/* 3. GPIO Alternate Function Select */
	GPIOF->AFSEL =	(0 << GPIO_PIN_1) |
					(0 << GPIO_PIN_2) |
					(0 << GPIO_PIN_3);

	/* 4. GPIO Port Control */
	GPIOF->PCTL =	(GPIOPCTL_F0 << (GPIO_PIN_1 << 2)) |
					(GPIOPCTL_F0 << (GPIO_PIN_2 << 2)) |
					(GPIOPCTL_F0 << (GPIO_PIN_3 << 2));

	/* 5. GPIO Digital Enable */
	GPIOF->DEN =	(1 << GPIO_PIN_1) |
					(1 << GPIO_PIN_2) |
					(1 << GPIO_PIN_3);

	/* 6. GPIO 2-mA Drive Select */
	GPIOF->DR2R =	(1 << GPIO_PIN_3) |
					(1 << GPIO_PIN_2) |
					(1 << GPIO_PIN_1);

	/* 7. GPIO Pull-Up Select */
//	GPIOF->PUR =	(1 << GPIO_PIN_3) |
//					(1 << GPIO_PIN_2) |
//					(1 << GPIO_PIN_1);

	/* 8. GPIO Pull-Down Select */
	GPIOF->PDR = 0;

	/* 9. GPIO Open Drain Select */
	GPIOF->ODR = 0;

	/* 10. GPIO Slew Rate Control Select */
	GPIOF->SLR = 0;

	/* 11. GPIO Interrupt Sense */
	GPIOF->IS = 0;

	/* 12. GPIO Interrupt Both Edges */
	GPIOF->IBE = 0;

	/* 13. GPIO Interrupt Event */
	GPIOF->IEV = 0;

	/* 14. GPIO Interrupt Mask */
	GPIOF->IM = 0;

	/* 15. Universal Serial Bus Run Mode Clock Gating Control */
	SYSCTL->RCGCUSB = 1;
}

/**
 * @brief	Настройка внутренних подсистем системы на кристалле.
 */
void soc_init(void)
{
	/* Настройка подсистемы тактирования. */
	rcc_init();
	/* Настраивает системный таймер ядра. */
	systick_init(CORE_FREQ, 10);
}

