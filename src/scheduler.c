/**
 * @file	src/scheduler.c
 *
 * @brief	Определения функций для работы с диспетчером на основе таймера TIM6.
 *
 * @author	Vasily Yurchenko <vasily.v.yurchenko@yandex.ru>
 */

#include "main.h"
#include "scheduler.h"

void sched_init(void)
{
	NVIC_SetPriority(TIM6_DAC1_IRQn, 0);
	NVIC_EnableIRQ(TIM6_DAC1_IRQn);

	LL_APB2_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM6);

	LL_TIM_SetUpdateSource(TIM6, LL_TIM_UPDATESOURCE_COUNTER);

	LL_TIM_SetAutoReload(TIM6, 48000);

	LL_TIM_DisableUpdateEvent(TIM6);

	LL_TIM_SetPrescaler(TIM6, 1000);

	LL_TIM_EnableIT_UPDATE(TIM6);

	LL_TIM_EnableCounter(TIM6);

	/* !!!!!! */
	LL_TIM_GenerateEvent_UPDATE(TIM6);
}

/*
 *	Обработчик прерывания событий таймера TIM6.
 */
void TIM6_DAC1_IRQHandler(void)
{

}
