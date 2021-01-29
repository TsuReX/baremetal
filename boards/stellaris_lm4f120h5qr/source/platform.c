/*
 * platform.c
 *
 *  Created on: Nov 15, 2020
 *      Author: vasily
 */

#include "drivers.h"
#include "config.h"

void spi_chip_activate(uint32_t chip_num)
{
}

void spi_chip_deactivate(uint32_t chip_num)
{
}

uint32_t log_level_get(void)
{
	return CURRENT_MESSAGE_LEVEL;
}
