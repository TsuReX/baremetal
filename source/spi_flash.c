/*
 * spi_flash.c
 *
 *  Created on: Nov 25, 2020
 *      Author: vasily
 */
#include <string.h>
#include "spi_flash.h"
#include "spi.h"
#include "config.h" /* TODO: Убрать это включение отсюда.*/
#include "debug.h" /* TODO: Убрать это включение отсюда.*/
#include "time.h"

void spi_flash_test()
{
	uint8_t flash_jdec_id[3] = {0x11,0x22,0x33};
	spi_chip_activate(FLASH_CHANNEL);
	spi_flash_jdec_id_read(flash_jdec_id);
	spi_chip_deactivate(FLASH_CHANNEL);

	printk(INFO, "JDEC Flash ID %02X%02X%02X\r\n", flash_jdec_id[0], flash_jdec_id[1], flash_jdec_id[2]);

	uint8_t data[256];
	memset(data, 0xFF, sizeof(data));
	spi_chip_activate(FLASH_CHANNEL);
	spi_flash_data_read(data, sizeof(data));
	spi_chip_deactivate(FLASH_CHANNEL);

	size_t i = 0;
	printk(INFO, "Flash data:\r\n");
	for (; i < 256; i++) {
		mdelay(1);
		printk(INFO, "%02X ", data[i]);
		if (((i + 1) % 16) == 0) {
			printk(INFO, "\r\n");
		}
	}
	printk(INFO, "\r\n");
}



void spi_flash_jdec_id_read(uint8_t flash_jdec_id[3])
{
	uint8_t	cmd = 0x9F;
#if !defined(SPI_DMA)
	spi_data_xfer(&cmd, NULL, 1);
	spi_data_xfer(NULL, flash_jdec_id, 3);
#else
	spi_dma_data_send(&cmd, sizeof(cmd));
	spi_dma_data_recv(flash_jdec_id, 3);
#endif
}

void spi_flash_data_read(uint8_t *dst_buf, size_t data_size)
{
	uint8_t	buffer[4] = {0x03, 0x00, 0x00, 0x00};
#if !defined(SPI_DMA)
	spi_data_xfer(buffer, NULL, 4);
	spi_data_xfer(NULL, dst_buf, data_size);
#else
	spi_dma_data_send(buffer, 4);
	spi_dma_data_recv(dst_buf, data_size);
#endif
}
