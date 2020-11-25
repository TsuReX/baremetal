/*
 * spi_flash.h
 *
 *  Created on: Nov 25, 2020
 *      Author: vasily
 */

#ifndef SPI_FLASH_H_
#define SPI_FLASH_H_

#include <stdint.h>
#include <stddef.h>

void spi_flash_test();

void spi_flash_jdec_id_read(uint8_t flash_jdec_id[3]);
void spi_flash_data_read(uint8_t *dst_buf, size_t data_size);

#endif /* SPI_FLASH_H_ */
