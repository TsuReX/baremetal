/**
 * @file	src/main.c
 *
 * @brief	Объявление точки вхождения в C-код.
 *
 * @author	Vasily Yurchenko <vasily.v.yurchenko@yandex.ru>
 */

#include "i2c.h"
#include "console.h"
#include "config.h"
#include "scheduler.h"
#include "drivers.h"

void scheduler_process(void)
{
	LL_GPIO_TogglePin(GPIOB, LL_GPIO_PIN_3);
	console_process();
}

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

	scheduler_init();

	uint32_t i = 0;
	while (1) {
		LL_mDelay(DELAY_500_MS * 2);
		print("Main thread iteration %d\r", i++);
	}
}
