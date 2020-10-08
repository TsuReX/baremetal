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

volatile uint32_t power_btn_pressed = 0;
volatile uint32_t reset_btn_pressed = 0;

volatile uint32_t timer_ticks = 0;

#define GO_LOW		0
#define GO_HIGH		1
#define LOW			2
#define HIGH		3

volatile uint32_t SLP_S45_N = LOW;
volatile uint32_t SLP_S3_N = LOW;

void exti_0_1_irq_handler(void)
{
	d_print("%s()\r\n", __func__);
	LL_EXTI_ClearFlag_0_31(LL_EXTI_LINE_1 | LL_EXTI_LINE_0);
}

void exti_2_3_irq_handler(void)
{
//	d_print("%s()\r\n", __func__);

	/* SLP_S45_N */
	if (LL_EXTI_IsActiveFlag_0_31(LL_EXTI_LINE_2)){
		if (SLP_S45_N == LOW){
			SLP_S45_N = GO_HIGH;
		} else {
			SLP_S45_N = GO_LOW;
		}
	}

	/* SLP_S3_N */
	if (LL_EXTI_IsActiveFlag_0_31(LL_EXTI_LINE_3)){
		if (SLP_S3_N == LOW){
			SLP_S3_N = GO_HIGH;
		} else {
			SLP_S3_N = GO_LOW;
		}
	}

	LL_EXTI_ClearFlag_0_31(LL_EXTI_LINE_3 | LL_EXTI_LINE_2);
}

void exti_4_15_irq_handler(void)
{
//	d_print("%s()\r\n", __func__);

	/* FP_PWR_BTN_N */
	if (LL_EXTI_IsActiveFlag_0_31(LL_EXTI_LINE_14) == 1) {
		if (power_btn_pressed == 0) {
			LL_EXTI_DisableFallingTrig_0_31(LL_EXTI_LINE_14);

			LL_GPIO_ResetOutputPin(GPIOA, LL_GPIO_PIN_11);

			power_btn_pressed = 1;
			d_print("PWRBTN is pushed\r\n");
			LL_EXTI_EnableRisingTrig_0_31(LL_EXTI_LINE_14);
		} else {
			LL_EXTI_DisableRisingTrig_0_31(LL_EXTI_LINE_14);

			LL_GPIO_SetOutputPin(GPIOA, LL_GPIO_PIN_11);

			power_btn_pressed = 0;
			d_print("PWRBTN is pulled\r\n");
			LL_EXTI_EnableFallingTrig_0_31(LL_EXTI_LINE_14);
		}
	}

	/* FP_RST_BTN_N */
	if (LL_EXTI_IsActiveFlag_0_31(LL_EXTI_LINE_13) == 1) {
		if (reset_btn_pressed == 0) {
			LL_EXTI_DisableFallingTrig_0_31(LL_EXTI_LINE_13);

			LL_GPIO_ResetOutputPin(GPIOA, LL_GPIO_PIN_12);

			reset_btn_pressed = 1;
			d_print("RSTBTN is pushed\r\n");
			LL_EXTI_EnableRisingTrig_0_31(LL_EXTI_LINE_13);
		} else {
			LL_EXTI_DisableRisingTrig_0_31(LL_EXTI_LINE_13);

			LL_GPIO_SetOutputPin(GPIOA, LL_GPIO_PIN_12);

			reset_btn_pressed = 0;
			d_print("RSTBTN is pulled\r\n");
			LL_EXTI_EnableFallingTrig_0_31(LL_EXTI_LINE_13);
		}
	}

	LL_EXTI_ClearFlag_0_31(LL_EXTI_LINE_ALL_0_31 & ~(LL_EXTI_LINE_3 | LL_EXTI_LINE_2 | LL_EXTI_LINE_1 | LL_EXTI_LINE_0));
}

void gpio_irq_enable(void)
{
	LL_APB1_GRP2_EnableClock(LL_APB1_GRP2_PERIPH_SYSCFG);

	/** SLP_S45_N */
	LL_SYSCFG_SetEXTISource(LL_SYSCFG_EXTI_PORTC, LL_SYSCFG_EXTI_LINE2);
	LL_EXTI_EnableIT_0_31(LL_EXTI_LINE_2);
	LL_EXTI_EnableFallingTrig_0_31(LL_EXTI_LINE_2);
	LL_EXTI_EnableRisingTrig_0_31(LL_EXTI_LINE_2);

	/** SLP_S3_N */
	LL_SYSCFG_SetEXTISource(LL_SYSCFG_EXTI_PORTC, LL_SYSCFG_EXTI_LINE3);
	LL_EXTI_EnableIT_0_31(LL_EXTI_LINE_3);
	LL_EXTI_EnableFallingTrig_0_31(LL_EXTI_LINE_3);
	LL_EXTI_EnableRisingTrig_0_31(LL_EXTI_LINE_3);

	/** FP_RST_BTN_N */
	LL_SYSCFG_SetEXTISource(LL_SYSCFG_EXTI_PORTC, LL_SYSCFG_EXTI_LINE13);
	LL_GPIO_SetPinPull(GPIOC, LL_GPIO_PIN_13, LL_GPIO_PULL_UP);
	LL_EXTI_EnableIT_0_31(LL_EXTI_LINE_13);
	LL_EXTI_EnableFallingTrig_0_31(LL_EXTI_LINE_13);

	/** FP_PWR_BTN_N */
	LL_SYSCFG_SetEXTISource(LL_SYSCFG_EXTI_PORTC, LL_SYSCFG_EXTI_LINE14);
	LL_GPIO_SetPinPull(GPIOC, LL_GPIO_PIN_14, LL_GPIO_PULL_UP);
	LL_EXTI_EnableIT_0_31(LL_EXTI_LINE_14);
	LL_EXTI_EnableFallingTrig_0_31(LL_EXTI_LINE_14);


	NVIC_EnableIRQ(EXTI0_1_IRQn);
	NVIC_SetPriority(EXTI0_1_IRQn, 0);

	NVIC_EnableIRQ(EXTI2_3_IRQn);
	NVIC_SetPriority(EXTI2_3_IRQn, 0);

	NVIC_EnableIRQ(EXTI4_15_IRQn);
	NVIC_SetPriority(EXTI4_15_IRQn, 0);
}

void gpio_irq_disable(void)
{
	NVIC_DisableIRQ(EXTI0_1_IRQn);

	NVIC_DisableIRQ(EXTI2_3_IRQn);

	NVIC_DisableIRQ(EXTI4_15_IRQn);

	/** SLP_S45_N */
	LL_EXTI_DisableIT_0_31(LL_EXTI_LINE_2);
	LL_EXTI_DisableFallingTrig_0_31(LL_EXTI_LINE_2);
	LL_EXTI_DisableRisingTrig_0_31(LL_EXTI_LINE_2);

	/** SLP_S3_N */
	LL_EXTI_DisableIT_0_31(LL_EXTI_LINE_3);
	LL_EXTI_DisableFallingTrig_0_31(LL_EXTI_LINE_3);
	LL_EXTI_DisableRisingTrig_0_31(LL_EXTI_LINE_3);

	/** FP_RST_BTN_N */
	LL_EXTI_DisableIT_0_31(LL_EXTI_LINE_13);
	LL_EXTI_DisableFallingTrig_0_31(LL_EXTI_LINE_13);
	LL_EXTI_DisableRisingTrig_0_31(LL_EXTI_LINE_13);

	/** FP_PWR_BTN_N */
	LL_EXTI_DisableIT_0_31(LL_EXTI_LINE_14);
	LL_EXTI_DisableFallingTrig_0_31(LL_EXTI_LINE_14);
	LL_EXTI_DisableRisingTrig_0_31(LL_EXTI_LINE_14);

	LL_APB1_GRP2_DisableClock(LL_APB1_GRP2_PERIPH_SYSCFG);
}

void scheduler_process(void)
{
	++timer_ticks;

	if ((timer_ticks % 10000) == 0) {
		LL_GPIO_SetOutputPin(GPIOA, LL_GPIO_PIN_5);
		LL_GPIO_ResetOutputPin(GPIOA, LL_GPIO_PIN_4);

	} else if ((timer_ticks % 5000) == 0) {
		LL_GPIO_ResetOutputPin(GPIOA, LL_GPIO_PIN_5);
		LL_GPIO_SetOutputPin(GPIOA, LL_GPIO_PIN_4);
	}

	//		d_print("Elapsed time %ld x 0.1ms\r\n", timer_ticks);
	//		LL_mDelay(DELAY_500_MS);
	//		LL_GPIO_SetOutputPin(GPIOA, LL_GPIO_PIN_5);
	//		LL_GPIO_ResetOutputPin(GPIOA, LL_GPIO_PIN_4);
	//		LL_mDelay(DELAY_500_MS);
	//		LL_GPIO_ResetOutputPin(GPIOA, LL_GPIO_PIN_5);
	//		LL_GPIO_SetOutputPin(GPIOA, LL_GPIO_PIN_4);
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

void vddq_vtt_on(uint32_t waiting)
{
	/** VDDQ_VTT_EN */
	LL_GPIO_SetOutputPin(GPIOB, LL_GPIO_PIN_11);

	/** VDDQ_VTT_PG */
	if (waiting != 0)
		while (LL_GPIO_IsInputPinSet(GPIOC, LL_GPIO_PIN_9) != 1);
}

void vddq_vtt_off(uint32_t waiting)
{
	/** VDDQ_VTT_EN */
	LL_GPIO_ResetOutputPin(GPIOB, LL_GPIO_PIN_11);

	/** VDDQ_VTT_PG */
	if (waiting != 0)
		while (LL_GPIO_IsInputPinSet(GPIOC, LL_GPIO_PIN_9) == 1);
}

void vccref_on()
{
	/** VCCREF_EN */
	LL_GPIO_SetOutputPin(GPIOA, LL_GPIO_PIN_15);
}

void vccref_off()
{
	/** VCCREF_EN */
	LL_GPIO_ResetOutputPin(GPIOA, LL_GPIO_PIN_15);
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

void v1p8_on(uint32_t waiting)
{
	/** V1P8_EN */
	LL_GPIO_SetOutputPin(GPIOB, LL_GPIO_PIN_1);

	/** V1P8_PG */
	if (waiting != 0)
		while (LL_GPIO_IsInputPinSet(GPIOF, LL_GPIO_PIN_4) != 1);
}

void v1p8_off(uint32_t waiting)
{
	/** V1P8_EN */
	LL_GPIO_ResetOutputPin(GPIOB, LL_GPIO_PIN_1);

	/** V1P8_PG */
	if (waiting != 0)
		while (LL_GPIO_IsInputPinSet(GPIOF, LL_GPIO_PIN_4) == 1);
}

void v1p05_on(uint32_t waiting)
{
	/** V1P05_EN */
	LL_GPIO_SetOutputPin(GPIOC, LL_GPIO_PIN_11);

	/** V1P05_PG */
	if (waiting != 0)
		while (LL_GPIO_IsInputPinSet(GPIOB, LL_GPIO_PIN_3) != 1);
}

void v1p05_off(uint32_t waiting)
{
	/** V1P05_EN */
	LL_GPIO_ResetOutputPin(GPIOC, LL_GPIO_PIN_11);

	/** V1P05_PG */
	if (waiting != 0)
		while (LL_GPIO_IsInputPinSet(GPIOB, LL_GPIO_PIN_3) == 1);
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

	v3p3_on(1);
	d_print("PSU V3P3 is ON\r\n");

	v1p8_on(1);
	d_print("PSU V1P8 is ON\r\n");

	LL_mDelay(15);

	vnn_on(1);
	d_print("PSU VNN is ON\r\n");

	LL_mDelay(5);

	v1p05_on(1);
	d_print("PSU V1P05 is ON\r\n");

	LL_mDelay(10);

	/* RSMRST_N */
	LL_GPIO_SetOutputPin(GPIOA, LL_GPIO_PIN_1);
	d_print("RSMRST_N is DEASSERTED\r\n");

	LL_mDelay(200);

	gpio_irq_enable();

	while (1) {

//		if (power_btn_pushed == 1) {
//			d_print("PWRBTN is pushed\r\n");
//		}
//
//		if (power_btn_pulled == 1) {
//			d_print("PWRBTN is pulled\r\n");
//		}
//
//		if (reset_btn_pushed == 1) {
//			d_print("RSTBTN is pushed\r\n");
//		}
//
//		if (reset_btn_pulled == 1) {
//			d_print("RSTBTN is pulled\r\n");
//		}

		if (SLP_S3_N == GO_LOW) {
			d_print("SLP_S3_N is ASSERTED\r\n");

			/* COREPWEROR */
			LL_GPIO_ResetOutputPin(GPIOC, LL_GPIO_PIN_1);
			d_print("COREPWEROK is ASSERTED\r\n");

			LL_mDelay(1);

			vccp_off(1);
			d_print("PSU VCCP is OFF\r\n");

			LL_mDelay(1);

			vpp_off(1);
			d_print("PSU VPP is OFF\r\n");

			LL_mDelay(1);

			vccsram_off(1);
			d_print("PSU VCCSRAM is OFF\r\n");

			LL_mDelay(1);

			vccref_off();
			d_print("PSU VCCREF is OFF\r\n");

			SLP_S3_N = LOW;
		}

		if (SLP_S3_N == GO_HIGH && SLP_S45_N == HIGH) {
			d_print("SLP_S3_N is DEASSERTED\r\n");

			LL_mDelay(5);

			vccref_on();
			d_print("PSU VCCREF is ON\r\n");

			LL_mDelay(5);

			vccsram_on(1);
			d_print("PSU VCCSRAM is ON\r\n");
			vpp_on(1);
			d_print("PSU VPP is ON\r\n");

			LL_mDelay(5);

			vccp_on(1);
			d_print("PSU VCCP is ON\r\n");

			/* COREPWEROR */
			LL_GPIO_SetOutputPin(GPIOC, LL_GPIO_PIN_1);
			d_print("COREPWEROK is DEASSERTED\r\n");

			SLP_S3_N = HIGH;
		}

		if (SLP_S45_N == GO_LOW && SLP_S3_N == LOW) {
			d_print("SLP_S45_N is ASSERTED\r\n");

			vddq_vtt_off(1);
			d_print("PSU VDDQ and VTT are OFF\r\n");
			SLP_S45_N = LOW;
		}

		if (SLP_S45_N == GO_HIGH) {
			d_print("SLP_S45_N is DEASSERTED\r\n");

			vddq_vtt_on(1);
			d_print("PSU VDDQ and VTT are ON\r\n");

			SLP_S45_N = HIGH;
		}

		LL_mDelay(1);
	}


//	/* SLP_S45_N */
//	while (LL_GPIO_IsInputPinSet(GPIOC, LL_GPIO_PIN_2) != 1);
//	d_print("SLP_S45_N is DEASSERTED\r\n");
//
//	vddq_vtt_on(1);
//	d_print("PSU VDDQ and VTT are ON\r\n");
//
//	/* SLP_S3_N */
//	while (LL_GPIO_IsInputPinSet(GPIOC, LL_GPIO_PIN_3) != 1);
//	d_print("SLP_S3_N is DEASSERTED\r\n");
//
//	LL_mDelay(5);
//
//	vccref_on();
//	d_print("PSU VCCREF is ON\r\n");
//
//	LL_mDelay(5);
//
//	vccsram_on(1);
//	d_print("PSU VCCSRAM is ON\r\n");
//	vpp_on(1);
//	d_print("PSU VPP is ON\r\n");
//
//	LL_mDelay(5);
//
//	vccp_on(1);
//	d_print("PSU VCCP is ON\r\n");
//
//	/* COREPWEROR */
//	LL_GPIO_SetOutputPin(GPIOC, LL_GPIO_PIN_1);
//	d_print("COREPWEROR is DEASSERTED\r\n");
//
//	d_print("BIOS should start working\r\n");

}
