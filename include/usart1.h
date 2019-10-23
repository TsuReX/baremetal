/**
 * @file	include/usart1.h
 *
 * @brief	Объявления функций для работы с устройством USART1.
 *
 * @author	Vasily Yurchenko <vasily.yurchenko@yandex.ru>
 */

#ifndef _USART1_H_
#define _USART1_H_

#include <stddef.h>
#include <stdint.h>

/**
 * @brief	Передает по каналу UART1 данные.
 *
 * @param[in]	data	передаваемые данные
 *
 * @param[in]	size	количество байтов данных
 *
 * @param[in]	ms_timeout	количество миллисекунд за которое должна произойти передача
 *
 * @retval		>= 0	количество переданных байтов данных
 * 				< 0		код ошибки
 */
int uart1_transmit(uint8_t *data, size_t size, size_t ms_timeout);


#endif /* _USART1_H_ */
