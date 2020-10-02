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

void exti_0_1_irq_handler(void)
{
	d_print("%s()\r\n", __func__);
	LL_EXTI_ClearFlag_0_31(LL_EXTI_LINE_1 | LL_EXTI_LINE_0);
}

void exti_2_3_irq_handler(void)
{
	d_print("%s()\r\n", __func__);
	LL_EXTI_ClearFlag_0_31(LL_EXTI_LINE_3 | LL_EXTI_LINE_2);
}

void exti_4_15_irq_handler(void)
{
	d_print("%s()\r\n", __func__);
	LL_EXTI_ClearFlag_0_31(LL_EXTI_LINE_ALL_0_31 & ~(LL_EXTI_LINE_3 | LL_EXTI_LINE_2 | LL_EXTI_LINE_1 | LL_EXTI_LINE_0));
}

void gpio_irq_config(void)
{
	LL_APB1_GRP2_EnableClock(LL_APB1_GRP2_PERIPH_SYSCFG);

	/** FP_RST_BTN_N */
	LL_SYSCFG_SetEXTISource(LL_SYSCFG_EXTI_PORTC, LL_SYSCFG_EXTI_LINE13);
	LL_GPIO_SetPinPull(GPIOC, LL_GPIO_PIN_13, LL_GPIO_PULL_UP);
	LL_EXTI_EnableIT_0_31(LL_EXTI_LINE_13);
	LL_EXTI_EnableRisingTrig_0_31(LL_EXTI_LINE_13);

	/** FP_PWR_BTN_N */
	LL_SYSCFG_SetEXTISource(LL_SYSCFG_EXTI_PORTC, LL_SYSCFG_EXTI_LINE14);
	LL_GPIO_SetPinPull(GPIOC, LL_GPIO_PIN_14, LL_GPIO_PULL_UP);
	LL_EXTI_EnableIT_0_31(LL_EXTI_LINE_14);
	LL_EXTI_EnableRisingTrig_0_31(LL_EXTI_LINE_14);

	NVIC_EnableIRQ(EXTI0_1_IRQn);
	NVIC_SetPriority(EXTI0_1_IRQn, 0);

	NVIC_EnableIRQ(EXTI2_3_IRQn);
	NVIC_SetPriority(EXTI2_3_IRQn, 0);

	NVIC_EnableIRQ(EXTI4_15_IRQn);
	NVIC_SetPriority(EXTI4_15_IRQn, 0);
}

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

	gpio_irq_config();

	console_init();

	scheduler_init();

	v3p3_on(1);
	d_print("PSU V3P3 is ON\r\n");

	LL_mDelay(10);

	vnn_on(1);
	d_print("PSU VNN is ON\r\n");

	LL_mDelay(10);

	v1p8_v1p05_on(1);
	d_print("PSU V1P8 and V1P05 are ON\r\n");

	LL_mDelay(10);

//	LL_GPIO_ResetOutputPin(GPIOA, LL_GPIO_PIN_8);
//	d_print("SOC_SRTCRST_N is DEASSERTED\r\n");
//
//	LL_mDelay(10);

	/* RSMRST_N */
	LL_GPIO_SetOutputPin(GPIOA, LL_GPIO_PIN_1);
	d_print("RSMRST_N is DEASSERTED\r\n");

	/* PWRBTN_N interrupt */

	/* SLP_S45_N interrupt */

//	vpp_on(1);
//	d_print("PSU VPP is ON\r\n");
//
//	vddq_vtt_vccref_on(1);
//	d_print("PSU VDDQ, VTT and VCCREF are ON\r\n");
//
//	/* SLP_S3_N interrupt */
//
//	LL_mDelay(10);
//
//	vccsram_on(1);
//	d_print("PSU VCCSRAM is ON\r\n");
//
//	LL_mDelay(10);
//
//	vccp_on(1);
//	d_print("PSU VCCP is ON\r\n");

	/* COREPWEROR interrupt */
	/* PMU_PLTRST_N interrupt */
	/* CPU_PWRGD interrupt */

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
