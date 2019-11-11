/**
 * @file	src/config.c
 *
 * @brief	Определения функций инициализации SoC и периферии.
 *
 * @author	Vasily Yurchenko <vasily.v.yurchenko@yandex.ru>
 */

#include "main.h"
#include "config.h"

/** Timing register value is computed with the STM32CubeMX Tool */
#define I2C_TIMING	0x2000090E
/** Частота шины HCLK (работы ядра процессора). */
#define HCLK_FREQ	48000000

/**
 * @brief	Настраивает внутреннюю флеш память для корректного взаимодействия с ядром,
 * 			работающем на частоте 48 МГц.
 */
static void flash_config(void)
{
	/* Настройка времени задержки доступа к флешке.
	 * Это необходимо для корректной работы флешки при различных частотах HCLK.
	 */
	LL_FLASH_SetLatency(LL_FLASH_LATENCY_2);
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
static void rcc_config(void)
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
	LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_1);

	/* Настройка делителя для шины APB2 - PCLK2. */
	LL_RCC_SetAPB2Prescaler(LL_RCC_APB2_DIV_1);

}

/**
 * @brief	Наcтраивает системный таймер ядра (SysTick)
 *
 * @param	hclk_freq	частота шины HCLK в герцах
 */
static void systick_config(uint32_t hclk_freq)
{
	/* Производится настройка системного таймера ядра для определения интервала времени равного 1 миллисекунде.  */
	LL_Init1msTick(hclk_freq);
}

/**
 * @brief  I2C1 Configuration
 */
static void i2c1_config(void)
{
	LL_I2C_InitTypeDef I2C_InitStruct;

	LL_GPIO_InitTypeDef GPIO_InitStruct;

	LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOB);
	/* Peripheral clock enable */
	LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_I2C1);

	/** I2C1 GPIO Configuration
	 PB6   ------> I2C1_SCL
	 PB7   ------> I2C1_SDA
	 */
	GPIO_InitStruct.Pin = LL_GPIO_PIN_6;
	GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
	GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
	GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
	GPIO_InitStruct.Pull = LL_GPIO_PULL_UP;
	GPIO_InitStruct.Alternate = LL_GPIO_AF_4;
	LL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = LL_GPIO_PIN_7;
	GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
	GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
	GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
	GPIO_InitStruct.Pull = LL_GPIO_PULL_UP;
	GPIO_InitStruct.Alternate = LL_GPIO_AF_4;
	LL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	/* USART1 interrupt Init */
	NVIC_SetPriority(I2C1_EV_IRQn, 0);
	NVIC_EnableIRQ(I2C1_EV_IRQn);

	I2C_InitStruct.PeripheralMode	= LL_I2C_MODE_SMBUS_HOST;
	I2C_InitStruct.Timing			= I2C_TIMING;
	I2C_InitStruct.AnalogFilter		= LL_I2C_ANALOGFILTER_ENABLE;
	I2C_InitStruct.DigitalFilter	= 0x00;
	I2C_InitStruct.TypeAcknowledge	= LL_I2C_ACK;
	I2C_InitStruct.OwnAddress1		= 0x00;
	LL_I2C_Init(I2C1, &I2C_InitStruct);

	LL_I2C_Enable(I2C1);
}

/**
 * @brief	Настройка устройств платформы(платы)
 */
void board_config(void)
{
	/** Fire LD3 (green) led. */
	LL_GPIO_SetOutputPin(GPIOB, LL_GPIO_PIN_3);
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
	systick_config(HCLK_FREQ);
	/* Настройка вспомогательных параметров. */
	LL_SetSystemCoreClock(HCLK_FREQ);

	/** Configuring GPIO. */
	LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOB);
	LL_GPIO_SetPinMode(GPIOB, LL_GPIO_PIN_3, LL_GPIO_MODE_OUTPUT);

	i2c1_config();

}

