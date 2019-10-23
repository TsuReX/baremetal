/**
 * @file	include/usart1.h
 *
 * @brief	Определения функций для работы с устройством USART1.
 *
 * @author	Vasily Yurchenko <vasily.yurchenko@yandex.ru>
 */

#include "usart1.h"
#include "main.h"

int uart1_transmit(uint8_t *data, size_t size, size_t ms_timeout)
{
	uint32_t stop_time = SysTick->VAL + ms_timeout;
	size_t i;
	for (i = 0; i < size; ++i) {
		LL_USART_TransmitData8(USART1, data[i]);
		do {
			if (stop_time < SysTick->VAL)
				return -ERR_TIMEOUT;
		} while (LL_USART_IsActiveFlag_TXE(USART1) != 1);
	}
	return i;
}
