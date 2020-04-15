/*
 * adc.h
 *
 *  Created on: Apr 15, 2020
 *      Author: vasily
 */

#ifndef BOARDS_CHECKBOARD_LPC54618J512_INCLUDE_ADC_H_
#define BOARDS_CHECKBOARD_LPC54618J512_INCLUDE_ADC_H_

#include <stddef.h>

void adc0_init(void);

uint32_t adc0_3_getval(void);
uint32_t adc0_4_getval(void);
uint32_t adc0_5_getval(void);

#endif /* BOARDS_CHECKBOARD_LPC54618J512_INCLUDE_ADC_H_ */
