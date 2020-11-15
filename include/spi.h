#ifndef SPI_H_
#define SPI_H_

#include <stdint.h>
#include <stddef.h>

void spi_init(void);
void spi_chip_activate(uint32_t chip_num);
void spi_chip_deactivate(uint32_t chip_num);
uint32_t spi_data_xfer(const uint8_t *src_buf, uint8_t *dst_buf, size_t data_size);

#endif /* SPI_H_ */
