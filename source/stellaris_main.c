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


int32_t main()
{
	soc_config();

	board_config();

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

