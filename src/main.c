/**
 * @file	src/main.c
 *
 * @brief	Объявление точки вхождения в C-код.
 *
 * @author	Vasily Yurchenko <vasily.yurchenko@yandex.ru>
 */

#include "main.h"

/** Delay 500 milliseconds. */
#define DELAY_500_MS	500

/**
 * @brief  System Clock Configuration
 *         The system Clock is configured as follow :
 *            System Clock source            = PLL (HSI)
 *            SYSCLK(Hz)                     = 48000000
 *            HCLK(Hz)                       = 48000000
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
	}
}

void board_config(void)
{
	/** Fire LD3 (green) led. */
	LL_GPIO_SetOutputPin(GPIOB, LL_GPIO_PIN_3);
}

void soc_config(void)
{
	LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOB);
	LL_GPIO_SetPinMode(GPIOB, LL_GPIO_PIN_3, LL_GPIO_MODE_OUTPUT);
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

	/* Set systick to 1ms in using frequency set to 48MHz */
	/* This frequency can be calculated through LL RCC macro */
	/* ex: __LL_RCC_CALC_PLLCLK_FREQ (HSI_VALUE / 2, LL_RCC_PLL_MUL_12) */
	LL_Init1msTick(72000000);

	/* Update CMSIS variable (which can be updated also through SystemCoreClockUpdate function) */
	LL_SetSystemCoreClock(72000000);
}
