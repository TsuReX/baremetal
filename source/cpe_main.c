/**
 * @file	src/psu_main.c
 *
 * @brief	Объявление точки вхождения в C-код.
 *
 * @author	Vasily Yurchenko <vasily.v.yurchenko@yandex.ru>
 */

#include "drivers.h"
#include "config.h"
#include "console.h"
#include "scheduler.h"

volatile uint32_t timer_ticks = 0;

void scheduler_process(void)
{
	++timer_ticks;
}

void v3p3_on(uint32_t waiting)
{
	/** V3P3_EN */
	LL_GPIO_SetOutputPin(GPIOB, LL_GPIO_PIN_2);

	/** V3P3_PG */
	if (waiting != 0)
			while (LL_GPIO_IsInputPinSet(GPIOC, LL_GPIO_PIN_6) != 1);
}

void v3p3_off(uint32_t waiting)
{
	/** V3P3_EN */
	LL_GPIO_ResetOutputPin(GPIOB, LL_GPIO_PIN_2);

	/** V3P3_PG */
	if (waiting != 0)
			while (LL_GPIO_IsInputPinSet(GPIOC, LL_GPIO_PIN_6) == 1);
}

void vccsram_on(uint32_t waiting)
{
	/** VCCSRAM_EN */
	LL_GPIO_SetOutputPin(GPIOB, LL_GPIO_PIN_4);

	/** VCCSRAM_PG */
	if (waiting != 0)
		while (LL_GPIO_IsInputPinSet(GPIOC, LL_GPIO_PIN_15) != 1);
}

void vccsram_off(uint32_t waiting)
{
	/** VCCSRAM_EN */
	LL_GPIO_ResetOutputPin(GPIOB, LL_GPIO_PIN_4);

	/** VCCSRAM_PG */
	if (waiting != 0)
		while (LL_GPIO_IsInputPinSet(GPIOC, LL_GPIO_PIN_15) == 1);
}

void vccp_on(uint32_t waiting)
{
	/** VCCP_EN */
	LL_GPIO_SetOutputPin(GPIOB, LL_GPIO_PIN_5);

	/** VCCP_PG INPUT */
	if (waiting != 0)
		while (LL_GPIO_IsInputPinSet(GPIOF, LL_GPIO_PIN_0) != 1);
}

void vccp_off(uint32_t waiting)
{
	/** VCCP_EN */
	LL_GPIO_ResetOutputPin(GPIOB, LL_GPIO_PIN_5);

	/** VCCP_PG INPUT */
	if (waiting != 0)
		while (LL_GPIO_IsInputPinSet(GPIOF, LL_GPIO_PIN_0) == 1);
}

void vddq_vtt_vccref_on(uint32_t waiting)
{
	/** VDDQ_VTT_VCCREF_EN */
	LL_GPIO_SetOutputPin(GPIOB, LL_GPIO_PIN_11);

	/** VDDQ_VTT_PG */
	if (waiting != 0)
		while (LL_GPIO_IsInputPinSet(GPIOC, LL_GPIO_PIN_9) != 1);
}

void vddq_vtt_vccref_off(uint32_t waiting)
{
	/** VDDQ_VTT_VCCREF_EN */
	LL_GPIO_ResetOutputPin(GPIOB, LL_GPIO_PIN_11);

	/** VDDQ_VTT_PG */
	if (waiting != 0)
		while (LL_GPIO_IsInputPinSet(GPIOC, LL_GPIO_PIN_9) == 1);
}

void vpp_on(uint32_t waiting)
{
	/** VPP_EN */
	LL_GPIO_SetOutputPin(GPIOB, LL_GPIO_PIN_10);

	/** VPP_PG */
	if (waiting != 0)
		while (LL_GPIO_IsInputPinSet(GPIOC, LL_GPIO_PIN_8) != 1);
}

void vpp_off(uint32_t waiting)
{
	/** VPP_EN */
	LL_GPIO_ResetOutputPin(GPIOB, LL_GPIO_PIN_10);

	/** VPP_PG */
	if (waiting != 0)
		while (LL_GPIO_IsInputPinSet(GPIOC, LL_GPIO_PIN_8) == 1);
}

void v1p8_v1p05_on(uint32_t waiting)
{
	/** V1P8_V1P05_EN */
	LL_GPIO_SetOutputPin(GPIOB, LL_GPIO_PIN_1);

	/** V1P8_PG */
	if (waiting != 0)
		while (LL_GPIO_IsInputPinSet(GPIOF, LL_GPIO_PIN_4) != 1);
}

void v1p8_v1p05_off(uint32_t waiting)
{
	/** V1P8_V1P05_EN */
	LL_GPIO_ResetOutputPin(GPIOB, LL_GPIO_PIN_1);

	/** V1P8_PG */
	if (waiting != 0)
		while (LL_GPIO_IsInputPinSet(GPIOF, LL_GPIO_PIN_4) == 1);
}

void vnn_on(uint32_t waiting)
{
	/** VNN_EN */
	LL_GPIO_SetOutputPin(GPIOB, LL_GPIO_PIN_6);

	/** VNN_PG */
	if (waiting != 0)
		while (LL_GPIO_IsInputPinSet(GPIOF, LL_GPIO_PIN_1) != 1);
}

void vnn_off(uint32_t waiting)
{
	/** VNN_EN */
	LL_GPIO_ResetOutputPin(GPIOB, LL_GPIO_PIN_6);

	/** VNN_PG */
	if (waiting != 0)
		while (LL_GPIO_IsInputPinSet(GPIOF, LL_GPIO_PIN_1) == 1);
}
/**
 * @brief	C-code entry point.
 *
 * @retval	0 in case of success, otherwise error code
 */
int main(void)
{
	/** Configure internal subsystems of SoC. */
	soc_config();

	/** Configure board's peripherals. */
	board_config();

	console_init();

	scheduler_init();

	while (1) {
		d_print("Elapsed time %ld x 0.1ms\r\n", timer_ticks);
		LL_mDelay(DELAY_500_MS);
		LL_GPIO_SetOutputPin(GPIOA, LL_GPIO_PIN_5);
		LL_GPIO_ResetOutputPin(GPIOA, LL_GPIO_PIN_4);
		LL_mDelay(DELAY_500_MS);
		LL_GPIO_ResetOutputPin(GPIOA, LL_GPIO_PIN_5);
		LL_GPIO_SetOutputPin(GPIOA, LL_GPIO_PIN_4);
	}


}
