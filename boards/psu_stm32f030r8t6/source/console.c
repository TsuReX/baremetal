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

#include "console.h"
#include "drivers.h"
#include "ringbuf.h"
#include "time.h"
#include "debug.h"
#include "config.h"

/*
 * @brief	Настройка портов GPIO 9/10 для реализации каналов приема и передачи USART1.
 */
static void console_gpio_init(void)
{
	LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOA);

	LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_9, LL_GPIO_MODE_ALTERNATE);
	LL_GPIO_SetAFPin_8_15(GPIOA, LL_GPIO_PIN_9, LL_GPIO_AF_1);
	LL_GPIO_SetPinSpeed(GPIOA, LL_GPIO_PIN_9, LL_GPIO_SPEED_FREQ_HIGH);
//	LL_GPIO_SetPinOutputType(GPIOA, LL_GPIO_PIN_9, LL_GPIO_OUTPUT_PUSHPULL);
	LL_GPIO_SetPinPull(GPIOA, LL_GPIO_PIN_9, LL_GPIO_PULL_UP);

	LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_10, LL_GPIO_MODE_ALTERNATE);
	LL_GPIO_SetAFPin_8_15(GPIOA, LL_GPIO_PIN_10, LL_GPIO_AF_1);
	LL_GPIO_SetPinSpeed(GPIOA, LL_GPIO_PIN_10, LL_GPIO_SPEED_FREQ_HIGH);
//	LL_GPIO_SetPinOutputType(GPIOA, LL_GPIO_PIN_10, LL_GPIO_OUTPUT_PUSHPULL);
	LL_GPIO_SetPinPull(GPIOA, LL_GPIO_PIN_10, LL_GPIO_PULL_UP);
}

/*
 * @brief Настройка USART1 для работы на скорости 115200, включение прерываний.
 */
static void console_usart1_init(void)
{
	LL_APB1_GRP2_EnableClock(LL_APB1_GRP2_PERIPH_USART1);

	LL_USART_ConfigCharacter(USART1, LL_USART_DATAWIDTH_8B, LL_USART_PARITY_NONE, LL_USART_STOPBITS_1);
	LL_USART_SetHWFlowCtrl(USART1, LL_USART_HWCONTROL_NONE);
	LL_USART_SetOverSampling(USART1, LL_USART_OVERSAMPLING_16);

	LL_USART_SetBaudRate(USART1, HCLK_FREQ >> 1, LL_USART_OVERSAMPLING_16, 1500000);

	LL_USART_EnableDirectionTx(USART1);
	LL_USART_Enable(USART1);
}

/*
 * @brief Отключение USART1.
 */
static void console_usart1_close(void)
{
	LL_USART_DisableDirectionRx(USART1);
	LL_USART_DisableDirectionTx(USART1);

	LL_USART_Disable(USART1);
}

/*
 * @brief	Отключает канал 5 DMA1 и приемник USART 1.
 */
static void console_stop_reception()
{
	/* TODO: Проанализировать работу функции и
	 * при необходимости реализовать отключение
	 * передачи с ожиднием флагов завершеня
	 * для избежания взникновения ошибок. */

	/* Включить приемник USART1. */
	LL_USART_DisableDirectionRx(USART1);

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

//	printk(INFO, "DEBUG: Console initialized\n\r");
}

/*
 * @brief	Завершение работы консоли, выключение устройств и освобождение ресурсов.
 */
void console_close(void)
{
	console_stop_reception();


	console_usart1_close();

}

size_t console_write(const uint8_t *src_buffer, size_t src_buffer_size, uint32_t usec_timeout)
{
	size_t i;
	for (i = 0; i < src_buffer_size; ++i) {
		LL_USART_TransmitData8(USART1, src_buffer[i]);

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
		} while (LL_USART_IsActiveFlag_TXE(USART1) != 1);
	}
	return i;
}

size_t console_read(const uint8_t *dst_buffer, size_t dst_buffer_size, uint32_t usec_timeout)
{
	return 0;
}

