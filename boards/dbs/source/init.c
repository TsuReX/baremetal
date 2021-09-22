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
#if defined(F407VET6)

	LL_FLASH_SetLatency(LL_FLASH_LATENCY_3);
	while(LL_FLASH_GetLatency()!= LL_FLASH_LATENCY_3) {
	}
	LL_PWR_SetRegulVoltageScaling(LL_PWR_REGU_VOLTAGE_SCALE1);
	LL_RCC_HSE_Enable();

	/* Wait till HSE is ready */
	while(LL_RCC_HSE_IsReady() != 1) {

	}
	LL_RCC_PLL_ConfigDomain_SYS(LL_RCC_PLLSOURCE_HSE, LL_RCC_PLLM_DIV_4, 120, LL_RCC_PLLP_DIV_2);
	LL_RCC_PLL_ConfigDomain_48M(LL_RCC_PLLSOURCE_HSE, LL_RCC_PLLM_DIV_4, 120, LL_RCC_PLLQ_DIV_5);
	LL_RCC_PLL_Enable();

	/* Wait till PLL is ready */
	while(LL_RCC_PLL_IsReady() != 1) {

	}
	LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
	LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_4);
	LL_RCC_SetAPB2Prescaler(LL_RCC_APB2_DIV_2);
	LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL);

	/* Wait till System clock is ready */
	while(LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_PLL) {
	}
	LL_SetSystemCoreClock(120000000);

#elif defined(F407ZGT6)

	LL_FLASH_SetLatency(LL_FLASH_LATENCY_3);
	while(LL_FLASH_GetLatency()!= LL_FLASH_LATENCY_3)
	{
	}
	LL_PWR_SetRegulVoltageScaling(LL_PWR_REGU_VOLTAGE_SCALE1);
	LL_RCC_HSE_Enable();

	 /* Wait till HSE is ready */
	while(LL_RCC_HSE_IsReady() != 1)
	{

	}
	LL_RCC_PLL_ConfigDomain_SYS(LL_RCC_PLLSOURCE_HSE, LL_RCC_PLLM_DIV_12, 120, LL_RCC_PLLP_DIV_2);
	LL_RCC_PLL_Enable();

	 /* Wait till PLL is ready */
	while(LL_RCC_PLL_IsReady() != 1)
	{

	}
	LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
	LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_4);
	LL_RCC_SetAPB2Prescaler(LL_RCC_APB2_DIV_2);
	LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL);

	 /* Wait till System clock is ready */
	while(LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_PLL)
	{

	}
	LL_SetSystemCoreClock(120000000);

#endif
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
#if defined(F407VET6)
	/** Configuring GPIO. */
	LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOA);

	LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_6, LL_GPIO_MODE_OUTPUT);
	LL_GPIO_SetOutputPin(GPIOA, LL_GPIO_PIN_6);

	LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_7, LL_GPIO_MODE_OUTPUT);
	LL_GPIO_SetOutputPin(GPIOA, LL_GPIO_PIN_7);

#elif defined(F407ZGT6)

	LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOA);
	LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOB);
	LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOC);
	LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOD);
	LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOE);
	LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOG);
	LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOH);

	LL_GPIO_SetPinMode(ON_BTN_PORT, ON_BTN_PIN, LL_GPIO_MODE_INPUT);

	LL_GPIO_SetPinMode(PS_ON_BMC_PORT, PS_ON_BMC_PIN, LL_GPIO_MODE_OUTPUT);
	LL_GPIO_ResetOutputPin(PS_ON_BMC_PORT, PS_ON_BMC_PIN);

	LL_GPIO_SetPinMode(PWR_OK_BMC_PORT, PWR_OK_BMC_PIN, LL_GPIO_MODE_INPUT);

	LL_GPIO_SetPinMode(PWRGD_VDDIO_CPU0_PORT, PWRGD_VDDIO_CPU0_PIN, LL_GPIO_MODE_INPUT);

	LL_GPIO_SetPinMode(PWRGD_VDDIO_CPU1_PORT, PWRGD_VDDIO_CPU1_PIN, LL_GPIO_MODE_INPUT);

	LL_GPIO_SetPinMode(CPU0_VDDQABC_PWRGD_PORT, CPU0_VDDQABC_PWRGD_PIN, LL_GPIO_MODE_INPUT);

	LL_GPIO_SetPinMode(CPU0_VDDQDEF_PWRGD_PORT, CPU0_VDDQDEF_PWRGD_PIN, LL_GPIO_MODE_INPUT);

	LL_GPIO_SetPinMode(PWRGD_VPP_CPU0_ABC_PORT, PWRGD_VPP_CPU0_ABC_PIN, LL_GPIO_MODE_INPUT);

	LL_GPIO_SetPinMode(PWRGD_VPP_CPU0_DEF_PORT, PWRGD_VPP_CPU0_DEF_PIN, LL_GPIO_MODE_INPUT);

	LL_GPIO_SetPinMode(PWRGD_VTT_CPU0_ABC_PORT, PWRGD_VTT_CPU0_ABC_PIN, LL_GPIO_MODE_INPUT);

	LL_GPIO_SetPinMode(PWRGD_VTT_CPU0_DEF_PORT, PWRGD_VTT_CPU0_DEF_PIN, LL_GPIO_MODE_INPUT);

	LL_GPIO_SetPinMode(CPU1_VDDQ_PWRGD_PORT, CPU1_VDDQ_PWRGD_PIN, LL_GPIO_MODE_INPUT);

	LL_GPIO_SetPinMode(PWRGD_VPP_CPU1_PORT, PWRGD_VPP_CPU1_PIN, LL_GPIO_MODE_INPUT);

	LL_GPIO_SetPinMode(PWRGD_VTT_CPU1_PORT, PWRGD_VTT_CPU1_PIN, LL_GPIO_MODE_INPUT);

	LL_GPIO_SetPinMode(VDD_PCIE_VPH_CPU0_EN_PORT, VDD_PCIE_VPH_CPU0_EN_PIN, LL_GPIO_MODE_OUTPUT);
	LL_GPIO_ResetOutputPin(VDD_PCIE_VPH_CPU0_EN_PORT, VDD_PCIE_VPH_CPU0_EN_PIN);

	LL_GPIO_SetPinMode(PWRGD_VDD_PCIE_VPH_CPU0_PORT, PWRGD_VDD_PCIE_VPH_CPU0_PIN, LL_GPIO_MODE_INPUT);

	LL_GPIO_SetPinMode(VDD_PCIE_VP_CPU0_EN_PORT, VDD_PCIE_VP_CPU0_EN_PIN, LL_GPIO_MODE_OUTPUT);
	LL_GPIO_ResetOutputPin(VDD_PCIE_VP_CPU0_EN_PORT, VDD_PCIE_VP_CPU0_EN_PIN);

	LL_GPIO_SetPinMode(PWRGD_VDD_PCIE_VP_CPU0_PORT, PWRGD_VDD_PCIE_VP_CPU0_PIN, LL_GPIO_MODE_INPUT);

	LL_GPIO_SetPinMode(VDD_PCIE_VPH_CPU1_EN_PORT, VDD_PCIE_VPH_CPU1_EN_PIN, LL_GPIO_MODE_OUTPUT);
	LL_GPIO_ResetOutputPin(VDD_PCIE_VPH_CPU1_EN_PORT, VDD_PCIE_VPH_CPU1_EN_PIN);

	LL_GPIO_SetPinMode(PWRGD_VDD_PCIE_VPH_CPU1_PORT, PWRGD_VDD_PCIE_VPH_CPU1_PIN, LL_GPIO_MODE_INPUT);

	LL_GPIO_SetPinMode(VDD_PCIE_VP_CPU1_EN_PORT, VDD_PCIE_VP_CPU1_EN_PIN, LL_GPIO_MODE_OUTPUT);
	LL_GPIO_ResetOutputPin(VDD_PCIE_VP_CPU1_EN_PORT, VDD_PCIE_VP_CPU1_EN_PIN);

	LL_GPIO_SetPinMode(PWRGD_VDD_PCIE_VP_CPU1_PORT, PWRGD_VDD_PCIE_VP_CPU1_PIN, LL_GPIO_MODE_INPUT);

	LL_GPIO_SetPinMode(CPU0_VDD_PWRGD_PORT, CPU0_VDD_PWRGD_PIN, LL_GPIO_MODE_INPUT);

	LL_GPIO_SetPinMode(CPU1_VDD_PWRGD_PORT, CPU1_VDD_PWRGD_PIN, LL_GPIO_MODE_INPUT);

#endif

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

