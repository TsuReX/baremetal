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

void timer0a_handler()
{
	GPIOF->DATA = (~(GPIOF->DATA & 0x1)) & 0x1;
	TIMER0->ICR = 0xFFFFFFFF;
}

void timer0a_init()
{

	NVIC_EnableIRQ(INT_TIMER0A_TM4C123);

	/* Disable timer */
	TIMER0->CTL &= ~TIMER_CTL_TAEN;

	TIMER0->CFG = 0x0;

	TIMER0->TAMR = TIMER_TAMR_TAMR_PERIOD;

	TIMER0->TAILR = 40;

	TIMER0->IMR = TIMER_IMR_TATOIM | TIMER_IMR_TAMIM;

	/* Enable timer */
	TIMER0->CTL |= TIMER_CTL_TAEN;

}

int32_t main()
{
	soc_init();

	board_init();

	console_init();

	log_level_set(DEBUG);

	timer0a_init();

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

