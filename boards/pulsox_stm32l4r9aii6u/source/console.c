/**
 * @file	src/console.c
 *
 * @brief	Определены функции определяющие поведение текстовой консоли
 * 			и настройки необходимых для ее работы аппаратных посистем (USART1 и DMA4-5).
 *
 * @author	Vasily Yurchenko <vasily.v.yurchenko@yandex.ru>
 */

#include "console.h"

#include <stdarg.h>
#include <stdio.h>
#include "drivers.h"
#include "ringbuf.h"

/** Кольцевой буфер передачи данных через USART 1. */
static struct ring_buf tx_rb;

static void console_gpio_init(void)
{
	LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOB);

	LL_GPIO_InitTypeDef GPIO_InitStruct = {0};
	/**USART1 GPIO Configuration
	PB6   ------> USART1_TX
	PB7   ------> USART1_RX
	*/
	GPIO_InitStruct.Pin = LL_GPIO_PIN_6|LL_GPIO_PIN_7;
	GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
	GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
	GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
	GPIO_InitStruct.Alternate = LL_GPIO_AF_7;

	LL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}

/*
 * @brief Настройка USART1 для работы на скорости 115200, включение прерываний.
 */
static void console_usart1_init(void)
{
	LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_USART1);

	LL_RCC_SetUSARTClockSource(LL_RCC_USART1_CLKSOURCE_PCLK2);

	LL_USART_InitTypeDef USART_InitStruct = {0};

	USART_InitStruct.PrescalerValue = LL_USART_PRESCALER_DIV1;
	USART_InitStruct.BaudRate = 1500000;
	USART_InitStruct.DataWidth = LL_USART_DATAWIDTH_8B;
	USART_InitStruct.StopBits = LL_USART_STOPBITS_1;
	USART_InitStruct.Parity = LL_USART_PARITY_NONE;
	USART_InitStruct.TransferDirection = LL_USART_DIRECTION_TX_RX;
	USART_InitStruct.HardwareFlowControl = LL_USART_HWCONTROL_NONE;
	USART_InitStruct.OverSampling = LL_USART_OVERSAMPLING_16;
	LL_USART_Init(USART1, &USART_InitStruct);
	LL_USART_SetTXFIFOThreshold(USART1, LL_USART_FIFOTHRESHOLD_1_8);
	LL_USART_SetRXFIFOThreshold(USART1, LL_USART_FIFOTHRESHOLD_1_8);
	LL_USART_DisableFIFO(USART1);
	LL_USART_ConfigAsyncMode(USART1);
	LL_USART_Enable(USART1);
}

void console_init(void)
{
	console_gpio_init();

	console_usart1_init();
}

/*
 * @brief	Завершение работы консоли, выключение устройств и освобождение ресурсов.
 */
void console_close(void)
{
}

void print(const char *format, ...)
{
	va_list argptr;
	char	str[512];
	int		sz;

	va_start(argptr, format);
	sz = vsnprintf(str, 512, format, argptr);
	if (sz > 0) {
		rb_store_data(&tx_rb, str, sz);
	}
	va_end(argptr);
}
#if 0
void d_print(const char *format, ...)
{
	va_list 	argptr;
	char		str[512];
	int			sz;
	uint32_t	ms_timeout = TRANSMIT_TIMEOUT * 1000;

	va_start(argptr, format);
	sz = vsnprintf(str, 512, format, argptr);
	va_end(argptr);

	if (sz <= 0) {
		return;
	}

	/*
	 * TODO: необходимо на момент работы данной функции дождаться завершения текущей передачи,
	 * сохранить состояние контроллера USART1, запретить все прерывания, произвести передачу,
	 * а после этого восстановить состояние контроллера USART1.
	 */
	size_t i;
	for (i = 0; i < sz; ++i) {
		LL_USART_TransmitData8(USART1, str[i]);

		/** Ожидать окончания передачи ms_timeout миллисекунд.*/
		do {
//			LL_mDelay(1);
			--ms_timeout;
			if (ms_timeout == 0)
				/* TODO: Рассмотреть возможные варианты действий в случае превышения таймаута. */
				break;
		} while (LL_USART_IsActiveFlag_TXE(USART1) != 1);
	}
}
#endif

size_t console_write(const uint8_t *src_buffer, size_t src_buffer_size, uint32_t usec_timeout)
{
	size_t i;
	for (i = 0; i < src_buffer_size; ++i) {
		LL_USART_TransmitData8(USART2, src_buffer[i]);

#if defined(PERIOD_TIMEOUT)
		struct period timeout;
		period_start(&timeout, usec_timeout);
#endif

	do {
			__DSB();
#if !defined(PERIOD_TIMEOUT)
			--usec_timeout;
			if (usec_timeout == 0)
				break;
#else
			if (is_period_expired(&timeout, NOT_RESTART_PERIOD))
				break;
#endif
		} while (LL_USART_IsActiveFlag_TXE(USART2) != 1);
	}
	return i;
}

size_t console_read(const uint8_t *dst_buffer, size_t dst_buffer_size, uint32_t usec_timeout)
{
	return 0;
}
