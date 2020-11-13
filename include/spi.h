#ifndef SPI_H_
#define SPI_H_

#include <stdint.h>
#include <stddef.h>

void spi_init(void);
void spi_chip_activate(void);
void spi_chip_deactivate(void);
uint32_t spi_data_xfer(uint8_t *src_buf, uint8_t *dst_buf, size_t data_size);

#endif /* SPI_H_ */
