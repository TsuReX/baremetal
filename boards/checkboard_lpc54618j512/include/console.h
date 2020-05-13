/**
 * @file	include/console.h
 *
 * @brief	Объявлены функции определяющие поведение текстовой консоли
 * 			и настройки необходимых для ее работы аппаратных подсистем (USART1 и DMA4-5).
 *
 * @author	Vasily Yurchenko <vasily.v.yurchenko@yandex.ru>
 */

#ifndef _CONSOLE_H_
#define _CONSOLE_H_

#include <stdint.h>
#include <stddef.h>

/**
 * @brief	Производит настройку текстовой консоли и
 * 			необходимых для ее работы аппаратных подсистем,
 * 			Начинает прием данных.
 */
void console_init(void);

/*
 * @brief	Производит освобождение всех занятых для функционирования консоли ресурсов.
 */
void console_close(void);

/*
 * @brief	Инициирует передачу данных из выходного буфера через DMA-USART.
 *
 * @retval	void
 */
void console_start_transmission();

/*
 * @brief	Обрабатывает приемный буфер с символами, передает буфер с символами на передачу.
 * 			Функция должна вызывается с необходимой периодичностью.
 *
 * @retval	TODO: Описать
 */
int32_t console_process(void);

/*
 * @brief	Выводит в текстовую консоль данные на основе строки формата.
 *
 * @param[in]	format	строка формата выводимых данных
 *
 * @param[in]	...	список аргументов, значения которых необходимо вывести в текстовую консоль
 *
 * @retval	void
 */
void print(const char *format, ...) __attribute__((format(printf, 1, 2)));

/*
 * @brief	Выводит в текстовую консоль данные на основе строки формата.
 * 			В отличие от функции print, данная функция производит вывод напрямую без использования DMA.
 *
 * @param[in]	format	строка формата выводимых данных
 *
 * @param[in]	...	список аргументов, значения которых необходимо вывести в текстовую консоль
 *
 * @retval	void
 */
void d_print(const char *format, ...) __attribute__((format(printf, 1, 2)));

#endif /* _CONSOLE_H_ */
