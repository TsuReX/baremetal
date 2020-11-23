/*
 * utils.h
 *
 *  Created on: Oct 2, 2020
 *      Author: user
 */

#ifndef DELAY_H_
#define DELAY_H_

#include <stdint.h>
/**
 * @brief	Выполняет активную задержку
 *
 * @param[in]	useconds	количество микросекунд задержки,
 * 							по факту длительность задержки
 * 							может быть кратна только значению system_period
 */
void udelay(uint32_t useconds);
void mdelay(uint32_t mseconds);
void u100delay(uint32_t u100seconds);

#endif /* DELAY_H_ */
