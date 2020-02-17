/*
 * gpio.c
 *
 *  Created on: Feb 15, 2020
 *      Author: vasily
 */

#include "stddef.h"
#include "drivers.h"
#include "gpio.h"
#include "console.h"

void gpio_setup_it(void)
{
	/* D10, D11, D12, D13 - PA11, PB5, PB4, PB3 - EXTI11, EXTI5, EXTI4, EXTI3*/

	LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOA);
	LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_11, LL_GPIO_MODE_INPUT);
	LL_GPIO_SetPinPull(GPIOA, LL_GPIO_PIN_11, LL_GPIO_PULL_UP);

	LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOB);
/*	LL_GPIO_SetPinMode(GPIOB, LL_GPIO_PIN_3, LL_GPIO_MODE_OUTPUT);
	LL_GPIO_SetPinPull(GPIOA, LL_GPIO_PIN_3, LL_GPIO_PULL_UP);
*/
	LL_GPIO_SetPinMode(GPIOB, LL_GPIO_PIN_4, LL_GPIO_MODE_INPUT);
	LL_GPIO_SetPinPull(GPIOB, LL_GPIO_PIN_4, LL_GPIO_PULL_UP);

	LL_GPIO_SetPinMode(GPIOB, LL_GPIO_PIN_5, LL_GPIO_MODE_INPUT);
	LL_GPIO_SetPinPull(GPIOB, LL_GPIO_PIN_5, LL_GPIO_PULL_UP);

	LL_EXTI_EnableRisingTrig_0_31(LL_EXTI_LINE_4);
	LL_EXTI_EnableFallingTrig_0_31(LL_EXTI_LINE_4);
	LL_EXTI_EnableIT_0_31(LL_EXTI_LINE_4);

	LL_EXTI_EnableRisingTrig_0_31(LL_EXTI_LINE_5);
	LL_EXTI_EnableFallingTrig_0_31(LL_EXTI_LINE_5);
	LL_EXTI_EnableIT_0_31(LL_EXTI_LINE_5);

	LL_EXTI_EnableRisingTrig_0_31(LL_EXTI_LINE_11);
	LL_EXTI_EnableFallingTrig_0_31(LL_EXTI_LINE_11);
	LL_EXTI_EnableIT_0_31(LL_EXTI_LINE_11);

/*	NVIC_SetPriority(I2C1_EV_IRQn, 0);
	NVIC_EnableIRQ(I2C1_EV_IRQn);
*/
	NVIC_SetPriority(EXTI4_IRQn, 0);
	NVIC_EnableIRQ(EXTI4_IRQn);

/*	NVIC_SetPriority(EXTI9_5_IRQn, 0);
	NVIC_EnableIRQ(EXTI9_5_IRQn);
*/
	NVIC_SetPriority(EXTI15_10_IRQn, 0);
	NVIC_EnableIRQ(EXTI15_10_IRQn);

}

/*void EXTI3_IRQHandler(void)
{
	print("%s()\r\n");
}
*/
void EXTI4_IRQHandler(void)
{
	print("%s()\r\n", __func__);
	LL_EXTI_ClearFlag_0_31(LL_EXTI_LINE_4);
}

void EXTI9_5_IRQHandler(void)
{
	print("%s()\r\n", __func__);
	LL_EXTI_ClearFlag_0_31(LL_EXTI_LINE_5 | LL_EXTI_LINE_6 | LL_EXTI_LINE_7 | LL_EXTI_LINE_8 | LL_EXTI_LINE_9);
}

void EXTI15_10_IRQHandler(void)
{
	print("%s()\r\n", __func__);
	LL_EXTI_ClearFlag_0_31(LL_EXTI_LINE_11);
}
