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
#include <TIVA.h>

int32_t main()
{
//	soc_config();

	board_config();

	GPIOF->DATA = (0x1 << 3) | (0x1 << 2) | (0x1 << 1);;

	while(1);

	return 0;
}

