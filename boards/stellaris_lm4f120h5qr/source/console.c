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
	GPIOA->PCTL =	(GPIOPCTL_F1 << (GPIO_PIN_0 << 2)) |	// RX
					(GPIOPCTL_F1 << (GPIO_PIN_1 << 2));		// TX



	/* 1. Enable clocking for GPIOC module */
	SYSCTL->RCGCGPIO |= (0x1 << RCGCGPIO_GPIOC);

	/* 6. GPIO 8-mA Drive Select */
	GPIOC->DR8R =	(1 << GPIO_PIN_4) |
					(1 << GPIO_PIN_5);

	GPIOC->DEN |=	(1 << GPIO_PIN_4) |
					(1 << GPIO_PIN_5);

	/* 2. GPIO Slew Rate Control Select */
	GPIOC->SLR =	(1 << GPIO_PIN_4) |
					(1 << GPIO_PIN_5);

	/* 3. Enable alternate function for pins */
	GPIOC->AFSEL |=	(1 << GPIO_PIN_4) |
					(1 << GPIO_PIN_5);

	/* 4. Select function  */
	GPIOC->PCTL |=	(GPIOPCTL_F2 << (GPIO_PIN_4 << 2)) |	// RX
					(GPIOPCTL_F2 << (GPIO_PIN_5 << 2));		// TX


}

/**
 */
static void console_uart0_init(void)
{
	SYSCTL->RCGCUART |= (0x1 << RCGCUART_UART0);
	SYSCTL->RCGC1 |= SYSCTL_RCGC1_UART0;

	/* !!!!!!!!!!!!!!!!!!!!!! */
	/* UART0 baudrate 1M5 bps */
	/* !!!!!!!!!!!!!!!!!!!!!! */

	UART0->CTL = 0;

	/* DIVISOR = SYSCLK_FREQ / (16 * BAUDRATE)
	 * IBRD = integer(DIVISOR)
	 */
	UART0->IBRD = 3;
//	UART0->IBRD = 43;

	/* FBRD = integer((DIVISOR - IBRD) * 64 + 0.5) */
	UART0->FBRD = 21;
//	UART0->FBRD = 26;

	UART0->LCRH = UART_LCRH_WLEN_8;

	UART0->CC = UART_CC_CS_SYSCLK;

//	UART0->DMACTL = UART_DMACTL_RXDMAE;

	UART0->CTL |= UART_CTL_UARTEN;

	UART0->CTL |= UART_CTL_TXE;

//	UART0->CTL |= UART_CTL_RXE;
}

/**
 */
static void console_uart1_init(void)
{
	SYSCTL->RCGCUART |= (0x1 << RCGCUART_UART1);
	SYSCTL->RCGC1 |= SYSCTL_RCGC1_UART1;

	/* !!!!!!!!!!!!!!!!!!!!!! */
	/* UART1 baudrate 1M5 bps */
	/* !!!!!!!!!!!!!!!!!!!!!! */

	UART1->CTL = 0;

	/* DIVISOR = SYSCLK_FREQ / (16 * BAUDRATE)
	 * IBRD = integer(DIVISOR)
	 */
	UART1->IBRD = 3;
//	UART1->IBRD = 43;

	/* FBRD = integer((DIVISOR - IBRD) * 64 + 0.5) */
	UART1->FBRD = 21;
//	UART1->FBRD = 26;

	UART1->LCRH = UART_LCRH_WLEN_8;

	UART1->CC = UART_CC_CS_SYSCLK;

//	UART1->DMACTL = UART_DMACTL_RXDMAE;

	UART1->CTL |= UART_CTL_UARTEN;

	UART1->CTL |= UART_CTL_TXE;

//	UART1->CTL |= UART_CTL_RXE;
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
	console_uart1_init();
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
		UART1->DR = src_buffer[i];

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
		} while (UART1->FR & UART_FR_TXFE != UART_FR_TXFE);
//		} while (UART0->FR & UART_FR_BUSY != UART_FR_BUSY);
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

