/**
 * @file	src/scheduler.c
 *
 * @brief	Определения функций для работы с диспетчером на основе таймера TIM3.
 *
 * @author	Vasily Yurchenko <vasily.v.yurchenko@yandex.ru>
 */

#include "../../cpe_stm32f030r8t6/include/pwm.h"

#include "../../cpe_stm32f030r8t6/include/console.h"
#include "../../cpe_stm32f030r8t6/include/drivers.h"

void pwm_init(void)
{
	LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOA);

	LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_4, LL_GPIO_MODE_ALTERNATE);
	LL_GPIO_SetAFPin_0_7(GPIOA, LL_GPIO_PIN_4, LL_GPIO_AF_2);
	LL_GPIO_SetPinSpeed(GPIOA, LL_GPIO_PIN_4, LL_GPIO_SPEED_FREQ_HIGH);
//	LL_GPIO_SetPinOutputType(GPIOA, LL_GPIO_PIN_4, LL_GPIO_OUTPUT_PUSHPULL);
//	LL_GPIO_SetPinPull(GPIOA, LL_GPIO_PIN_6, LL_GPIO_PULL_UP);

	LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM3);

	LL_TIM_SetPrescaler(TIM3, 18);

	LL_TIM_EnableARRPreload(TIM3);

	LL_TIM_SetAutoReload(TIM3, 100);

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
	if (duty < 10)
		duty = 10;

	if (duty > 100)
		duty = 100;

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
	d_print("T3_H: SR 0x%08X\r\n", tim3_sr);
//	d_print("T3_H: CCR1 0x%08X\r\n", __func__, READ_BIT(TIM3->CCR1, 0xFFFFFFFF));
//	d_print("T3_H: CCR2 0x%08X\r\n", __func__, READ_BIT(TIM3->CCR2, 0xFFFFFFFF));
//	LL_TIM_ClearFlag_UPDATE(TIM3);
//	LL_TIM_ClearFlag_CC2(TIM3);
	WRITE_REG(TIM3->SR, 0x0);
}
