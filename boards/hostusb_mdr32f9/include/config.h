/*
 * config.h
 *
 *  Created on: Nov 13, 2020
 *      Author: vasily
 */

#ifndef _CONFIG_H_
#define _CONFIG_H_

/** Частота шины HCLK (работы ядра процессора). */
#define HCLK_FREQ	64000000

#define ALIAS(f) __attribute__ ((weak, alias (#f)))

#endif /* _CONFIG_H_ */
