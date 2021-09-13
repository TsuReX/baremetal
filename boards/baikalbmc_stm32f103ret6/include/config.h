/*
 * config.h
 *
 *  Created on: Nov 13, 2020
 *      Author: vasily
 */

#ifndef _CONFIG_H_
#define _CONFIG_H_

/** Частота шины HCLK (работы ядра процессора). */
#define HCLK_FREQ	48000000

#define CURRENT_MESSAGE_LEVEL 2

#define FLASH_CHANNEL 0x0

#define CORE_CLOCK_PIN		LL_GPIO_PIN_1
#define CORE_CLOCK_PORT		GPIOB

#define JTAG_RST_PIN		LL_GPIO_PIN_8
#define JTAG_RST_PORT		GPIOA

#define DRVRST_PIN		LL_GPIO_PIN_4
#define DRVRST_PORT		GPIOA

#define LED_PWR_PIN		LL_GPIO_PIN_9
#define LED_PWR_PORT	GPIOA

#define LED_STAT_PIN	LL_GPIO_PIN_10
#define LED_STAT_PORT	GPIOA

#define LED_AUX_PIN		LL_GPIO_PIN_0
#define LED_AUX_PORT	GPIOB

#define EN_CLKDIST_PIN		LL_GPIO_PIN_6
#define EN_CLKDIST_PORT		GPIOA

#define EN_VDDPLL_PIN		LL_GPIO_PIN_7
#define EN_VDDPLL_PORT		GPIOA

#define EN_VDD_PIN		LL_GPIO_PIN_11
#define EN_VDD_PORT		GPIOA

#define PG_VDD_PIN		LL_GPIO_PIN_12
#define PG_VDD_PORT		GPIOA

#define EN_VL_PIN		LL_GPIO_PIN_4
#define EN_VL_PORT		GPIOC

#define PG_VL_PIN		LL_GPIO_PIN_5
#define PG_VL_PORT		GPIOC

#define EN_VCORE_PIN	LL_GPIO_PIN_0
#define EN_VCORE_PORT	GPIOC

#define PG_VCORE_PIN	LL_GPIO_PIN_1
#define PG_VCORE_PORT	GPIOC

#define EN_VLL_PIN		LL_GPIO_PIN_4
#define EN_VLL_PORT		GPIOB

#define PG_VLL_PIN		LL_GPIO_PIN_5
#define PG_VLL_PORT		GPIOB

#define EN_VDRAM_PIN	LL_GPIO_PIN_2
#define EN_VDRAM_PORT	GPIOC

#define PG_VDRAM_PIN	LL_GPIO_PIN_3
#define PG_VDRAM_PORT	GPIOC

#define EN_AUX_PIN		LL_GPIO_PIN_2
#define EN_AUX_PORT		GPIOD

#define PG_AUX_PIN		LL_GPIO_PIN_5
#define PG_AUX_PORT		GPIOA

#define EN_VCC_PIN		LL_GPIO_PIN_9
#define EN_VCC_PORT		GPIOC

#define PG_VCC_PIN		LL_GPIO_PIN_12
#define PG_VCC_PORT		GPIOC

#define PWR12_SENSE_PIN		LL_GPIO_PIN_1
#define PWR12_SENSE_PORT	GPIOA

#define PWRINT_SENSE_PIN	LL_GPIO_PIN_0
#define PWRINT_SENSE_PORT	GPIOA

#endif /* _CONFIG_H_ */
