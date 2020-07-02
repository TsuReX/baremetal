/**
 * @file	src/main.c
 *
 * @brief	Объявление точки вхождения в C-код.
 *
 * @author	Vasily Yurchenko <vasily.v.yurchenko@yandex.ru>
 */

#include "drivers.h"
#include "config.h"
#include "console.h"
#include "scheduler.h"
/*#include "i2c.h"
#include "ds3231m.h"
#include "ina3221.h"
#include "gpio.h"
*/

/**
 * @brief	C-code entry point.
 *
 * @retval	0 in case of success, otherwise error code
 */
int main(void)
{
	/** Configure internal subsystems of SoC. */
	soc_init();

	/** Configure board's peripherals. */
	board_init();

	console_init();

	scheduler_init();

	/*i2c_init();*/

	/*gpio_setup_it();*/

	uint32_t i = 0;
	while (1) {
		LL_mDelay(DELAY_500_MS * 2);
		print("Main thread iteration %ld\r\n", i++);
		/*ina3221_print_voltage_current();*/
		LL_GPIO_TogglePin(GPIOA, LL_GPIO_PIN_0);
		LL_GPIO_TogglePin(GPIOC, LL_GPIO_PIN_13);
		LL_GPIO_TogglePin(GPIOC, LL_GPIO_PIN_14);
		LL_GPIO_TogglePin(GPIOC, LL_GPIO_PIN_15);
	}
}
