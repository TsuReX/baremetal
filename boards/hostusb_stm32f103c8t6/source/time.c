/*
 * utils.c
 *
 *  Created on: Oct 2, 2020
 *      Author: user
 */


#include "time.h"
#include "drivers.h"

extern uint64_t system_ticks;
extern uint32_t system_period;

/**
 * @brief	Выполняет активную задержку
 *
 * @param[in]	useconds	количество микросекунд задержки,
 * 							по факту длительность задержки
 * 							может быть кратна только значению system_period
 */
void udelay(uint32_t useconds)
{
	uint32_t ticks = (useconds + (system_period / 2)) / system_period;
	if (ticks == 0)
		++ticks;
#if 1
	/* Disable interrupt */
	uint64_t next_tick = system_ticks + ticks;
	/* Enable interrupt */
#else
	/* Исходить из соображения, что задержка должна уложится в 32 разряда. */
	uint32_t next_tick = system_ticks + ticks;
#endif
	while (next_tick > system_ticks);
}

void mdelay(uint32_t mseconds)
{
	udelay(mseconds * 1000);
}

void u100delay(uint32_t u100seconds)
{
	udelay(u100seconds * 100);
}
