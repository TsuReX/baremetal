/*
 * adc.h
 *
 *  Created on: Apr 15, 2020
 *      Author: vasily
 */

#ifndef BOARDS_CHECKBOARD_LPC54618J512_INCLUDE_ADC_H_
#define BOARDS_CHECKBOARD_LPC54618J512_INCLUDE_ADC_H_

#include <stddef.h>

enum adc_channel_t {
	CHAN3_3V3_BP_ADC 			= 0x3,
	CHAN4_5V_BP_ADC				= 0x4,
	CHAN5_12V_BP_ADC			= 0x5,
	CHAN6_P3V3_SYS_PL_OHM_ADC	= 0x6,
	CHAN7_MUX4_OHM_ADC			= 0x7,
	CHAN8_MUX4_VOLT_ADC			= 0x8,
	CHAN9_MUX5_OHM_ADC			= 0x9,
	CHAN10_MUX6_OHM_ADC			= 0xA,

};

#define ADC0_CHANNEL_COUNT	12

void adc0_init(void);

void adc0_convert();

uint32_t adc0_get_value(uint32_t channel);

#endif /* BOARDS_CHECKBOARD_LPC54618J512_INCLUDE_ADC_H_ */
