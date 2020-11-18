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
	LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOA);

	/* USART1 TX */
	LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_9, LL_GPIO_MODE_ALTERNATE);
	LL_GPIO_SetPinSpeed(GPIOA, LL_GPIO_PIN_9, LL_GPIO_SPEED_FREQ_HIGH);
	LL_GPIO_SetPinOutputType(GPIOA, LL_GPIO_PIN_9, LL_GPIO_OUTPUT_PUSHPULL);
	LL_GPIO_SetPinPull(GPIOA, LL_GPIO_PIN_9, LL_GPIO_PULL_UP);

	/* USART1 RX */
	LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_10, LL_GPIO_MODE_FLOATING);
	LL_GPIO_SetPinSpeed(GPIOA, LL_GPIO_PIN_10, LL_GPIO_SPEED_FREQ_HIGH);
	LL_GPIO_SetPinPull(GPIOA, LL_GPIO_PIN_10, LL_GPIO_PULL_UP);

}

/*
 * @brief Настройка USART1 для работы на скорости 1500000, включение прерываний.
 */
static void console_usart1_init(void)
{
	LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_USART1);

	LL_USART_ConfigCharacter(USART1, LL_USART_DATAWIDTH_8B, LL_USART_PARITY_NONE, LL_USART_STOPBITS_1);

	LL_USART_SetBaudRate(USART1, HCLK_FREQ >> 1, 1500000);

//	NVIC_SetPriority(USART1_IRQn, 0);
//	NVIC_EnableIRQ(USART1_IRQn);

	LL_USART_EnableDirectionTx(USART1);
	LL_USART_EnableDirectionRx(USART1);

	LL_USART_Enable(USART1);
}

/*
 * @brief Отключение USART1.
 */
static void console_usart1_close(void)
{
	LL_USART_DisableDirectionTx(USART1);
	LL_USART_DisableDirectionRx(USART1);

	LL_USART_Disable(USART1);
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
		LL_USART_TransmitData8(USART1, src_buffer[i]);

		do {
			__DSB();
			--usec_timeout;
			if (usec_timeout == 0)
				break;
		} while (LL_USART_IsActiveFlag_TXE(USART1) != 1);
	}
	return i;
}

size_t console_read(const uint8_t *dst_buffer, size_t dst_buffer_size, uint32_t usec_timeout)
{
	return 0;
}

