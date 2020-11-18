/*
 * debug.h
 *
 *  Created on: Nov 18, 2020
 *      Author: vasily
 */

#ifndef DEBUG_H_
#define DEBUG_H_

#include "stdint.h"

#define INFO	2
#define ERR		1
#define CRIT	0

uint32_t log_level_get(void);

void printk(uint32_t debug_level, const char *format, ...) __attribute__((format(printf, 2, 3)));

#endif /* INCLUDE_DEBUG_H_ */
