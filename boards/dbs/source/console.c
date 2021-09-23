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

//#include "ringbuf.h"
#include "console.h"

#include "config.h"
#include "drivers.h"
#include "debug.h"

#include "usbd_cdc_if.h"

uint32_t trans_status = 1;
extern uint32_t usb_inited;
/*
 * @brief	Настройка портов GPIOA 2/3 для реализации каналов приема и передачи USART2.
 */
static void console_gpio_init(void)
{
	LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOA);

	/* USART1 TX */
	LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_2, LL_GPIO_MODE_ALTERNATE);
	LL_GPIO_SetPinSpeed(GPIOA, LL_GPIO_PIN_2, LL_GPIO_SPEED_FREQ_HIGH);

	/* USART1 RX */
	LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_3, LL_GPIO_MODE_ALTERNATE);
	LL_GPIO_SetPinSpeed(GPIOA, LL_GPIO_PIN_3, LL_GPIO_SPEED_FREQ_HIGH);

	LL_GPIO_SetAFPin_0_7(GPIOA, LL_GPIO_PIN_2, LL_GPIO_AF_7);
	LL_GPIO_SetAFPin_0_7(GPIOA, LL_GPIO_PIN_3, LL_GPIO_AF_7);
}

/*
 * @brief Настройка USART2 для работы на скорости 115200, включение прерываний.
 */
static void console_usart2_init(void)
{
	LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_USART2);

	LL_USART_ConfigCharacter(USART2, LL_USART_DATAWIDTH_8B, LL_USART_PARITY_NONE, LL_USART_STOPBITS_1);

	LL_USART_SetBaudRate(USART2, 30000000, LL_USART_OVERSAMPLING_16, 115200);

	LL_USART_EnableDirectionTx(USART2);
	LL_USART_EnableDirectionRx(USART2);

	LL_USART_Enable(USART2);
}

/*
 * @brief Отключение USART1.
 */
static void console_usart2_close(void)
{
	LL_USART_DisableDirectionTx(USART2);
	LL_USART_DisableDirectionRx(USART2);

	LL_USART_Disable(USART2);
}

/*
 *
 */
void console_init(void)
{
	console_gpio_init();

	console_usart2_init();

	printk(INFO, "Console initialized\n\r");
}

/*
 * @brief	Завершение работы консоли, выключение устройств и освобождение ресурсов.
 */
void console_close(void)
{
	console_usart2_close();
}

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

	if (usb_inited == 1) {
		while(trans_status == 0);

		CDC_Transmit_FS((uint8_t *)src_buffer, src_buffer_size);
	}
	return i;
}

size_t console_read(const uint8_t *dst_buffer, size_t dst_buffer_size, uint32_t usec_timeout)
{
	return 0;
}

