/*
 * platform.c
 *
 *  Created on: Nov 15, 2020
 *      Author: vasily
 */

#include "drivers.h"
#include "config.h"

uint32_t log_level_get(void)
{
	return CURRENT_DEBUG_LEVEL;
}
