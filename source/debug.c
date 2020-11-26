/*
 * debug.c
 *
 *  Created on: Nov 18, 2020
 *      Author: vasily
 */
#include <stdarg.h>
#include <stdio.h>
#include "debug.h"
#include "console.h"

char str[512];

void printk(uint32_t msg_level, const char *format, ...)
{
	if (msg_level > log_level_get())
		return;

	va_list 	argptr;
	int			sz;

	va_start(argptr, format);
	sz = vsnprintf(str, 512, format, argptr);
	va_end(argptr);

	if (sz <= 0) {
		return;
	}

	console_write((uint8_t *)str, sz, 100000);
}
