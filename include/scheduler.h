/**
 * @file	include/scheduler.h
 *
 * @brief	Объявления функций для работы с диспетчером на основе таймера TIM6.
 *
 * @author	Vasily Yurchenko <vasily.v.yurchenko@yandex.ru>
 */

#ifndef __SCHEDULER_H_
#define __SCHEDULER_H_

/*
 *	Инициализирует подсистему диспетчирования на основе таймера TIM6.
 */
void sched_config(void);

#endif /* __SCHEDULER_H_ */
