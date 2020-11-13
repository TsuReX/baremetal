/**
 * @file	src/psu_main.c
 *
 * @brief	Объявление точки вхождения в C-код.
 *
 * @author	Vasily Yurchenko <vasily.v.yurchenko@yandex.ru>
 */

#include "drivers.h"
#include "init.h"
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

void pwm_init(void)
{
	LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_6, LL_GPIO_MODE_ALTERNATE);
	LL_GPIO_SetAFPin_0_7(GPIOA, LL_GPIO_PIN_6, LL_GPIO_AF_1);
	LL_GPIO_SetPinSpeed(GPIOA, LL_GPIO_PIN_6, LL_GPIO_SPEED_FREQ_HIGH);

	LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_7, LL_GPIO_MODE_ALTERNATE);
	LL_GPIO_SetAFPin_0_7(GPIOA, LL_GPIO_PIN_7, LL_GPIO_AF_1);
	LL_GPIO_SetPinSpeed(GPIOA, LL_GPIO_PIN_7, LL_GPIO_SPEED_FREQ_HIGH);

	LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM3);

	LL_TIM_SetPrescaler(TIM3, 18);

	LL_TIM_EnableARRPreload(TIM3);

	LL_TIM_SetAutoReload(TIM3, 1000);

	/* Set TIM_CR1_URS = 0 */
//	LL_TIM_SetUpdateSource(TIM3, LL_TIM_UPDATESOURCE_COUNTER);

	/* Set TIM_CR1_UDIS = 0
	 * By default TIM_CR1_UDIS is 0. */
//	LL_TIM_EnableUpdateEvent(TIM3);

	LL_TIM_OC_SetMode(TIM3, LL_TIM_CHANNEL_CH2, LL_TIM_OCMODE_PWM1);

	LL_TIM_OC_SetCompareCH2(TIM3, 10);

	LL_TIM_OC_EnablePreload(TIM3, LL_TIM_CHANNEL_CH2);

//	LL_TIM_OC_EnableFast(TIM3, LL_TIM_CHANNEL_CH2);

//	LL_TIM_OC_ConfigOutput(TIM3, LL_TIM_CHANNEL_CH2, LL_TIM_OCPOLARITY_HIGH | LL_TIM_OCIDLESTATE_LOW);

	LL_TIM_EnableIT_CC2(TIM3);

//	NVIC_SetPriority(TIM3_IRQn, 0);
//	NVIC_EnableIRQ(TIM3_IRQn);

	LL_TIM_CC_EnableChannel(TIM3, LL_TIM_CHANNEL_CH2);

	LL_TIM_EnableCounter(TIM3);

	LL_TIM_GenerateEvent_UPDATE(TIM3);
}

void pwd_duty(uint32_t duty)
{
	if (duty < 100)
		duty = 100;

	if (duty > 1000)
		duty = 1000;

	LL_TIM_OC_SetCompareCH2(TIM3, duty);
}

void pwm_close(void)
{
//	LL_TIM_DisableCounter(TIM3);
//	LL_TIM_DisableIT_UPDATE(TIM3);
//	NVIC_DisableIRQ(TIM3_IRQn);
	LL_TIM_DisableCounter(TIM3);
}

void TIM3_IRQHandler(void)
{
	uint32_t tim3_sr = READ_BIT(TIM3->SR, 0xFFFFFFFF);
	d_print("T3_H: SR 0x%08lX\r\n", tim3_sr);
//	d_print("T3_H: CCR1 0x%08X\r\n", __func__, READ_BIT(TIM3->CCR1, 0xFFFFFFFF));
//	d_print("T3_H: CCR2 0x%08X\r\n", __func__, READ_BIT(TIM3->CCR2, 0xFFFFFFFF));
//	LL_TIM_ClearFlag_UPDATE(TIM3);
//	LL_TIM_ClearFlag_CC2(TIM3);
	WRITE_REG(TIM3->SR, 0x0);
}

int main(void)
{
	/** Configure internal subsystems of SoC. */
	soc_init();

	/** Configure board's peripherals. */
	board_init();

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

	uint32_t duty = 10;
	uint32_t up = 1;

	pwm_init();

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

		LL_mDelay(10);

		pwd_duty(duty);

		if (up == 1)
			duty += 1;

		if (up == 0)
			duty -= 1;

		if (duty == 1000)
			up = 0;

		if (duty == 10)
			up = 1;
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
