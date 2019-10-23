/**
 * @file	src/console.c
 *
 * @brief	Определены функции вывода данных в текстовую консоль.
 *
 * @author	Vasily Yurchenko <vasily.yurchenko@yandex.ru>
 */

#include <stdarg.h>
#include <stdio.h>

#include "usart1.h"
#include "console.h"
#include "main.h"

void print(const char *format, ...)
{
	va_list argptr;
	char	str[512];
	int		sz;
	size_t	i;

	va_start(argptr, format);
	sz = vsnprintf(str, 512, format, argptr);
	if (sz > 0) {
		uart1_transmit(str, sz, DELAY_500_MS);
	}
	va_end(argptr);
}
