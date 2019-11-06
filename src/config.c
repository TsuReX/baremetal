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

/**
 * @brief	Производит настройку источников тактирования и системного таймера:
 * 			SYSCLK, PLLCLK, HCLK, PCLK1, PCLK2, FLITFCLK, RTCCLK, IWDGCLK, MCO, SystemTimer(SysTick)
 *			The system Clock is configured as follow :
 *				System Clock source            = PLL (HSI)
 *				SYSCLK(Hz)                     = 48000000
 *				HCLK(Hz)                       = 48000000
 *				AHB Prescaler                  = 1
 *				APB1 Prescaler                 = 1
 *				APB2 Prescaler                 = 1
 *				PLLMUL                         = RCC_PLL_MUL12 (12)
 *				Flash Latency(WS)              = 2
 * @param  None
 * @retval None
 */
static void sysclk_config(void)
{
	/* Настройка частоты FLITFCLK для интерфейса программирования флешки. */
	LL_FLASH_SetLatency(LL_FLASH_LATENCY_2);

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

	/* Системный источник тактирования SYSCLK настроен. */

	/** Sysclk activation on the main PLL */
	LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
	LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL);
	while (LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_PLL);

	/** Set APB1 prescaler - PCLK1*/
	LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_1);
	/** Set APB2 prescaler - PCLK2*/
	LL_RCC_SetAPB2Prescaler(LL_RCC_APB2_DIV_1);

	/** Set systick to 1ms in using frequency set to 72MHz */
	/* This frequency can be calculated through LL RCC macro */
	/* ex: __LL_RCC_CALC_PLLCLK_FREQ (HSI_VALUE / 2, LL_RCC_PLL_MUL_12) */
	LL_Init1msTick(72000000);

	/** Update CMSIS variable (which can be updated also through SystemCoreClockUpdate function) */
	LL_SetSystemCoreClock(72000000);
}

/**
 * @brief  USART1 Configuration
 */
static void usart1_config(void)
{

	LL_USART_InitTypeDef USART_InitStruct;

	LL_GPIO_InitTypeDef GPIO_InitStruct;

	LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOA);
	/* Peripheral clock enable */
	LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_USART1);

	/**USART1 GPIO Configuration
	 PA9   ------> USART1_TX
	 PA10   ------> USART1_RX
	 */
	GPIO_InitStruct.Pin = LL_GPIO_PIN_9;
	GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
	GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
	GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
	GPIO_InitStruct.Pull = LL_GPIO_PULL_UP;
	GPIO_InitStruct.Alternate = LL_GPIO_AF_7;
	LL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = LL_GPIO_PIN_10;
	GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
	GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
	GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
	GPIO_InitStruct.Pull = LL_GPIO_PULL_UP;
	GPIO_InitStruct.Alternate = LL_GPIO_AF_7;
	LL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	/* USART1 interrupt Init */
/*	NVIC_SetPriority(USART1_IRQn, 0);
	NVIC_EnableIRQ(USART1_IRQn);
*/
	USART_InitStruct.BaudRate = 115200;
	USART_InitStruct.DataWidth = LL_USART_DATAWIDTH_8B;
	USART_InitStruct.StopBits = LL_USART_STOPBITS_1;
	USART_InitStruct.Parity = LL_USART_PARITY_NONE;
	USART_InitStruct.TransferDirection = LL_USART_DIRECTION_TX_RX;
	USART_InitStruct.HardwareFlowControl = LL_USART_HWCONTROL_NONE;
	USART_InitStruct.OverSampling = LL_USART_OVERSAMPLING_16;
	LL_USART_Init(USART1, &USART_InitStruct);

	LL_USART_DisableIT_CTS(USART1);

/*	LL_USART_EnableOverrunDetect(USART1);*/

/*	LL_USART_EnableDMADeactOnRxErr(USART1);*/

	LL_USART_ConfigAsyncMode(USART1);

	LL_USART_Enable(USART1);

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

void board_config(void)
{
	/** Fire LD3 (green) led. */
	LL_GPIO_SetOutputPin(GPIOB, LL_GPIO_PIN_3);
}

void soc_config(void)
{
	/** Configure the system clock to 48 MHz */
	sysclk_config();

	/** Configuring GPIO. */
	LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOB);
	LL_GPIO_SetPinMode(GPIOB, LL_GPIO_PIN_3, LL_GPIO_MODE_OUTPUT);

	usart1_config();
	i2c1_config();

}

