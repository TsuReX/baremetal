/**
 * @file	src/main.c
 *
 * @brief	Объявление точки вхождения в C-код.
 *
 * @author	Vasily Yurchenko <vasily.v.yurchenko@yandex.ru>
 */

#include "usart1.h"
#include "console.h"
#include "config.h"
#include "main.h"

/**
 * @brief	C-code entry point.
 *
 * @retval	0 in case of success, otherwise error code
 */
int main(void)
{
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
