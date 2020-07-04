/**
 * @file	src/scheduler.c
 *
 * @brief	Определения функций для работы с диспетчером на основе таймера TIM2.
 *
 * @author	Vasily Yurchenko <vasily.v.yurchenko@yandex.ru>
 */

#include "drivers.h"
#include "scheduler.h"

void scheduler_init(void)
{

	LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM2);

	LL_TIM_SetPrescaler(TIM2, 100);

	LL_TIM_SetAutoReload(TIM2, 24000);

	/* Set TIM_CR1_URS = 0 */
	LL_TIM_SetUpdateSource(TIM2, LL_TIM_UPDATESOURCE_REGULAR);

	/* Set TIM_CR1_UDIS = 0
	 * By default TIM_CR1_UDIS is 0. */
	LL_TIM_EnableUpdateEvent(TIM2);

	LL_TIM_EnableIT_UPDATE(TIM2);

	NVIC_SetPriority(TIM2_IRQn, 0);

	NVIC_EnableIRQ(TIM2_IRQn);

	LL_TIM_EnableCounter(TIM2);

	LL_TIM_GenerateEvent_UPDATE(TIM2);
}

void scheduler_close(void)
{
	LL_TIM_DisableCounter(TIM2);
	LL_TIM_DisableIT_UPDATE(TIM2);
	NVIC_DisableIRQ(TIM2_IRQn);
}


void __attribute__((weak)) scheduler_process(void)
{
}

/*
 *	Обработчик прерывания событий таймера TIM2.
 */
void TIM2_IRQHandler(void)
{
	scheduler_process();
	LL_TIM_ClearFlag_UPDATE(TIM2);
}
