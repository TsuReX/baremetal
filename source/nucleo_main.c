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
#include "ds3231m.h"
#include "ina3221.h"
#include "gpio.h"
#include "pwm.h"
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

	i2c_init();

//	gpio_setup_it();

	pwm_init();

	uint32_t duty = 10;
	uint32_t up = 1;
	while (1) {
		LL_mDelay(DELAY_500_MS * 2);
		print("Duty cycle %d\r\n", duty);
//		ina3221_print_voltage_current();
		pwd_duty(duty);

		if (up == 1)
			duty += 10;

		if (up == 0)
			duty -= 10;

		if (duty == 100)
			up = 0;

		if (duty == 10)
			up = 1;
	}
}
