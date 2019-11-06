/**
 * @file	include/console.h
 *
 * @brief	Объявление функции вывода данных в текстовую консоль.
 *
 * @author	Vasily Yurchenko <vasily.v.yurchenko@yandex.ru>
 */

#ifndef _CONSOLE_H_
#define _CONSOLE_H_

/**
 * @brief	Выводит в текстовую консоль данные на основе строки формата.
 *
 * @param[in]	format	строка формата выводимых данных
 *
 * @param[in]	...	список аргументов, значения которых необходимо вывести в текстовую консоль
 *
 * @retval	void
 */
void print(const char *format, ...) __attribute__((format(printf, 1, 2)));

#endif /* _CONSOLE_H_ */
