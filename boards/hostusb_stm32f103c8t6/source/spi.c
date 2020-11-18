/*
 * spi.c
 *
 *  Created on: Sep 8, 2020
 *      Author: user
 */

#include "spi.h"
#include "console.h"
#include "drivers.h"

static void gpio_spi1_init(void) {

	LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOA);
	/* Configure SCK Pin connected to pin 5 */
	LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_5, LL_GPIO_MODE_ALTERNATE);
	LL_GPIO_SetPinSpeed(GPIOA, LL_GPIO_PIN_5, LL_GPIO_SPEED_FREQ_HIGH);
	LL_GPIO_SetPinPull(GPIOA, LL_GPIO_PIN_5, LL_GPIO_PULL_DOWN);

	/* Configure MISO Pin connected to pin 6 */
	LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_6, LL_GPIO_MODE_ALTERNATE);
	LL_GPIO_SetPinSpeed(GPIOA, LL_GPIO_PIN_6, LL_GPIO_SPEED_FREQ_HIGH);
	LL_GPIO_SetPinPull(GPIOA, LL_GPIO_PIN_6, LL_GPIO_PULL_DOWN);

	/* Configure MOSI Pin connected to pin 7 */
	LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_7, LL_GPIO_MODE_ALTERNATE);
	LL_GPIO_SetPinSpeed(GPIOA, LL_GPIO_PIN_7, LL_GPIO_SPEED_FREQ_HIGH);
	LL_GPIO_SetPinPull(GPIOA, LL_GPIO_PIN_7, LL_GPIO_PULL_DOWN);
}

static void spi1_init(void)
{

	NVIC_SetPriority(SPI1_IRQn, 0);
	NVIC_EnableIRQ(SPI1_IRQn);
	LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_SPI1);

	/* Configure SPI1 communication */
	LL_SPI_SetBaudRatePrescaler(SPI1, LL_SPI_BAUDRATEPRESCALER_DIV8);
	LL_SPI_SetTransferDirection(SPI1, LL_SPI_FULL_DUPLEX);
	LL_SPI_SetClockPhase(SPI1, LL_SPI_PHASE_1EDGE);
	LL_SPI_SetClockPolarity(SPI1, LL_SPI_POLARITY_LOW);
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

static void gpio_spi2_init(void) {

	LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOB);

	LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOA);
	/* Configure SCK Pin connected to pin 13 */
	LL_GPIO_SetPinMode(GPIOB, LL_GPIO_PIN_13, LL_GPIO_MODE_ALTERNATE);
	LL_GPIO_SetPinSpeed(GPIOB, LL_GPIO_PIN_13, LL_GPIO_SPEED_FREQ_HIGH);
	LL_GPIO_SetPinPull(GPIOB, LL_GPIO_PIN_13, LL_GPIO_PULL_DOWN);

	/* Configure MISO Pin connected to pin 14 */
	LL_GPIO_SetPinMode(GPIOB, LL_GPIO_PIN_14, LL_GPIO_MODE_ALTERNATE);
	LL_GPIO_SetPinSpeed(GPIOB, LL_GPIO_PIN_14, LL_GPIO_SPEED_FREQ_HIGH);
	LL_GPIO_SetPinPull(GPIOB, LL_GPIO_PIN_14, LL_GPIO_PULL_DOWN);

	/* Configure MOSI Pin connected to pin 15 */
	LL_GPIO_SetPinMode(GPIOB, LL_GPIO_PIN_15, LL_GPIO_MODE_ALTERNATE);
	LL_GPIO_SetPinSpeed(GPIOB, LL_GPIO_PIN_15, LL_GPIO_SPEED_FREQ_HIGH);
	LL_GPIO_SetPinPull(GPIOB, LL_GPIO_PIN_15, LL_GPIO_PULL_DOWN);
}

static void spi2_init(void)
{

	NVIC_SetPriority(SPI2_IRQn, 0);
	NVIC_EnableIRQ(SPI2_IRQn);
	LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_SPI2);

	/* Configure SPI1 communication */
	LL_SPI_SetBaudRatePrescaler(SPI2, LL_SPI_BAUDRATEPRESCALER_DIV8);
	LL_SPI_SetTransferDirection(SPI2, LL_SPI_FULL_DUPLEX);
	LL_SPI_SetClockPhase(SPI1, LL_SPI_PHASE_1EDGE);
	LL_SPI_SetClockPolarity(SPI1, LL_SPI_POLARITY_LOW);
	/* Reset value is LL_SPI_MSB_FIRST */
	//  LL_SPI_SetTransferBitOrder(SPI2, LL_SPI_MSB_FIRST);
	LL_SPI_SetDataWidth(SPI2, LL_SPI_DATAWIDTH_8BIT);
	LL_SPI_SetNSSMode(SPI2, LL_SPI_NSS_SOFT);
	LL_SPI_SetMode(SPI2, LL_SPI_MODE_MASTER);

//	LL_SPI_EnableIT_RXNE(SPI2);
//	LL_SPI_EnableIT_TXE(SPI2);
//	LL_SPI_EnableIT_ERR(SPI2);

	LL_SPI_Enable(SPI2);
}

void spi_init(void)
{
	gpio_spi1_init();
	spi1_init();

	gpio_spi2_init();
	spi2_init();
}

uint32_t spi_data_xfer(const uint8_t *src_buf, uint8_t *dst_buf, size_t data_size)
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
