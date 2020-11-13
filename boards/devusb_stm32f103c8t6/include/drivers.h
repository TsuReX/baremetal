#ifndef __DRIVERS_H
#define __DRIVERS_H

/* LL drivers common to all LL examples */
#include "stm32f1xx_ll_bus.h"
#include "stm32f1xx_ll_rcc.h"
#include "stm32f1xx_ll_system.h"
#include "stm32f1xx_ll_utils.h"
#include "stm32f1xx_ll_pwr.h"
#include "stm32f1xx_ll_exti.h"
#include "stm32f1xx_ll_gpio.h"
/* LL drivers specific to LL examples IPs */
#include "stm32f1xx_ll_adc.h"
/*#include "stm32f1xx_ll_comp.h"*/
#include "stm32f1xx_ll_cortex.h"
#include "stm32f1xx_ll_crc.h"
#include "stm32f1xx_ll_dac.h"
#include "stm32f1xx_ll_dma.h"
/*#include "stm32f1xx_ll_hrtim.h"*/
#include "stm32f1xx_ll_i2c.h"
#include "stm32f1xx_ll_iwdg.h"
/*#include "stm32f1xx_ll_opamp.h"*/
#include "stm32f1xx_ll_rtc.h"
#include "stm32f1xx_ll_spi.h"
#include "stm32f1xx_ll_tim.h"
#include "stm32f1xx_ll_usart.h"
#include "stm32f1xx_ll_wwdg.h"

#if defined(USE_FULL_ASSERT)
#include "stm32_assert.h"
#endif /* USE_FULL_ASSERT */

/** Delay 500 milliseconds. */
#define DELAY_500_MS	500
/** Delay 50 milliseconds. */
#define DELAY_50_MS	50

#define KEYBOARD

static inline uint16_t swap_bytes_in_word(uint16_t word)
{
	uint8_t tmp = ((uint8_t *)&word)[0];
	((uint8_t *)&word)[0] = ((uint8_t *)&word)[1];
	((uint8_t *)&word)[1] = tmp;
	return word;
}

#endif /* __DRIVERS_H */

