/*
 * spi_flash.c
 *
 *  Created on: Nov 25, 2020
 *      Author: vasily
 */
#include "spi_flash.h"
#include "spi.h"
#include "config.h" /* TODO: Убрать это включение отсюда.*/
#include "debug.h" /* TODO: Убрать это включение отсюда.*/

void spi_flash_test()
{
	uint8_t flash_jdec_id[3];
	spi_chip_activate(FLASH_CHANNEL);
	spi_flash_jdec_id_read(flash_jdec_id);
	spi_chip_deactivate(FLASH_CHANNEL);

	printk(INFO, "JDEC Flash ID %02X%02X%02X\r\n", flash_jdec_id[0], flash_jdec_id[1], flash_jdec_id[2]);

	uint8_t data[256];
	spi_chip_activate(FLASH_CHANNEL);
	spi_flash_data_read(data, sizeof(data));
	spi_chip_deactivate(FLASH_CHANNEL);

	size_t i = 0;
	printk(INFO, "Flash data:\r\n");
	for (; i < 256; i++) {
		printk(INFO, "%x02 ", data[i]);
		if (((i + 1) % 16) == 0) {
			printk(INFO, "\r\n");
		}
	}
}

void spi_flash_jdec_id_read(uint8_t flash_jdec_id[3])
{
	uint8_t	cmd = 0x9F;
	spi_data_xfer(&cmd, NULL, 1);
	spi_data_xfer(NULL, flash_jdec_id, 3);
}

void spi_flash_data_read(uint8_t *dst_buf, size_t data_size)
{
	uint8_t	buffer[4] = {0x03, 0x00, 0x00, 0x00};
	spi_data_xfer(buffer, NULL, 4);
	spi_data_xfer(NULL, dst_buf, data_size);
}
