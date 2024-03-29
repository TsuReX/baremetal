/**
 * @file	src/console.c
 *
 * @brief	Определены функции определяющие поведение текстовой консоли
 * 			и настройки необходимых для ее работы аппаратных посистем (USART1 и DMA4-5).
 *
 * @author	Vasily Yurchenko <vasily.v.yurchenko@yandex.ru>
 */

#include <stdarg.h>
#include <stdio.h>

#include "ringbuf.h"
#include "console.h"
#include "drivers.h"
#include "config.h"
#include "debug.h"

/** Количество байтов передаваемых через USART1. */
#define SIZE_TO_TRANSMIT	32
/** Количество миллисекунд,
 * за которое должна завершиться передача байта даных через USART1. */
#define TRANSMIT_TIMEOUT	20


/** Кольцевой буфер передачи данных через USART 1. */
static struct ring_buf tx_rb;

/*
 * @brief	Настройка портов GPIO 9/10 для реализации каналов приема и передачи USART1.
 */
static void console_gpio_init(void)
{
	LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

	LL_RCC_SetLPUARTClockSource(LL_RCC_LPUART1_CLKSOURCE_PCLK1);

	/* Peripheral clock enable */
	LL_APB1_GRP2_EnableClock(LL_APB1_GRP2_PERIPH_LPUART1);

	LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOA);
	/**LPUART1 GPIO Configuration
	PA2   ------> LPUART1_TX
	PA3   ------> LPUART1_RX
	*/
	GPIO_InitStruct.Pin = LL_GPIO_PIN_2|LL_GPIO_PIN_3;
	GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
	GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
	GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
	GPIO_InitStruct.Alternate = LL_GPIO_AF_8;
	LL_GPIO_Init(GPIOA, &GPIO_InitStruct);

}

/*
 * @brief Настройка USART1 для работы на скорости 1500000, включение прерываний.
 */
static void console_usart1_init(void)
{
	LL_LPUART_InitTypeDef LPUART_InitStruct = {0};
	LPUART_InitStruct.BaudRate = 115200;
	LPUART_InitStruct.DataWidth = LL_LPUART_DATAWIDTH_8B;
	LPUART_InitStruct.StopBits = LL_LPUART_STOPBITS_1;
	LPUART_InitStruct.Parity = LL_LPUART_PARITY_NONE;
	LPUART_InitStruct.TransferDirection = LL_LPUART_DIRECTION_TX_RX;
	LPUART_InitStruct.HardwareFlowControl = LL_LPUART_HWCONTROL_NONE;
	LL_LPUART_Init(LPUART1, &LPUART_InitStruct);
	LL_LPUART_Enable(LPUART1);
}

/*
 * @brief Отключение USART1.
 */
static void console_usart1_close(void)
{
	LL_LPUART_DisableDirectionTx(LPUART1);
	LL_LPUART_DisableDirectionRx(LPUART1);

	LL_LPUART_Disable(LPUART1);
}

/*
 *	Настройка USART 1.
 *	Для работы USART 1 настраиваются AHB1, GPIO9/10, USART1, DMA4/5, NVIC.
 */
void console_init(void)
{
	/* Настройка GPIO9/10. */
	console_gpio_init();

	/* Настройка USART1. */
	console_usart1_init();

	printk(INFO, "Console initialized\n\r");
}

/*
 * @brief	Завершение работы консоли, выключение устройств и освобождение ресурсов.
 */
void console_close(void)
{
	console_usart1_close();
}

int32_t console_process(void)
{
	/* Возвращаемое значение может отражать факт переполнения кольцевого буфера приема. */
	return 0;
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
		LL_LPUART_TransmitData8(LPUART1, str[i]);

		/** Ожидать окончания передачи ms_timeout миллисекунд.*/
		do {
//			LL_mDelay(1);
			--ms_timeout;
			if (ms_timeout == 0)
				/* TODO: Рассмотреть возможные варианты действий в случае превышения таймаута. */
				break;
		} while (LL_LPUART_IsActiveFlag_TXE(LPUART1) != 1);
	}
}
#endif

size_t console_write(const uint8_t *src_buffer, size_t src_buffer_size, uint32_t usec_timeout)
{
	size_t i;
	for (i = 0; i < src_buffer_size; ++i) {
		LL_LPUART_TransmitData8(LPUART1, src_buffer[i]);

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
		} while (LL_LPUART_IsActiveFlag_TXE(LPUART1) != 1);
	}
	return i;
}

size_t console_read(const uint8_t *dst_buffer, size_t dst_buffer_size, uint32_t usec_timeout)
{
	return 0;
}

