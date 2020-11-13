/**
 * @file	src/main.c
 *
 * @brief	Объявление точки вхождения в C-код.
 *
 * @author	Vasily Yurchenko <vasily.v.yurchenko@yandex.ru>
 */


#include <stdint.h>
#include <stdbool.h>
#include <context.h>
#include <config.h>
#include <drivers.h>
#include <time.h>

static uint8_t	led_value = 1;

void usb0_irq_handler(void) {
	GPIOF->DATA = led_value++;
	led_value = led_value & 0x7;


}

int32_t main()
{
	soc_init();

	board_init();

	while(1) {

		GPIOF->DATA = 4;
		mdelay(500);
		GPIOF->DATA = 2;
		mdelay(500);
		GPIOF->DATA = 8;
		mdelay(500);

	}

	return 0;
}

