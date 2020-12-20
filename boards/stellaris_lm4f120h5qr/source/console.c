/**
 * @file	console.c
 *
 * @brief
 *
 * @author	Vasily Yurchenko <vasily.v.yurchenko@yandex.ru>
 */

#include <stdarg.h>
#include <stdio.h>

#include "console.h"
#include "drivers.h"
//#include "config.h"

/**
 */
static void console_gpio_init(void)
{
	/* 1. Enable clocking for GPIOA module */
	SYSCTL->RCGCGPIO |= (0x1 << RCGCGPIO_GPIOA);

	/* 6. GPIO 8-mA Drive Select */
	GPIOA->DR8R =	(1 << GPIO_PIN_0) |
					(1 << GPIO_PIN_1);

	/* 2. GPIO Slew Rate Control Select */
	GPIOA->SLR =	(1 << GPIO_PIN_0) |
					(1 << GPIO_PIN_1);

	/* 3. Enable alternate function for pins */
	GPIOA->AFSEL =	(1 << GPIO_PIN_0) |
					(1 << GPIO_PIN_1);

	/* 4. Select function  */
	GPIOF->PCTL =	(GPIOPCTL_F1 << (GPIO_PIN_1 << 0)) |	// RX
					(GPIOPCTL_F1 << (GPIO_PIN_2 << 1));		// TX

}

/**
 */
static void console_uart0_init(void)
{
	SYSCTL->RCGCUART |= (0x1 << RCGCUART_UART0);

	/* !!!!!!!!!!!!!!!!!!!!!! */
	/* UART0 baudrate 1M5 bps */
	/* !!!!!!!!!!!!!!!!!!!!!! */

	UART0->CTL &= ~UART_CTL_UARTEN;

	/* DIVISOR = SYSCLK_FREQ / (16 * BAUDRATE)
	 * IBRD = integer(DIVISOR)
	 */
//	UART0->IBRD = 3;

	/* FBRD = integer((DIVISOR - IBRD) * 64 + 0.5) */
//	UART0->FBRD = 21;

//	UART0->LCRH |= UART_LCRH_WLEN_8;

//	UART0->CC = UART_CC_CS_SYSCLK;

//	UART0->DMACTL = UART_DMACTL_RXDMAE;

//	UART0->CTL |= UART_CTL_UARTEN;

//	UART0->CTL |= UART_CTL_TXE;

//	UART0->CTL |= UART_CTL_RXE;
}

/**
 */
static void console_uart0_close(void)
{
}

/**
 */
void console_init(void)
{
	console_gpio_init();
	console_uart0_init();
}

/**
 */
void console_close(void)
{
	console_uart0_close();
}

/**
 *
 */
size_t console_write(const uint8_t *src_buffer, size_t src_buffer_size, uint32_t usec_timeout)
{
	size_t i;
	for (i = 0; i < src_buffer_size; ++i) {
		UART0->DR = src_buffer[i];

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
		} while (UART0->FR & UART_FR_TXFE != UART_FR_TXFE);
	}
	return i;
}


/**
 *
 */
size_t console_read(const uint8_t *dst_buffer, size_t dst_buffer_size, uint32_t usec_timeout)
{
	return 0;
}

