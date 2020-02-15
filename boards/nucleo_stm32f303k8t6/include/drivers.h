/**
  ******************************************************************************
  * @file    Templates_LL/Inc/main.h 
  * @author  MCD Application Team
  * @brief   Header for main.c module
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2016 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */

#ifndef __MAIN_H
#define __MAIN_H

/* LL drivers common to all LL examples */
#include "stm32f3xx_ll_bus.h"
#include "stm32f3xx_ll_rcc.h"
#include "stm32f3xx_ll_system.h"
#include "stm32f3xx_ll_utils.h"
#include "stm32f3xx_ll_pwr.h"
#include "stm32f3xx_ll_exti.h"
#include "stm32f3xx_ll_gpio.h"
/* LL drivers specific to LL examples IPs */
#include "stm32f3xx_ll_adc.h"
#include "stm32f3xx_ll_comp.h"
#include "stm32f3xx_ll_cortex.h"
#include "stm32f3xx_ll_crc.h"
#include "stm32f3xx_ll_dac.h"
#include "stm32f3xx_ll_dma.h"
#include "stm32f3xx_ll_hrtim.h"
#include "stm32f3xx_ll_i2c.h"
#include "stm32f3xx_ll_iwdg.h"
#include "stm32f3xx_ll_opamp.h"
#include "stm32f3xx_ll_rtc.h"
#include "stm32f3xx_ll_spi.h"
#include "stm32f3xx_ll_tim.h"
#include "stm32f3xx_ll_usart.h"
#include "stm32f3xx_ll_wwdg.h"

#if defined(USE_FULL_ASSERT)
#include "stm32_assert.h"
#endif /* USE_FULL_ASSERT */

/** Delay 500 milliseconds. */
#define DELAY_500_MS	500
/** Delay 50 milliseconds. */
#define DELAY_50_MS	50

static inline uint16_t swap_bytes_in_word(uint16_t word)
{
	uint8_t tmp = ((uint8_t *)&word)[0];
	((uint8_t *)&word)[0] = ((uint8_t *)&word)[1];
	((uint8_t *)&word)[1] = tmp;
	return word;
}

#endif /* __MAIN_H */

