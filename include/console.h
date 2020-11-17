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

#include <stddef.h>
#include <stdint.h>

/*
 * Дескриптор команды.
 */
struct console_command {

};

/**
 * @brief	Производит настройку текстовой консоли и
 * 			необходимых для ее работы аппаратных подсистем,
 * 			Начинает прием данных.
 * 			Включаются и настраиваются следующие подсистемы:
 * 			GPIO (порт A, контакты 9(TX) и 10(RX));
 * 			USART (контроллер 1, скорость 115200);
 * 			DMA (каналы 4(TX) и 5(RX));
 * 			NVIC.
 */
void console_init(void);

/*
 * @brief	Производит освобождение всех занятых для функционирования консоли ресурсов.
 */
void console_close(void);

/*
 * @brief	Обрабатывает приемный буфер с символами, передает буфер с символами на передачу.
 * 			Функция должна вызывается с необходимой периодичностью.
 *
 * @retval	TODO: Описать
 */
int32_t console_process(void);

/*
 * @brief	Добавляет команду в список команд распознаваемых в тексте и доступных для выполнения.
 *
 * @param[in] csl_cmd	дескриптор добавляемой команды
 *
 * @retval	TODO: Описать
 */
int32_t console_add_cmd(const struct console_command *csl_cmd);

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

#define INFO	2
#define ERR		1
#define CRIT	0

uint32_t log_level_get(void);

void printk(uint32_t debug_level, const char *format, ...) __attribute__((format(printf, 2, 3)));

#endif /* _CONSOLE_H_ */
