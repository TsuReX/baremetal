/**
 * @file	src/scheduler.c
 *
 * @brief	Определения функций для работы с диспетчером на основе таймера RIT.
 *
 * @author	Vasily Yurchenko <vasily.v.yurchenko@yandex.ru>
 */

#include "drivers.h"
#include "scheduler.h"
#include "console.h"

void scheduler_init(void)
{
    rit_config_t ritConfig;
    RIT_GetDefaultConfig(&ritConfig);

    RIT_Init(RIT, &ritConfig);

    RIT_SetTimerCompare(RIT, 180000000 / 1000);

    /* Set to enable the Counter register auto clear to zero when the counter value equals the set period. */
    RIT_SetCountAutoClear(RIT, true);

    /* Start counting */
    RIT_StartTimer(RIT);

    /* Enable at the NVIC */
    EnableIRQ(RIT_IRQn);

    d_print("RIT initialized\n\r");
}

void scheduler_close(void)
{
    DisableIRQ(RIT_IRQn);
    RIT_StopTimer(RIT);
}

void __attribute__((weak)) scheduler_process(void)
{
}

/*
 *	Обработчик прерывания событий таймера RIT.
 */
void RIT_IRQHandler(void)
{
	scheduler_process();
	RIT_ClearStatusFlags(RIT, kRIT_TimerFlag);
	__DSB();
}
