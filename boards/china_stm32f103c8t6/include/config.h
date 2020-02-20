/**
 * @file	include/config.h
 *
 * @brief	Объявление функций инициализации SoC и периферии.
 *
 * @author	Vasily Yurchenko <vasily.v.yurchenko@yandex.ru>
 */

#ifndef _CONFIG_H_
#define _CONFIG_H_

/**
 * @brief	Инициализирует устройства и подсистемы внутри SoC.
 */
void soc_init(void);

/**
 * @brief	Инициализирует устройства платформы.
 */
void board_init(void);


#endif /* _CONFIG_H_ */
