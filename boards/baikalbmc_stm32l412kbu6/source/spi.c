/*
 * spi.c
 *
 *  Created on: Sep 8, 2020
 *      Author: user
 */

#include "spi.h"
#include "console.h"
#include "drivers.h"
#include "time.h"

uint8_t dummy_tx = 0x0;

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
//
//	NVIC_SetPriority(SPI1_IRQn, 0);
//	NVIC_EnableIRQ(SPI1_IRQn);

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

static void spi_dma_tx_init()
{
	LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_DMA1);

//	/* Настройка канала передачи. */
//	LL_DMA_ConfigTransfer(DMA1, LL_DMA_CHANNEL_3,
//							LL_DMA_DIRECTION_MEMORY_TO_PERIPH |
//							LL_DMA_PRIORITY_HIGH              |
//							LL_DMA_MODE_NORMAL	              |
//							LL_DMA_PERIPH_NOINCREMENT         |
//							LL_DMA_MEMORY_INCREMENT           |
//							LL_DMA_PDATAALIGN_BYTE            |
//							LL_DMA_MDATAALIGN_BYTE);
//
//	LL_SPI_EnableDMAReq_TX(SPI1);
}

static void spi_dma_rx_init()
{
	LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_DMA1);

//	/* Настройка канала приема. */
//	LL_DMA_ConfigTransfer(DMA1, LL_DMA_CHANNEL_2,
//							LL_DMA_DIRECTION_PERIPH_TO_MEMORY |
//							LL_DMA_PRIORITY_HIGH              |
//							LL_DMA_MODE_NORMAL	              |
//							LL_DMA_PERIPH_NOINCREMENT         |
//							LL_DMA_MEMORY_INCREMENT           |
//							LL_DMA_PDATAALIGN_BYTE            |
//							LL_DMA_MDATAALIGN_BYTE);
//
//	LL_SPI_EnableDMAReq_RX(SPI1);
}

static void spi_rx_trans_prepare(const void *rx_buf, size_t rx_buf_size)
{
	LL_SPI_ReceiveData8(SPI1);

	/* Настройка канала приема. */
	LL_DMA_ConfigTransfer(DMA1, LL_DMA_CHANNEL_2,
							LL_DMA_DIRECTION_PERIPH_TO_MEMORY |
							LL_DMA_PRIORITY_HIGH              |
							LL_DMA_MODE_NORMAL	              |
							LL_DMA_PERIPH_NOINCREMENT         |
							LL_DMA_MEMORY_INCREMENT           |
							LL_DMA_PDATAALIGN_BYTE            |
							LL_DMA_MDATAALIGN_BYTE);

	LL_DMA_ConfigAddresses(DMA1, LL_DMA_CHANNEL_2,
							(uint32_t) &(SPI1->DR),
							(uint32_t)rx_buf,
							LL_DMA_DIRECTION_PERIPH_TO_MEMORY);

	LL_DMA_SetDataLength(DMA1, LL_DMA_CHANNEL_2, rx_buf_size);
	LL_SPI_EnableDMAReq_RX(SPI1);

	/* Настройка канала передачи. */
	LL_DMA_ConfigTransfer(DMA1, LL_DMA_CHANNEL_3,
							LL_DMA_DIRECTION_MEMORY_TO_PERIPH 	|
							LL_DMA_PRIORITY_HIGH              	|
							LL_DMA_MODE_NORMAL					|
							LL_DMA_PERIPH_NOINCREMENT			|
							LL_DMA_MEMORY_NOINCREMENT			|
							LL_DMA_PDATAALIGN_BYTE            |
							LL_DMA_MDATAALIGN_BYTE);

	LL_DMA_ConfigAddresses(DMA1, LL_DMA_CHANNEL_3,
							(uint32_t)&dummy_tx,
							(uint32_t) &(SPI1->DR),
							LL_DMA_DIRECTION_MEMORY_TO_PERIPH);

	LL_DMA_SetDataLength(DMA1, LL_DMA_CHANNEL_3, rx_buf_size);
	LL_SPI_EnableDMAReq_TX(SPI1);
}

static void spi_tx_trans_prepare(const void *tx_buf, size_t tx_buf_size)
{
	/* Настройка канала передачи. */
	LL_DMA_ConfigTransfer(DMA1, LL_DMA_CHANNEL_3,
							LL_DMA_DIRECTION_MEMORY_TO_PERIPH |
							LL_DMA_PRIORITY_HIGH              |
							LL_DMA_MODE_NORMAL	              |
							LL_DMA_PERIPH_NOINCREMENT         |
							LL_DMA_MEMORY_INCREMENT           |
							LL_DMA_PDATAALIGN_BYTE            |
							LL_DMA_MDATAALIGN_BYTE);

	LL_DMA_ConfigAddresses(DMA1, LL_DMA_CHANNEL_3,
							(uint32_t)tx_buf,
							(uint32_t) &(SPI1->DR),
							LL_DMA_DIRECTION_MEMORY_TO_PERIPH);

	LL_DMA_SetDataLength(DMA1, LL_DMA_CHANNEL_3, tx_buf_size);
	LL_SPI_EnableDMAReq_TX(SPI1);
}

static void spi_dma_rx_start()
{
	/* Важна последовательность включения. */
	/* Включить приемный канал 2 DMA1. */
	LL_DMA_EnableChannel(DMA1, LL_DMA_CHANNEL_2);
	/* Включить передающий канал 3 DMA1. */
	LL_DMA_EnableChannel(DMA1, LL_DMA_CHANNEL_3);
}

static void spi_dma_rx_stop()
{
	/* Выключить приемный канал 2 DMA1. */
	LL_DMA_DisableChannel(DMA1, LL_DMA_CHANNEL_2);
	/* Выключить передающий канал 3 DMA1. */
	LL_DMA_DisableChannel(DMA1, LL_DMA_CHANNEL_3);
}

static void spi_dma_tx_start()
{
	/* Включить передающий канал 3 DMA1. */
	LL_DMA_EnableChannel(DMA1, LL_DMA_CHANNEL_3);
}

static void spi_dma_tx_stop()
{
	/* Выключить приемный канал 3 DMA1. */
	LL_DMA_DisableChannel(DMA1, LL_DMA_CHANNEL_3);
}

static uint32_t spi_dma_rx_compl_wait(uint32_t msec_timeout)
{
	do {
		if (LL_DMA_IsActiveFlag_TC2(DMA1) != 0) {
			LL_DMA_ClearFlag_TC2(DMA1);
			return 1;
		}
		mdelay(1);
	} while (msec_timeout-- != 0);

	return 0;
}

static uint32_t spi_dma_tx_compl_wait(uint32_t msec_timeout)
{
	do {
		if (LL_DMA_IsActiveFlag_TC3(DMA1) != 0) {
			LL_DMA_ClearFlag_TC3(DMA1);
			return 1;
		}
		mdelay(1);
	} while (msec_timeout-- != 0);

	return 0;
}

uint32_t spi_dma_data_send(const uint8_t *src_buf, size_t data_size)
{
	spi_tx_trans_prepare(src_buf, data_size);
	spi_dma_tx_start();
	uint32_t ret_val = spi_dma_tx_compl_wait(1000);
	spi_dma_tx_stop();

	return ret_val;
}

uint32_t spi_dma_data_recv(uint8_t *dst_buf, size_t data_size)
{
	spi_rx_trans_prepare(dst_buf, data_size);
	spi_dma_rx_start();
	uint32_t ret_val = spi_dma_rx_compl_wait(1000);
	spi_dma_rx_stop();

	return ret_val;
}

void spi_init(void)
{
	gpio_spi1_init();
	spi1_init();

	gpio_spi2_init();
	spi2_init();

	spi_dma_rx_init();
	spi_dma_tx_init();
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
