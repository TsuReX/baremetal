/**
 * @file	src/config.c
 *
 * @brief	Определения функций инициализации SoC и периферии.
 *
 * @author	Vasily Yurchenko <vasily.v.yurchenko@yandex.ru>
 */

#include "init.h"

#include "config.h"
#include "drivers.h"

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
	LL_FLASH_SetLatency(LL_FLASH_LATENCY_0);
}

/**
 * @brief TODO
 */

static void hse_sysclk_120mhz_init()
{
	LL_FLASH_SetLatency(LL_FLASH_LATENCY_5);
	while(LL_FLASH_GetLatency()!= LL_FLASH_LATENCY_5) {
	}
	LL_PWR_SetRegulVoltageScaling(LL_PWR_REGU_VOLTAGE_SCALE1);
	LL_PWR_EnableRange1BoostMode();
	LL_RCC_HSE_Enable();

	/* Wait till HSE is ready */
	while(LL_RCC_HSE_IsReady() != 1) {

	}
	LL_RCC_PLL_ConfigDomain_SYS(LL_RCC_PLLSOURCE_HSE, LL_RCC_PLLM_DIV_2, 30, LL_RCC_PLLR_DIV_2);
	LL_RCC_PLL_EnableDomain_SYS();
	LL_RCC_PLL_Enable();

	/* Wait till PLL is ready */
	while(LL_RCC_PLL_IsReady() != 1) {

	}

	/* Intermediate AHB prescaler 2 when target frequency clock is higher than 80 MHz */
	LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);

	LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL);

	/* Wait till System clock is ready */
	while(LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_PLL) {

	}

	/* Insure 1µs transition state at intermediate medium speed clock based on DWT*/
	CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;

	DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
	DWT->CYCCNT = 0;
	while(DWT->CYCCNT < 100);

	LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
	LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_1);
	LL_RCC_SetAPB2Prescaler(LL_RCC_APB2_DIV_1);

	LL_SetSystemCoreClock(120000000);
}


static void rcc_init(void)
{
	hse_sysclk_120mhz_init();
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
//	LL_SetSystemCoreClock(HCLK_FREQ);
	/* Настраивает системный таймер ядра. */
	systick_init(HCLK_FREQ, 10);
}

