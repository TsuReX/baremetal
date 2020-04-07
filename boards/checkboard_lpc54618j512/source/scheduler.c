/**
 * @file	src/scheduler.c
 *
 * @brief	Определения функций для работы с диспетчером на основе таймера RIT.
 *
 * @author	Vasily Yurchenko <vasily.v.yurchenko@yandex.ru>
 */

#include "drivers.h"
#include "scheduler.h"

void scheduler_init(void)
{
    /* Structure of initialize RIT */
    rit_config_t ritConfig;
    /*
     * ritConfig.enableRunInDebug = false;
     */
    RIT_GetDefaultConfig(&ritConfig);

    /* Init rit module */
    RIT_Init(RIT, &ritConfig);

    /* Set timer period for Compare register. */
    RIT_SetTimerCompare(RIT, CLOCK_GetFreq(kCLOCK_CoreSysClk) / 1000);

    /* Set to enable the Counter register auto clear to zero when the counter value equals the set period. */
    RIT_SetCountAutoClear(RIT, true);

    /* Start counting */
    RIT_StartTimer(RIT);

    /* Enable at the NVIC */
    EnableIRQ(RIT_IRQn);
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
	RIT_ClearStatusFlags(RIT, kRIT_TimerFlag);
	scheduler_process();
	__DSB();
}
