/*
 * spi.c
 *
 *  Created on: Sep 8, 2020
 *      Author: user
 */

#include "spi.h"
#include "console.h"
#include "drivers.h"

void spi_init(void) {

	NVIC_SetPriority(SPI1_IRQn, 0);
	NVIC_EnableIRQ(SPI1_IRQn);
	LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_SPI1);

	/* Configure SPI1 communication */
	LL_SPI_SetBaudRatePrescaler(SPI1, LL_SPI_BAUDRATEPRESCALER_DIV256);
	LL_SPI_SetTransferDirection(SPI1, LL_SPI_FULL_DUPLEX);
	LL_SPI_SetClockPhase(SPI1, LL_SPI_PHASE_2EDGE);
	LL_SPI_SetClockPolarity(SPI1, LL_SPI_POLARITY_HIGH);
	/* Reset value is LL_SPI_MSB_FIRST */
	//  LL_SPI_SetTransferBitOrder(SPI2, LL_SPI_MSB_FIRST);
	LL_SPI_SetDataWidth(SPI1, LL_SPI_DATAWIDTH_8BIT);
	LL_SPI_SetNSSMode(SPI1, LL_SPI_NSS_SOFT);
	LL_SPI_SetMode(SPI1, LL_SPI_MODE_MASTER);

//	LL_SPI_EnableIT_RXNE(SPI1);
//	LL_SPI_EnableIT_TXE(SPI1);
//	LL_SPI_EnableIT_ERR(SPI1);

	LL_SPI_Enable(SPI1);
}

uint32_t spi_data_xfer(uint8_t *src_buf, uint8_t *dst_buf, size_t data_size)
{
	size_t byte_idx = 0;
	if (src_buf == NULL && dst_buf == NULL)
		return 0;

	if (src_buf == NULL) {
		for (; byte_idx < data_size; ++byte_idx) {
			size_t i = 1000;

			while (!LL_SPI_IsActiveFlag_TXE(SPI1) && i-- != 0);
			if (i == 0)
				return byte_idx;

			LL_SPI_TransmitData8(SPI1, 0x0);

			i = 1000;
			while (!LL_SPI_IsActiveFlag_RXNE(SPI1) && i-- != 0);

			if (i == 0)
				return byte_idx;

			dst_buf[byte_idx] = LL_SPI_ReceiveData8(SPI1);
		}
	} else if (dst_buf == NULL) {

		for (; byte_idx < data_size; ++byte_idx) {
			size_t i = 1000;

			while (!LL_SPI_IsActiveFlag_TXE(SPI1) && i-- != 0);
			if (i == 0)
				return byte_idx;

			LL_SPI_TransmitData8(SPI1, src_buf[byte_idx]);

			i = 1000;
			while (!LL_SPI_IsActiveFlag_RXNE(SPI1) && i-- != 0);

			if (i == 0)
				return byte_idx;

			LL_SPI_ReceiveData8(SPI1);
		}
	} else {


		for (; byte_idx < data_size; ++byte_idx) {
			size_t i = 1000;

			while (!LL_SPI_IsActiveFlag_TXE(SPI1) && i-- != 0);
			if (i == 0)
				return byte_idx;

			LL_SPI_TransmitData8(SPI1, src_buf[byte_idx]);

			i = 1000;
			while (!LL_SPI_IsActiveFlag_RXNE(SPI1) && i-- != 0);

			if (i == 0)
				return byte_idx;

			dst_buf[byte_idx] = LL_SPI_ReceiveData8(SPI1);
		}
	}
	return data_size;
}


void spi1_irq_handler(void)
{
//	if ((SPI1->SR & SPI_SR_RXNE) != 0) {
//
//	}
//
//	if ((SPI1->SR & SPI_SR_TXE) != 0) {
//
//	}
//
//	if ((SPI1->SR & SPI_SR_CRCERR) != 0) {
//
//	}
//
//	if ((SPI1->SR & SPI_SR_OVR) != 0) {
//
//	}
//
//	if ((SPI1->SR & SPI_SR_UDR) != 0) {
//
//	}
//
//	if ((SPI1->SR & SPI_SR_MODF) != 0) {
//
//	}
}
