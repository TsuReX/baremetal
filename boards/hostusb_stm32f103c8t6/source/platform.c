/*
 * platform.c
 *
 *  Created on: Nov 15, 2020
 *      Author: vasily
 */

#include "drivers.h"
#include "kbmsusb.h"
#include "config.h"

void max3421e_chip_activate(uint32_t chip_num)
{
	switch(chip_num) {
		case KEYBOARD_CHANNEL:
			LL_GPIO_ResetOutputPin(GPIOB, LL_GPIO_PIN_0);
			break;

		case MOUSE_CHANNEL:
			LL_GPIO_ResetOutputPin(GPIOB, LL_GPIO_PIN_1);
			break;
	};
}

void max3421e_chip_deactivate(uint32_t chip_num)
{
	switch(chip_num) {
		case KEYBOARD_CHANNEL:
			LL_GPIO_SetOutputPin(GPIOB, LL_GPIO_PIN_0);
			break;

		case MOUSE_CHANNEL:
			LL_GPIO_SetOutputPin(GPIOB, LL_GPIO_PIN_1);
			break;
	};
}

void spi_chip_activate(uint32_t chip_num)
{
	switch(chip_num) {
		case FLASH_CHANNEL:
			LL_GPIO_ResetOutputPin(GPIOB, LL_GPIO_PIN_2);
			break;

	};
}

void spi_chip_deactivate(uint32_t chip_num)
{
	switch(chip_num) {
		case FLASH_CHANNEL:
			LL_GPIO_SetOutputPin(GPIOB, LL_GPIO_PIN_2);
			break;
	};
}

