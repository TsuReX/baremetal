/**
 * @file	src/scheduler.c
 *
 * @brief	Определения функций для работы с диспетчером на основе таймера TIM6.
 *
 * @author	Vasily Yurchenko <vasily.v.yurchenko@yandex.ru>
 */

#include "drivers.h"
#include "scheduler.h"

void scheduler_init(void)
{

	LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM6);

	LL_TIM_SetPrescaler(TIM6, 100);

	LL_TIM_SetAutoReload(TIM6, 48000);

	/* Set TIM_CR1_URS = 0 */
	LL_TIM_SetUpdateSource(TIM6, LL_TIM_UPDATESOURCE_REGULAR);

	/* Set TIM_CR1_UDIS = 0
	 * By default TIM_CR1_UDIS is 0. */
	LL_TIM_EnableUpdateEvent(TIM6);

	LL_TIM_EnableIT_UPDATE(TIM6);

	NVIC_SetPriority(TIM6_DAC1_IRQn, 0);

	NVIC_EnableIRQ(TIM6_DAC1_IRQn);

	LL_TIM_EnableCounter(TIM6);

	LL_TIM_GenerateEvent_UPDATE(TIM6);
}

void scheduler_close(void)
{
	LL_TIM_DisableCounter(TIM6);
	LL_TIM_DisableIT_UPDATE(TIM6);
	NVIC_DisableIRQ(TIM6_DAC1_IRQn);
}


void __attribute__((weak)) scheduler_process(void)
{
}

/*
 *	Обработчик прерывания событий таймера TIM6.
 */
void TIM6_DAC1_IRQHandler(void)
{
	scheduler_process();
	LL_TIM_ClearFlag_UPDATE(TIM6);
}
