/**
 * @file	src/main.c
 *
 * @brief	Объявление точки вхождения в C-код.
 *
 * @author	Vasily Yurchenko <vasily.v.yurchenko@yandex.ru>
 */


#include <stdint.h>
#include <stdbool.h>

#include "context.h"
#include "config.h"
#include "init.h"
#include "drivers.h"
#include "time.h"
#include "console.h"
#include "debug.h"

int32_t main()
{
	soc_init();

	board_init();

	console_init();

	log_level_set(DEBUG);

	printk(DEBUG, "Console initialized\r\n");

	while(1) {

		GPIOF->DATA = 2;
		mdelay(150);
		GPIOF->DATA = 4;
		mdelay(150);
		GPIOF->DATA = 8;
		mdelay(150);
		printk(DEBUG, "Cycle\r\n");
	}

	return 0;
}

