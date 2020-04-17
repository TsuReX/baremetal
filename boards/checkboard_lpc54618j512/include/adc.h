/*
 * adc.h
 *
 *  Created on: Apr 15, 2020
 *      Author: vasily
 */

#ifndef BOARDS_CHECKBOARD_LPC54618J512_INCLUDE_ADC_H_
#define BOARDS_CHECKBOARD_LPC54618J512_INCLUDE_ADC_H_

#include <stddef.h>

#define ADC0_CHANNEL_COUNT	12

void adc0_init(void);

void adc0_convert();

uint32_t adc0_get_value(uint32_t channel);

#endif /* BOARDS_CHECKBOARD_LPC54618J512_INCLUDE_ADC_H_ */
