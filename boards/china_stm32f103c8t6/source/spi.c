/*
 * spi.c
 *
 *  Created on: Sep 8, 2020
 *      Author: user
 */

#include "spi.h"
#include "console.h"

/** Количество байтов идентификатора SPI Flash. */
#define SPI_ID_BYTES		64
/** Количество байтов в одной странице данных SPI Flash. */
#define SPI_PAGE_SIZE		256

/** Код команды чтения идентификатора SPI Flash. */
#define SPI_CMD_RDID		0x9F
/** Код команды включения записи SPI Flash. */
#define SPI_CMD_WREN		0x06
/** Код команды полного стирания SPI Flash. */
#define SPI_CMD_BE		0x60
/** Код команды чтения статусного регистра SPI Flash. */
#define SPI_CMD_RDSR1		0x05
/** Код команды выключения записи SPI Flash. */
#define SPI_CMD_WRDI		0x04
/** Код команды записи одной страницы данных в SPI Flash. */
#define SPI_CMD_PP		0x02
/** Код команды чтения данных SPI Flash. */
#define SPI_CMD_READ		0x03

void spi1_init(void) {

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
	d_print("DEBUG: SPI1 initialized\n\r");
}

void spi1_test(void) {

	uint8_t buffer[SPI_ID_BYTES + 1] = {0,0,0,0};
	size_t i = 0;

	buffer[0] = SPI_CMD_RDID;

	LL_GPIO_ResetOutputPin(GPIOA, LL_GPIO_PIN_4);

	for (i = 0; i < sizeof(buffer) / sizeof(buffer[0]); ++i) {

		while (!LL_SPI_IsActiveFlag_TXE(SPI1));
		LL_SPI_TransmitData8(SPI1, buffer[i]);

		while (!LL_SPI_IsActiveFlag_RXNE(SPI1));
		buffer[i] = LL_SPI_ReceiveData8(SPI1);
	}

	LL_GPIO_SetOutputPin(GPIOA, LL_GPIO_PIN_4);

	d_print("MC_SPI_FLASH_ID (Hex) ");
	for (i = 1; i < sizeof(buffer) / sizeof(buffer[0]); ++i)
		d_print("%02X ", buffer[i]);
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
