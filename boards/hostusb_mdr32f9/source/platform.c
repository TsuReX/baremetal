/*
 * platform.c
 *
 *  Created on: Nov 15, 2020
 *      Author: vasily
 */

#include "drivers.h"
#include "kbmsusb.h"

void max3421e_chip_activate(uint32_t chip_num)
{
	switch(chip_num) {
		case KEYBOARD_CHANNEL:
			PORT_ResetBits(MDR_PORTE, PORT_Pin_0);
			break;

		case MOUSE_CHANNEL:
			PORT_ResetBits(MDR_PORTB, PORT_Pin_8);
			break;
	};
}

void max3421e_chip_deactivate(uint32_t chip_num)
{
	switch(chip_num) {
		case KEYBOARD_CHANNEL:
			PORT_SetBits(MDR_PORTE, PORT_Pin_0);
			break;

		case MOUSE_CHANNEL:
			PORT_SetBits(MDR_PORTB, PORT_Pin_8);
			break;
	};
}

//void max3421e_chip_activate(uint32_t chip_num)
//{
//	switch(chip_num) {
//		case KEYBOARD_CHANNEL:
//			LL_GPIO_ResetOutputPin(GPIOA, LL_GPIO_PIN_4);
//			break;
//
//		case MOUSE_CHANNEL:
//			LL_GPIO_ResetOutputPin(GPIOA, LL_GPIO_PIN_3);
//			break;
//	};
//}
//
//void max3421e_chip_deactivate(uint32_t chip_num)
//{
//	switch(chip_num) {
//		case KEYBOARD_CHANNEL:
//			LL_GPIO_SetOutputPin(GPIOA, LL_GPIO_PIN_4);
//			break;
//
//		case MOUSE_CHANNEL:
//			LL_GPIO_SetOutputPin(GPIOA, LL_GPIO_PIN_3);
//			break;
//	};
//}
