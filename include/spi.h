#ifndef SPI_H_
#define SPI_H_

#include <stdint.h>
#include <stddef.h>

#define SPI_DMA 1

void spi_init(void);
void spi_chip_activate(uint32_t chip_num);
void spi_chip_deactivate(uint32_t chip_num);
/* TODO: Implement the function with additional first argument bus_num */
uint32_t spi_data_xfer(/*uin32_t bus_num,*/const uint8_t *src_buf, uint8_t *dst_buf, size_t data_size);

uint32_t spi_dma_data_send(const uint8_t *src_buf, size_t data_size);
uint32_t spi_dma_data_recv(uint8_t *dst_buf, size_t data_size);

#endif /* SPI_H_ */
