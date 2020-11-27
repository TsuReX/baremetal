/**
 * @file	src/psu_main.c
 *
 * @brief	Объявление точки вхождения в C-код.
 *
 * @author	Vasily Yurchenko <vasily.v.yurchenko@yandex.ru>
 */

#include "console.h"
#include "config.h"
#include "drivers.h"
#include "time.h"
#include "debug.h"

void adc_init()
{
	LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOA);

	/** VAC */
	LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_0, LL_GPIO_MODE_ANALOG);

	/** VPFC */
	LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_1, LL_GPIO_MODE_ANALOG);

	/** STM32_UART1 TX/RX */
//	LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_9, LL_GPIO_MODE_OUTPUT);
//	LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_10, LL_GPIO_MODE_OUTPUT);


	LL_RCC_HSI14_Enable();
	mdelay(1);
	LL_APB1_GRP2_EnableClock(LL_APB1_GRP2_PERIPH_ADC1);
	mdelay(1);
	LL_ADC_SetClock(ADC1, LL_ADC_CLOCK_ASYNC);

	LL_ADC_Disable(ADC1);
	LL_ADC_StartCalibration(ADC1);
	while (LL_ADC_IsCalibrationOnGoing(ADC1));
	LL_ADC_ClearFlag_ADRDY(ADC1);
	LL_ADC_Enable(ADC1);
	LL_ADC_SetResolution(ADC1, LL_ADC_RESOLUTION_12B);

#if 1
	LL_ADC_REG_SetTriggerSource(ADC1, LL_ADC_REG_TRIG_SOFTWARE);
#else
	LL_ADC_REG_SetTriggerSource(ADC1, LL_ADC_REG_TRIG_EXT_TIM1_TRGO);
	LL_ADC_REG_GetTriggerEdge(ADC1, LL_ADC_REG_TRIG_EXT_RISING);
#endif

	LL_ADC_SetSamplingTimeCommonChannels(ADC1, LL_ADC_SAMPLINGTIME_1CYCLE_5);
	LL_ADC_REG_SetSequencerChannels(ADC1, LL_ADC_CHANNEL_0 | LL_ADC_CHANNEL_1);
	LL_ADC_REG_SetSequencerScanDirection(ADC1, LL_ADC_REG_SEQ_SCAN_DIR_FORWARD);
	LL_ADC_REG_SetContinuousMode(ADC1, LL_ADC_REG_CONV_SINGLE);
	LL_ADC_REG_SetSequencerDiscont(ADC1, LL_ADC_REG_SEQ_DISCONT_DISABLE);
	LL_ADC_REG_SetOverrun(ADC1, LL_ADC_REG_OVR_DATA_OVERWRITTEN);
	LL_ADC_SetLowPowerMode(ADC1, LL_ADC_LP_AUTOWAIT);
	LL_ADC_Enable(ADC1);
}

void adc_start_convertion(void)
{
#define VOLTAGE_COUNT	2
#define VREF			(3300)
#define VK				(((VREF * 1000) / 4096) * (310000 / 2370))

	LL_ADC_ClearFlag_ADRDY(ADC1);
	uint16_t voltage[VOLTAGE_COUNT];
	size_t i = 0;
//	uint32_t prev_vac = 0xFFFFFFFF;
	uint32_t cur_vac = 0xFFFFFFFF;
	uint32_t vpfc = 0;

	while (1) {
		if (i == 0)
			LL_ADC_REG_StartConversion(ADC1);

//		while(LL_ADC_REG_IsConversionOngoing(ADC1));

		while (!LL_ADC_IsActiveFlag_EOC(ADC1));
		voltage[i++] = LL_ADC_REG_ReadConversionData12(ADC1);
		LL_ADC_ClearFlag_EOC(ADC1);

		if (LL_ADC_IsActiveFlag_EOS(ADC1)) {
			LL_ADC_ClearFlag_EOS(ADC1);
//			printk(INFO, "VAC: 0x%03X, VPFC: 0x%03X\r\n", voltage[0], voltage[1]);
			cur_vac = (uint32_t)voltage[0] * VK / 1000000;
			vpfc = (uint32_t)voltage[1] * VK / 1000000;
			printk(INFO, "VAC: %ld, VPFC: %ld\r\n", cur_vac, vpfc);
			i %= VOLTAGE_COUNT;
		}
//		if (cur_vac <= prev_vac) {
//			prev_vac = cur_vac;
//			LL_GPIO_ResetOutputPin(GPIOB, LL_GPIO_PIN_1);
//		} else { /* cur_vac > prev_vac */
//			prev_vac = cur_vac;
//			LL_GPIO_SetOutputPin(GPIOB, LL_GPIO_PIN_1);
//		}
//		udelay(100);

		mdelay(100);
	}

}

/**
 * @brief	C-code entry point.
 *
 * @retval	0 in case of success, otherwise error code
 */
int main(void)
{
	/** Configure internal subsystems of SoC. */
	soc_init();

	/** Configure board's peripherals. */
	board_init();

	console_init();
	adc_init();

//	adc_start_convertion();

	while (1) {
		udelay(500000);
		LL_GPIO_ResetOutputPin(GPIOC, LL_GPIO_PIN_8);
		LL_GPIO_ResetOutputPin(GPIOB, LL_GPIO_PIN_1);
		udelay(500000);
		LL_GPIO_SetOutputPin(GPIOC, LL_GPIO_PIN_8);
		LL_GPIO_SetOutputPin(GPIOB, LL_GPIO_PIN_1);
	}
}
