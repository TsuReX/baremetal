/**
 * @file	include/config.h
 *
 * @brief	Объявление функций инициализации SoC и периферии.
 *
 * @author	Vasily Yurchenko <vasily.v.yurchenko@yandex.ru>
 */

#ifndef _CONFIG_H_
#define _CONFIG_H_

/** Частота шины HCLK (работы ядра процессора). */
#define HCLK_FREQ	48000000

/**
 * @brief	Инициализирует устройства и подсистемы внутри SoC.
 */
void soc_init(void);

/**
 * @brief	Инициализирует устройства платформы.
 */
void board_init(void);


#endif /* _CONFIG_H_ */
