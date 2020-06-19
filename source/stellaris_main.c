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

extern	uint32_t __stack_end__;

int32_t main()
{
	uint32_t	init_process_address = (uint32_t)&__stack_end__;
	size_t		init_process_address_size = 0x1000;

	init_process_create(init_process_address, init_process_address_size);

	init_process_start();

	return 0;
}

