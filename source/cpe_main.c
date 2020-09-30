/**
 * @file	src/psu_main.c
 *
 * @brief	Объявление точки вхождения в C-код.
 *
 * @author	Vasily Yurchenko <vasily.v.yurchenko@yandex.ru>
 */

#include "console.h"
#include "config.h"
#include "drivers.h"
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

	console_init();

//	scheduler_init();

//	gpio_setup_it();

	while (1) {
		d_print("%s()\r\n", __func__);
		LL_mDelay(DELAY_500_MS);
		LL_GPIO_SetOutputPin(GPIOA, LL_GPIO_PIN_5);
		LL_GPIO_ResetOutputPin(GPIOA, LL_GPIO_PIN_4);
		LL_mDelay(DELAY_500_MS);
		LL_GPIO_ResetOutputPin(GPIOA, LL_GPIO_PIN_5);
		LL_GPIO_SetOutputPin(GPIOA, LL_GPIO_PIN_4);
	}


}
