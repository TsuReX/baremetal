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
 * @brief	Производит настройку источников тактирования:
 * 				SYSCLK, PLLCLK;
 * 				HCLK, PCLK1, PCLK2;
 *
 *			The system Clock is configured as follow :
 *				System Clock source            = PLL (HSI)
 *				SYSCLK(Hz)                     = 48000000
 *				HCLK(Hz)                       = 48000000
 *				AHB Prescaler                  = 1
 *				APB1 Prescaler                 = 1
 *				APB2 Prescaler                 = 1
 *				PLLMUL                         = RCC_PLL_MUL12 (12)
 */
static void rcc_init(void)
{

	/* Настройка системного тактового сигнала SYSCLK.
	 * HSI ------------------------> or
	 * HSI -> /2 -----> |			 |
	 * 					|--->PLL---> or--> SYSCLK
	 * HSE -> PREDIV -> |			 |
	 * HSE ------------------------> or
	 * До того, как будет настроена новая цепь тактирования,
	 * SYSCLK будет тактироваться напрямую от внутреннего источника HSI.
	 */

	/* Включение внутреннего источника тактирования HSI.
	 * HSI активен по умолчанию. */
	LL_RCC_HSI_Enable();
	/* Ожидание активации источника HSI. */
	while (LL_RCC_HSI_IsReady() != 1);

	/* В случае наличия внешнего источника тактирования. */

	/* Включение внешнего источника тактирования HSE. */
	/* LL_RCC_HSE_Enable(); */
	/* Ожидание активации источника HSI. */
	/* while (LL_RCC_HSE_IsReady() != 1); */

	/* Установка источника сигнала для PLL - PLLSRC.
	 * Установка множителя PLL - PLLMUL.*/
	LL_RCC_PLL_ConfigDomain_SYS(LL_RCC_PLLSOURCE_HSI_DIV_2, LL_RCC_PLL_MUL_12);

	/* Включение PLL. */
	LL_RCC_PLL_Enable();
	/* Ожидание активации PLL. */
	while (LL_RCC_PLL_IsReady() != 1);

	/* Настройка источника тактирования для SYSCLK. */
	LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL);
	/* Ожидание активации переключателя. */
	while (LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_PLL);

	/* Системный источник тактирования SYSCLK настроен. */

	/* Настройка делителя для шины AHB - HCLK. */
	LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);

	/* Настройка делителя для шины APB1 - PCLK1. */
	LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_2);

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
	LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOA);

	/** PS_ON */
	LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_0, LL_GPIO_MODE_OUTPUT);
	LL_GPIO_SetOutputPin(GPIOA, LL_GPIO_PIN_0);

	/** RSMRST_N */
	LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_1, LL_GPIO_MODE_OUTPUT);
	LL_GPIO_ResetOutputPin(GPIOA, LL_GPIO_PIN_1);

	/** SOC_UART1 TX/RX */
//	LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_2, LL_GPIO_MODE_OUTPUT);
//	LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_3, LL_GPIO_MODE_OUTPUT);

	/** SYS_RED_LED */
	LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_4, LL_GPIO_MODE_OUTPUT);
	LL_GPIO_ResetOutputPin(GPIOA, LL_GPIO_PIN_4);

	/** SYS_GRN_LED */
	LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_5, LL_GPIO_MODE_OUTPUT);
	LL_GPIO_ResetOutputPin(GPIOA, LL_GPIO_PIN_5);

	/** FAN_PWM1 */
//	LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_6, LL_GPIO_MODE_OUTPUT);

	/** FAN_PWM2 */
//	LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_7, LL_GPIO_MODE_OUTPUT);

	/** SOC_SRTCRST_N */
	LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_8, LL_GPIO_MODE_INPUT);
//	LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_8, LL_GPIO_MODE_OUTPUT);
//	LL_GPIO_SetOutputPin(GPIOA, LL_GPIO_PIN_8);

	/** STM32_UART1 TX/RX */
//	LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_9, LL_GPIO_MODE_OUTPUT);
//	LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_10, LL_GPIO_MODE_OUTPUT);

	/** SOC_PWRBTN_N */
	LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_11, LL_GPIO_MODE_OUTPUT);
	LL_GPIO_SetOutputPin(GPIOA, LL_GPIO_PIN_11);

	/** SOC_RSTBTN_N */
	LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_12, LL_GPIO_MODE_OUTPUT);
	LL_GPIO_SetOutputPin(GPIOA, LL_GPIO_PIN_12);

	/** SWDIO */
//	LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_13, LL_GPIO_MODE_OUTPUT);
	/** SWCLK */
//	LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_14, LL_GPIO_MODE_OUTPUT);

	/** VCCREF */
	LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_15, LL_GPIO_MODE_OUTPUT);
	LL_GPIO_ResetOutputPin(GPIOA, LL_GPIO_PIN_15);

	/***************************************************/
	LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOB);

	/** CON TO GND */
//	LL_GPIO_SetPinMode(GPIOB, LL_GPIO_PIN_0, LL_GPIO_MODE_OUTPUT);

	/** V1P8 */
	LL_GPIO_SetPinMode(GPIOB, LL_GPIO_PIN_1, LL_GPIO_MODE_OUTPUT);
	LL_GPIO_ResetOutputPin(GPIOB, LL_GPIO_PIN_1);

	/** V3P3_EN */
	LL_GPIO_SetPinMode(GPIOB, LL_GPIO_PIN_2, LL_GPIO_MODE_OUTPUT);
	LL_GPIO_SetOutputPin(GPIOB, LL_GPIO_PIN_2);

	/** V1P05_PG */
	LL_GPIO_SetPinMode(GPIOB, LL_GPIO_PIN_3, LL_GPIO_MODE_INPUT);
	/** TEMPORAL! Until pull up resistor will be placed */
	LL_GPIO_SetPinPull(GPIOB, LL_GPIO_PIN_3, LL_GPIO_PULL_UP);

	/** VCCSRAM_EN */
	LL_GPIO_SetPinMode(GPIOB, LL_GPIO_PIN_4, LL_GPIO_MODE_OUTPUT);
	LL_GPIO_ResetOutputPin(GPIOB, LL_GPIO_PIN_4);

	/** VCCP_EN */
	LL_GPIO_SetPinMode(GPIOB, LL_GPIO_PIN_5, LL_GPIO_MODE_OUTPUT);
	LL_GPIO_ResetOutputPin(GPIOB, LL_GPIO_PIN_5);

	/** VNN_EN */
	LL_GPIO_SetPinMode(GPIOB, LL_GPIO_PIN_6, LL_GPIO_MODE_OUTPUT);
	LL_GPIO_ResetOutputPin(GPIOB, LL_GPIO_PIN_6);

	/** CON TO GND */
//	LL_GPIO_SetPinMode(GPIOB, LL_GPIO_PIN_7, LL_GPIO_MODE_OUTPUT);

	/** SMB_LEG_CLK */
//	LL_GPIO_SetPinMode(GPIOB, LL_GPIO_PIN_8, LL_GPIO_MODE_OUTPUT);

	/** SMB_LEG_DATA */
//	LL_GPIO_SetPinMode(GPIOB, LL_GPIO_PIN_9, LL_GPIO_MODE_OUTPUT);

	/** VPP_EN */
	LL_GPIO_SetPinMode(GPIOB, LL_GPIO_PIN_10, LL_GPIO_MODE_OUTPUT);
	LL_GPIO_ResetOutputPin(GPIOB, LL_GPIO_PIN_10);

	/** VDDQ_VTT_VCCREF_EN */
	LL_GPIO_SetPinMode(GPIOB, LL_GPIO_PIN_11, LL_GPIO_MODE_OUTPUT);
	LL_GPIO_ResetOutputPin(GPIOB, LL_GPIO_PIN_11);

	/** SPI_BIOS_NSS */
	LL_GPIO_SetPinMode(GPIOB, LL_GPIO_PIN_12, LL_GPIO_MODE_OUTPUT);
	LL_GPIO_SetOutputPin(GPIOB, LL_GPIO_PIN_12);

	/** SPI_BIOS_SCK */
//	LL_GPIO_SetPinMode(GPIOB, LL_GPIO_PIN_13, LL_GPIO_MODE_OUTPUT);

	/** SPI_BIOS_MISO */
//	LL_GPIO_SetPinMode(GPIOB, LL_GPIO_PIN_14, LL_GPIO_MODE_OUTPUT);

	/** SPI_BIOS_MOSI */
//	LL_GPIO_SetPinMode(GPIOB, LL_GPIO_PIN_14, LL_GPIO_MODE_OUTPUT);

	/***************************************************/
	LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOC);

	/** 12V_FALLING ANALOG INPUT */
//	LL_GPIO_SetPinMode(GPIOC, LL_GPIO_PIN_0, LL_GPIO_MODE_OUTPUT);

	/** SOC_COREPWROK*/
	LL_GPIO_SetPinMode(GPIOC, LL_GPIO_PIN_1, LL_GPIO_MODE_OUTPUT);
	LL_GPIO_ResetOutputPin(GPIOC, LL_GPIO_PIN_1);

	/** SLP_S45_N INPUT */
	LL_GPIO_SetPinMode(GPIOC, LL_GPIO_PIN_2, LL_GPIO_MODE_INPUT);

	/** SLP_S3_N INPUT */
	LL_GPIO_SetPinMode(GPIOC, LL_GPIO_PIN_3, LL_GPIO_MODE_INPUT);

	/** CON TO GND */
//	LL_GPIO_SetPinMode(GPIOC, LL_GPIO_PIN_4, LL_GPIO_MODE_OUTPUT);
	/** CON TO GND */
//	LL_GPIO_SetPinMode(GPIOC, LL_GPIO_PIN_5, LL_GPIO_MODE_OUTPUT);

	/** V3P3_PG INPUT */
	LL_GPIO_SetPinMode(GPIOC, LL_GPIO_PIN_6, LL_GPIO_MODE_INPUT);

	/** V1P5_PG INPUT */
	LL_GPIO_SetPinMode(GPIOC, LL_GPIO_PIN_7, LL_GPIO_MODE_INPUT);

	/** VPP_PG INPUT */
	LL_GPIO_SetPinMode(GPIOC, LL_GPIO_PIN_8, LL_GPIO_MODE_INPUT);

	/** VDDQ_VTT_PG INPUT*/
	LL_GPIO_SetPinMode(GPIOC, LL_GPIO_PIN_9, LL_GPIO_MODE_INPUT);

	/** THERMTRIP_SHUTDOWN_N INPUT */
	LL_GPIO_SetPinMode(GPIOC, LL_GPIO_PIN_10, LL_GPIO_MODE_INPUT);

	/** V1P05 */
	LL_GPIO_SetPinMode(GPIOC, LL_GPIO_PIN_11, LL_GPIO_MODE_OUTPUT);
	LL_GPIO_ResetOutputPin(GPIOC, LL_GPIO_PIN_11);

	/** SOC_RTEST_N INPUT*/
	LL_GPIO_SetPinMode(GPIOC, LL_GPIO_PIN_12, LL_GPIO_MODE_INPUT);

	/** FP_RST_BTN_N INPUT*/
	LL_GPIO_SetPinMode(GPIOC, LL_GPIO_PIN_13, LL_GPIO_MODE_INPUT);

	/** FP_PWR_BTN_N INPUT */
	LL_GPIO_SetPinMode(GPIOC, LL_GPIO_PIN_14, LL_GPIO_MODE_INPUT);

	/** VCCSRAM_PG INPUT */
	LL_GPIO_SetPinMode(GPIOC, LL_GPIO_PIN_15, LL_GPIO_MODE_INPUT);

	/***************************************************/
	LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOD);

	/** SPI_BUF_EN_N */
	LL_GPIO_SetPinMode(GPIOD, LL_GPIO_PIN_2, LL_GPIO_MODE_OUTPUT);
	LL_GPIO_SetOutputPin(GPIOD, LL_GPIO_PIN_2);

	/***************************************************/
	LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOF);

	/** VCCP_PG INPUT */
	LL_GPIO_SetPinMode(GPIOF, LL_GPIO_PIN_0, LL_GPIO_MODE_INPUT);

	/** VNN_PG INPUT */
	LL_GPIO_SetPinMode(GPIOF, LL_GPIO_PIN_1, LL_GPIO_MODE_INPUT);

	/** V1P8_PG INPUT*/
	LL_GPIO_SetPinMode(GPIOF, LL_GPIO_PIN_4, LL_GPIO_MODE_INPUT);

	/** NC */
//	LL_GPIO_SetPinMode(GPIOF, LL_GPIO_PIN_5, LL_GPIO_MODE_OUTPUT);

	/** STM32_I2C_CLK SMB_PECI_CLK*/
//	LL_GPIO_SetPinMode(GPIOF, LL_GPIO_PIN_6, LL_GPIO_MODE_OUTPUT);
	/** STM32_I2C_DAT SMB_PECI_DATA*/
//	LL_GPIO_SetPinMode(GPIOF, LL_GPIO_PIN_7, LL_GPIO_MODE_OUTPUT);
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
	/* Настройка вспомогательных параметров. */
	LL_SetSystemCoreClock(HCLK_FREQ);
}

