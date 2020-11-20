/*
 * utils.c
 *
 *  Created on: Oct 2, 2020
 *      Author: user
 */


#include "time.h"
#include "drivers.h"



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
