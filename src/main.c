/**
 * @file	src/main.c
 *
 * @brief	Объявление точки вхождения в C-код.
 *
 * @author	Vasily Yurchenko <vasily.yurchenko@yandex.ru>
 */

#include <stdarg.h>
#include <stdio.h>

#include "main.h"

/** Delay 500 milliseconds. */
#define DELAY_500_MS	500

/**
 * @brief	Выводит в текстовую консоль данные на основе строки формата.
 *
 * @param[in]	format	строка формата выводимых данных
 *
 * @param[in]	...	список аргументов, значения которых необходимо вывести в текстовую консоль
 *
 * @retval	void
 */
static void print(const char *format, ...) __attribute__((format(printf, 1, 2)));


/**
 * @brief  System Clock Configuration
 *         The system Clock is configured as follow :
 *            System Clock source            = PLL (HSI)
 *            SYSCLK(Hz)                     = 72000000
 *            HCLK(Hz)                       = 72000000
 *            AHB Prescaler                  = 1
 *            APB1 Prescaler                 = 1
 *            APB2 Prescaler                 = 1
 *            PLLMUL                         = RCC_PLL_MUL12 (12)
 *            Flash Latency(WS)              = 2
 * @param  None
 * @retval None
 */
static void sysclk_config(void);

/**
 * @brief
 */
static void soc_config(void);

/**
 * @brief
 */
static void board_config(void);

/**
 * @brief	C-code entry point.
 *
 * @retval	0 in case of success, otherwise error code
 */
int main(void)
{
	/** Configure the system clock to 48 MHz */
	sysclk_config();

	/** Configure internal subsystems of SoC. */
	soc_config();

	/** Configure board's peripherals. */
	board_config();

	/** Add your application code here */

	/** Infinite loop */
	while (1) {
		LL_mDelay(DELAY_500_MS);
		LL_GPIO_TogglePin(GPIOB, LL_GPIO_PIN_3);
		print("Test output\r\n");
	}
}

static void usart1_init(void)
{

	LL_USART_InitTypeDef USART_InitStruct;

	LL_GPIO_InitTypeDef GPIO_InitStruct;

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
	GPIO_InitStruct.Alternate = LL_GPIO_AF_1;
	LL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = LL_GPIO_PIN_10;
	GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
	GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
	GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
	GPIO_InitStruct.Pull = LL_GPIO_PULL_UP;
	GPIO_InitStruct.Alternate = LL_GPIO_AF_1;
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

void board_config(void)
{
	/** Fire LD3 (green) led. */
	LL_GPIO_SetOutputPin(GPIOB, LL_GPIO_PIN_3);
}

void soc_config(void)
{
	/** Configuring GPIO. */
	LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOB);
	LL_GPIO_SetPinMode(GPIOB, LL_GPIO_PIN_3, LL_GPIO_MODE_OUTPUT);

	/** Configuring USART1.*/
	usart1_init();

}

void sysclk_config(void)
{
	/* Set FLASH latency */
	LL_FLASH_SetLatency(LL_FLASH_LATENCY_2);

	/* Enable HSI and wait for activation*/
	LL_RCC_HSI_Enable();
	while (LL_RCC_HSI_IsReady() != 1) {
	}
	;

	/* Main PLL configuration and activation */
	LL_RCC_PLL_ConfigDomain_SYS(LL_RCC_PLLSOURCE_HSI_DIV_2, LL_RCC_PLL_MUL_12);

	LL_RCC_PLL_Enable();
	while (LL_RCC_PLL_IsReady() != 1) {
	}
	;

	/* Sysclk activation on the main PLL */
	LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
	LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL);
	while (LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_PLL) {
	}
	;

	/* Set APB1 & APB2 prescaler*/
	LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_1);
	LL_RCC_SetAPB2Prescaler(LL_RCC_APB2_DIV_1);

	/* Set systick to 1ms in using frequency set to 72MHz */
	/* This frequency can be calculated through LL RCC macro */
	/* ex: __LL_RCC_CALC_PLLCLK_FREQ (HSI_VALUE / 2, LL_RCC_PLL_MUL_12) */
	LL_Init1msTick(72000000);

	/* Update CMSIS variable (which can be updated also through SystemCoreClockUpdate function) */
	LL_SetSystemCoreClock(72000000);
}

void print(const char *format, ...)
{
	va_list argptr;
	char	str[512];
	int		sz;
	size_t	i;

	va_start(argptr, format);
	sz = vsnprintf(str, 512, format, argptr);
	if (sz > 0)
		for (i = 0; i < sz; ++i)
			LL_USART_TransmitData8(USART1, str[i]);
	va_end(argptr);
}
