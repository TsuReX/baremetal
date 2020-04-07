/*
 * time.c
 *
 *  Created on: Apr 7, 2020
 *      Author: vasily
 */
#include <drivers.h>
#include <time.h>

/* Max delay can be used in LL_mDelay */
#define LL_MAX_DELAY                  0xFFFFFFFFU

void mdelay(uint32_t Delay)
{
  __IO uint32_t  tmp = SysTick->CTRL;  /* Clear the COUNTFLAG first */
  /* Add this code to indicate that local variable is not used */
  ((void)tmp);

  /* Add a period to guaranty minimum wait */
  if(Delay < LL_MAX_DELAY)
  {
    Delay++;
  }

  while (Delay)
  {
    if((SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk) != 0U)
    {
      Delay--;
    }
  }
}
