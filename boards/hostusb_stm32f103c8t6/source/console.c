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
#include "config.h"
#include "debug.h"
#include "time.h"

/*
 * @brief	Настройка портов GPIOA 9/10 для реализации каналов приема и передачи USART1.
 */
static void console_gpio_1_init(void)
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
 * @brief	Настройка портов GPIOA 2/3 для реализации каналов приема и передачи USART2.
 */
static void console_gpio_2_init(void)
{
	LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOB);

	/* USART1 TX */
	LL_GPIO_SetPinMode(GPIOB, LL_GPIO_PIN_2, LL_GPIO_MODE_ALTERNATE);
	LL_GPIO_SetPinSpeed(GPIOB, LL_GPIO_PIN_2, LL_GPIO_SPEED_FREQ_HIGH);
	LL_GPIO_SetPinOutputType(GPIOB, LL_GPIO_PIN_2, LL_GPIO_OUTPUT_PUSHPULL);
	LL_GPIO_SetPinPull(GPIOB, LL_GPIO_PIN_2, LL_GPIO_PULL_UP);

	/* USART1 RX */
	LL_GPIO_SetPinMode(GPIOB, LL_GPIO_PIN_3, LL_GPIO_MODE_FLOATING);
	LL_GPIO_SetPinSpeed(GPIOB, LL_GPIO_PIN_3, LL_GPIO_SPEED_FREQ_HIGH);
	LL_GPIO_SetPinPull(GPIOB, LL_GPIO_PIN_3, LL_GPIO_PULL_UP);

}

/*
 * @brief	Настройка портов GPIOB 10/11 для реализации каналов приема и передачи USART3.
 */
static void console_gpio_3_init(void)
{
	LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOB);

	/* USART1 TX */
	LL_GPIO_SetPinMode(GPIOB, LL_GPIO_PIN_10, LL_GPIO_MODE_ALTERNATE);
	LL_GPIO_SetPinSpeed(GPIOB, LL_GPIO_PIN_10, LL_GPIO_SPEED_FREQ_HIGH);
	LL_GPIO_SetPinOutputType(GPIOB, LL_GPIO_PIN_10, LL_GPIO_OUTPUT_PUSHPULL);
	LL_GPIO_SetPinPull(GPIOB, LL_GPIO_PIN_10, LL_GPIO_PULL_UP);

	/* USART1 RX */
	LL_GPIO_SetPinMode(GPIOB, LL_GPIO_PIN_11, LL_GPIO_MODE_FLOATING);
	LL_GPIO_SetPinSpeed(GPIOB, LL_GPIO_PIN_11, LL_GPIO_SPEED_FREQ_HIGH);
	LL_GPIO_SetPinPull(GPIOB, LL_GPIO_PIN_11, LL_GPIO_PULL_UP);

}

/*
 * @brief Настройка USART1 для работы на скорости 1500000, включение прерываний.
 */
static void console_usart1_init(void)
{
	LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_USART1);

	LL_USART_ConfigCharacter(USART1, LL_USART_DATAWIDTH_8B, LL_USART_PARITY_NONE, LL_USART_STOPBITS_1);

	LL_USART_SetBaudRate(USART1, HCLK_FREQ >> 1, 1500000);

#if 0
	NVIC_SetPriority(USART1_IRQn, 0);
	NVIC_EnableIRQ(USART1_IRQn);
#endif

	LL_USART_EnableDirectionTx(USART1);
	LL_USART_EnableDirectionRx(USART1);

	LL_USART_Enable(USART1);
}

/*
 * @brief Настройка USART1 для работы на скорости 1500000, включение прерываний.
 */
static void console_usart2_init(void)
{
	LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_USART2);

	LL_USART_ConfigCharacter(USART2, LL_USART_DATAWIDTH_8B, LL_USART_PARITY_NONE, LL_USART_STOPBITS_1);

	LL_USART_SetBaudRate(USART2, HCLK_FREQ >> 1, 1500000);

#if 0
	NVIC_SetPriority(USART2_IRQn, 0);
	NVIC_EnableIRQ(USART2_IRQn);
#endif

	LL_USART_EnableDirectionTx(USART2);
	LL_USART_EnableDirectionRx(USART2);

	LL_USART_Enable(USART2);
}

/*
 * @brief Настройка USART1 для работы на скорости 1500000, включение прерываний.
 */
static void console_usart3_init(void)
{
	LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_USART3);

	LL_USART_ConfigCharacter(USART3, LL_USART_DATAWIDTH_8B, LL_USART_PARITY_NONE, LL_USART_STOPBITS_1);

	LL_USART_SetBaudRate(USART3, HCLK_FREQ >> 1, 1500000);

#if 0
	NVIC_SetPriority(USART3_IRQn, 0);
	NVIC_EnableIRQ(USART3_IRQn);
#endif

	LL_USART_EnableDirectionTx(USART3);
	LL_USART_EnableDirectionRx(USART3);

	LL_USART_Enable(USART3);
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
 * @brief Отключение USART1.
 */
static void console_usart2_close(void)
{
	LL_USART_DisableDirectionTx(USART2);
	LL_USART_DisableDirectionRx(USART2);

	LL_USART_Disable(USART2);
}

/*
 * @brief Отключение USART1.
 */
static void console_usart3_close(void)
{
	LL_USART_DisableDirectionTx(USART3);
	LL_USART_DisableDirectionRx(USART3);

	LL_USART_Disable(USART3);
}

/*
 *	Настройка USART 1.
 *	Для работы USART 1 настраиваются AHB1, GPIO9/10, USART1, DMA4/5, NVIC.
 */
void console_init(void)
{
	console_gpio_1_init();
	console_usart1_init();

	console_gpio_2_init();
	console_usart2_init();

	console_gpio_3_init();
	console_usart3_init();

	printk(INFO, "Console initialized\n\r");
}

/*
 * @brief	Завершение работы консоли, выключение устройств и освобождение ресурсов.
 */
void console_close(void)
{
	console_usart1_close();
	console_usart2_close();
	console_usart3_close();
}

int32_t console_process(void)
{
	/* Возвращаемое значение может отражать факт переполнения кольцевого буфера приема. */
	return 0;
}

size_t console_write(const uint8_t *src_buffer, size_t src_buffer_size, uint32_t usec_timeout)
{
	size_t i;
	for (i = 0; i < src_buffer_size; ++i) {
		LL_USART_TransmitData8(USART3, src_buffer[i]);

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
		} while (LL_USART_IsActiveFlag_TXE(USART3) != 1);
	}
	return i;
}

size_t console_read(const uint8_t *dst_buffer, size_t dst_buffer_size, uint32_t usec_timeout)
{
	return 0;
}

