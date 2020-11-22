/*
 * utils.c
 *
 *  Created on: Oct 2, 2020
 *      Author: user
 */


#include "time.h"
#include "drivers.h"

extern uint64_t system_ticks;

void _mdelay(uint32_t ticks)
{
#if 1
	/* Disable interrupt */
	uint64_t next_tick = system_ticks + ticks;
	/* Enable interrupt */
#else
	/* Исходить из соображения, что задержка должна уложится в 32 разряда. */
	uint32_t next_tick = (uint32_t)system_ticks + ticks;
#endif
	while (next_tick < system_ticks)
		__DSB();
}

void mdelay(uint32_t mseconds)
{
	mseconds *= 10;
  __IO uint32_t  tmp = SysTick->CTRL;  /* Clear the COUNTFLAG first */
  /* Add this code to indicate that local variable is not used */
  ((void)tmp);

//  /* Add a period to guaranty minimum wait */
//  if (delay < LL_MAX_DELAY) {
//    delay++;
//  }

  while (mseconds) {
    if ((SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk) != 0U) {
      mseconds--;
    }
  }
}

void u100delay(uint32_t u100seconds)
{
  __IO uint32_t  tmp = SysTick->CTRL;  /* Clear the COUNTFLAG first */
  /* Add this code to indicate that local variable is not used */
  ((void)tmp);

//  /* Add a period to guaranty minimum wait */
//  if (delay < LL_MAX_DELAY) {
//    delay++;
//  }

  while (u100seconds) {
    if ((SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk) != 0U) {
      u100seconds--;
    }
  }
}
