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
#define RCC_MOSCDIS		(0)
#define RCC_OSCSRC		(4)
#define RCC_XTAL		(6)
#define RCC_BYPASS		(11)
#define RCC_PWRDN		(13)
#define RCC_USESYSDIV	(22)
#define RCC_SYSDIV		(23)
#define RCC_ACG			(27)

#define RCC2_OSCSRC2	(4)
#define RCC2_BYPASS2	(11)
#define RCC2_PWRDN2		(13)
#define RCC2_USBPWRDN	(14)
#define RCC2_SYSDIV2LSB	(22)
#define RCC2_SYSDIV2	(23)
#define RCC2_DIV400		(30)
#define RCC2_USERCC2	(31)


	/* 1. Run-Mode Clock Configuration 2 */
	SYSCTL->RCC2 |= SYSCTL_RCC2_USERCC2;

	/* 2. Run-Mode Clock Configuration */
	SYSCTL->RCC &= ~SYSCTL_RCC_MOSCDIS;

	/* 3. Raw Interrupt Status */
	while (SYSCTL->RIS & SYSCTL_RIS_MOSCPUPRIS == 0);

	/* 4.  */
	SYSCTL->RCC |= SYSCTL_RCC_BYPASS;
	SYSCTL->RCC2 |= SYSCTL_RCC2_BYPASS2;

	SYSCTL->RCC |= SYSCTL_RCC_USESYSDIV;

	/* 5.  */
	SYSCTL->RCC |= SYSCTL_RCC_XTAL_16MHZ;

	SYSCTL->RCC &= ~SYSCTL_RCC_OSCSRC_M;
	SYSCTL->RCC2 &= ~SYSCTL_RCC2_OSCSRC2_M;

	SYSCTL->RCC &= ~SYSCTL_RCC_PWRDN;
	SYSCTL->RCC2 &= ~SYSCTL_RCC2_PWRDN2;

/* 2.1 */
	/* 400MHz = (16MHz * (49 + (1024 / 1204) ) / ((0 + 1) * (0 + 1))); */

	SYSCTL->PLLFREQ0 =	(1024 << SYSCTL_PLLFREQ0_MFRAC_S) |
						(49 << SYSCTL_PLLFREQ0_MINT_S);

	SYSCTL->PLLFREQ1 =	(0 << SYSCTL_PLLFREQ1_Q_S) |
						(0 << SYSCTL_PLLFREQ1_N_S);

	while (SYSCTL->PLLSTAT & SYSCTL_PLLSTAT_LOCK == 0);

/* 3.*/
	SYSCTL->RCC &= ~SYSCTL_RCC_USESYSDIV;
	SYSCTL->RCC2 |= SYSCTL_RCC2_DIV400;

	SYSCTL->RCC2 &= ~SYSCTL_RCC2_SYSDIV2_M;
	SYSCTL->RCC2 |= (0x4 << SYSCTL_RCC2_SYSDIV2_S);

/* 4.*/
	while (SYSCTL->RIS & SYSCTL_RIS_PLLLRIS == 0);

/* 5.*/
	SYSCTL->RCC &= ~SYSCTL_RCC_BYPASS;
	SYSCTL->RCC2 &= ~SYSCTL_RCC2_BYPASS2;

/* 6.*/
	SYSCTL->RCC &= ~SYSCTL_RCC_ACG;

/* 7.*/


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
#define RCGCGPIO_GPIOA			(0)
#define RCGCGPIO_GPIOB			(1)
#define RCGCGPIO_GPIOC			(2)
#define RCGCGPIO_GPIOD			(3)
#define RCGCGPIO_GPIOE			(4)
#define RCGCGPIO_GPIOF			(5)
#define RCGCGPIO_GPIOG			(6)
#define RCGCGPIO_GPIOH			(7)

#define GPIO_PIN_0				(0)
#define GPIO_PIN_1				(1)
#define GPIO_PIN_2				(2)
#define GPIO_PIN_3				(3)
#define GPIO_PIN_4				(4)
#define GPIO_PIN_5				(5)
#define GPIO_PIN_6				(6)
#define GPIO_PIN_7				(7)

#define GPIOPCTL_F0				(0x0)
#define GPIOPCTL_F1				(0x1)
#define GPIOPCTL_F2				(0x2)
#define GPIOPCTL_F3				(0x3)
#define GPIOPCTL_F4				(0x4)
#define GPIOPCTL_F5				(0x5)
#define GPIOPCTL_F6				(0x6)
#define GPIOPCTL_F7				(0x7)
#define GPIOPCTL_F8				(0x8)
#define GPIOPCTL_F9				(0x9)
#define GPIOPCTL_F10			(0xA)
#define GPIOPCTL_F11			(0xB)
#define GPIOPCTL_F12			(0xC)
#define GPIOPCTL_F13			(0xD)
#define GPIOPCTL_F14			(0xE)
#define GPIOPCTL_F15			(0xF)

	/* 1. General-Purpose Input/Output Run Mode Clock Gating Control */
	SYSCTL->RCGCGPIO |= (0x1 << RCGCGPIO_GPIOF);

	/* 2. GPIO Direction */
	GPIOF->DIR = 	(1 << GPIO_PIN_1) |
					(1 << GPIO_PIN_2) |
					(1 << GPIO_PIN_3);

	/* 3. GPIO Alternate Function Select */
	GPIOF->AFSEL =	(1 << GPIO_PIN_1) |
					(1 << GPIO_PIN_2) |
					(1 << GPIO_PIN_3);

	/* 4. GPIO Port Control */
	GPIOF->PCTL =	(GPIOPCTL_F0 << (GPIO_PIN_1 << 2)) |
					(GPIOPCTL_F0 << (GPIO_PIN_2 << 2)) |
					(GPIOPCTL_F0 << (GPIO_PIN_3 << 2));

	/* 5. GPIO Digital Enable */
	GPIOF->DEN =	(1 << GPIO_PIN_1) |
					(1 << GPIO_PIN_2) |
					(1 << GPIO_PIN_1);

	/* 6. GPIO 2-mA Drive Select */
	GPIOF->DR2R =	(1 << GPIO_PIN_3) |
					(1 << GPIO_PIN_2) |
					(1 << GPIO_PIN_1);

	/* 7. GPIO Pull-Up Select */
	GPIOF->PUR =	(1 << GPIO_PIN_3) |
					(1 << GPIO_PIN_2) |
					(1 << GPIO_PIN_1);

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

