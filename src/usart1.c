/**
 * @file	include/usart1.h
 *
 * @brief	Определения функций для работы с устройством USART1.
 *
 * @author	Vasily Yurchenko <vasily.v.yurchenko@yandex.ru>
 */

#include "usart1.h"
#include "main.h"
#include "console.h"

#include <stdint.h>

/** Размер передаваемого буфера в байтах.*/
#define USART1_TX_BUF_SIZE	0x1
/** Размер приемного буфера в байтах.*/
#define USART1_RX_BUF_SIZE	0x1

/** Приемный буфер.*/
static uint8_t usart1_tx_buf[USART1_TX_BUF_SIZE];
/** Передаваемый буфер.*/
static uint8_t usart1_rx_buf[USART1_RX_BUF_SIZE];

void DMA1_Channel4_IRQHandler(void)
{
	/*LL_DMA_IsActiveFlag_GI4(DMA1);
	LL_DMA_IsActiveFlag_TC4(DMA1);
	LL_DMA_IsActiveFlag_HT4(DMA1);
	LL_DMA_IsActiveFlag_TE4(DMA1);*/

	uint32_t dma1ch4_if = READ_BIT(DMA1->ISR, (DMA_ISR_GIF4 | DMA_ISR_TCIF4 | DMA_ISR_HTIF4 | DMA_ISR_TEIF4));

	print("%s()\r\n", __func__);

	WRITE_REG(DMA1->IFCR, (DMA_IFCR_CGIF4 | DMA_IFCR_CTCIF4 | DMA_IFCR_CHTIF4 | DMA_IFCR_CTEIF4));
}

void DMA1_Channel5_IRQHandler(void)
{
	/*LL_DMA_IsActiveFlag_GI5(DMA1);
	LL_DMA_IsActiveFlag_TC5(DMA1);
	LL_DMA_IsActiveFlag_HT5(DMA1);
	LL_DMA_IsActiveFlag_TE5(DMA1);*/

	uint32_t dma1ch5_if = READ_BIT(DMA1->ISR, (DMA_ISR_GIF5 | DMA_ISR_TCIF5 | DMA_ISR_HTIF5 | DMA_ISR_TEIF5));

	print("%s()\r\n", __func__);

	WRITE_REG(DMA1->IFCR, (DMA_IFCR_CGIF5 | DMA_IFCR_CTCIF5 | DMA_IFCR_CHTIF5 | DMA_IFCR_CTEIF5));
}

void USART1_IRQHandler(void)
{
	/* Выбрать все (пока) прерывания.*/
	uint32_t usart1_if = READ_BIT(USART1->ISR, (0XFFFFFFFF));

	print("%s()\r\n", __func__);

	WRITE_REG(USART1->ICR, (0XFFFFFFFF));
}

void usart1_config(void)
{

	/* Настройка GPIO */

	LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOA);

	LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_9, LL_GPIO_MODE_ALTERNATE);
	LL_GPIO_SetAFPin_8_15(GPIOA, LL_GPIO_PIN_9, LL_GPIO_AF_7);
	LL_GPIO_SetPinSpeed(GPIOA, LL_GPIO_PIN_9, LL_GPIO_SPEED_FREQ_HIGH);
	LL_GPIO_SetPinOutputType(GPIOA, LL_GPIO_PIN_9, LL_GPIO_OUTPUT_PUSHPULL);
	LL_GPIO_SetPinPull(GPIOA, LL_GPIO_PIN_9, LL_GPIO_PULL_UP);

	LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_10, LL_GPIO_MODE_ALTERNATE);
	LL_GPIO_SetAFPin_8_15(GPIOA, LL_GPIO_PIN_10, LL_GPIO_AF_7);
	LL_GPIO_SetPinSpeed(GPIOA, LL_GPIO_PIN_10, LL_GPIO_SPEED_FREQ_HIGH);
	LL_GPIO_SetPinOutputType(GPIOA, LL_GPIO_PIN_10, LL_GPIO_OUTPUT_PUSHPULL);
	LL_GPIO_SetPinPull(GPIOA, LL_GPIO_PIN_10, LL_GPIO_PULL_UP);

	/* Настройка USART1 */

	LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_USART1);

	LL_USART_SetTransferDirection(USART1, LL_USART_DIRECTION_TX_RX);
	LL_USART_ConfigCharacter(USART1, LL_USART_DATAWIDTH_8B, LL_USART_PARITY_NONE, LL_USART_STOPBITS_1);
	LL_USART_SetHWFlowCtrl(USART1, LL_USART_HWCONTROL_NONE);
	LL_USART_SetOverSampling(USART1, LL_USART_OVERSAMPLING_16);

	LL_RCC_SetUSARTClockSource(LL_RCC_USART1_CLKSOURCE_PCLK1);
	LL_USART_SetBaudRate(USART1, LL_RCC_GetUSARTClockFreq(LL_RCC_USART1_CLKSOURCE), LL_USART_OVERSAMPLING_16, 115200);

	LL_USART_EnableOverrunDetect(USART1);
	LL_USART_EnableDMADeactOnRxErr(USART1);
	LL_USART_EnableDMAReq_RX(USART1);
	LL_USART_EnableIT_RXNE(USART1);
	/*LL_USART_EnableDMAReq_TX(USART1);*/

//	while((!(LL_USART_IsActiveFlag_TEACK(USART1))) || (!(LL_USART_IsActiveFlag_REACK(USART1)))) {}

	/* Настройка NVIC */

	NVIC_SetPriority(DMA1_Channel4_IRQn, 0);
	NVIC_EnableIRQ(DMA1_Channel4_IRQn);

	NVIC_SetPriority(DMA1_Channel5_IRQn, 0);
	NVIC_EnableIRQ(DMA1_Channel5_IRQn);

	NVIC_SetPriority(USART1_IRQn, 0);
	NVIC_EnableIRQ(USART1_IRQn);

	/* Настройка DMA. */

	LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_DMA1);

	/* Настройка канала передачи. */

	LL_DMA_ConfigTransfer(DMA1, LL_DMA_CHANNEL_4,
							LL_DMA_DIRECTION_MEMORY_TO_PERIPH |
							LL_DMA_PRIORITY_HIGH              |
							LL_DMA_MODE_NORMAL                |
							LL_DMA_PERIPH_NOINCREMENT         |
							LL_DMA_MEMORY_INCREMENT           |
							LL_DMA_PDATAALIGN_BYTE            |
							LL_DMA_MDATAALIGN_BYTE);

	LL_DMA_ConfigAddresses(DMA1, LL_DMA_CHANNEL_4,
							(uint32_t)&usart1_tx_buf,
							LL_USART_DMA_GetRegAddr(USART1, LL_USART_DMA_REG_DATA_TRANSMIT),
							LL_DMA_GetDataTransferDirection(DMA1, LL_DMA_CHANNEL_4));

	LL_DMA_SetDataLength(DMA1, LL_DMA_CHANNEL_4, USART1_TX_BUF_SIZE);

	/* Настройка канала приема. */

	/* Настраивается циклический режим приема.
	 * После завершения приема канал будет вновь настроен. */
	LL_DMA_ConfigTransfer(DMA1, LL_DMA_CHANNEL_5,
							LL_DMA_DIRECTION_PERIPH_TO_MEMORY |
							LL_DMA_PRIORITY_HIGH              |
							LL_DMA_MODE_CIRCULAR              |
							LL_DMA_PERIPH_NOINCREMENT         |
							LL_DMA_MEMORY_INCREMENT           |
							LL_DMA_PDATAALIGN_BYTE            |
							LL_DMA_MDATAALIGN_BYTE);

	LL_DMA_ConfigAddresses(DMA1, LL_DMA_CHANNEL_5,
							LL_USART_DMA_GetRegAddr(USART1, LL_USART_DMA_REG_DATA_RECEIVE),
							(uint32_t)&usart1_rx_buf,
							LL_DMA_GetDataTransferDirection(DMA1, LL_DMA_CHANNEL_5));

	LL_DMA_SetDataLength(DMA1, LL_DMA_CHANNEL_5, USART1_RX_BUF_SIZE);

	LL_DMA_EnableIT_HT(DMA1, LL_DMA_CHANNEL_4);
	LL_DMA_EnableIT_TC(DMA1, LL_DMA_CHANNEL_4);
	LL_DMA_EnableIT_TE(DMA1, LL_DMA_CHANNEL_4);

	LL_DMA_EnableIT_HT(DMA1, LL_DMA_CHANNEL_5);
	LL_DMA_EnableIT_TC(DMA1, LL_DMA_CHANNEL_5);
	LL_DMA_EnableIT_TE(DMA1, LL_DMA_CHANNEL_5);

	LL_DMA_EnableChannel(DMA1, 4);
	LL_DMA_EnableChannel(DMA1, 5);

	LL_USART_Enable(USART1);

}

int uart1_transmit(uint8_t *data, size_t size, size_t ms_timeout)
{
	size_t i;
	for (i = 0; i < size; ++i) {
		LL_USART_TransmitData8(USART1, data[i]);
		do {
			LL_mDelay(1);
			--ms_timeout;
			if (ms_timeout == 0)
				return -ERR_TIMEOUT;
		} while (LL_USART_IsActiveFlag_TXE(USART1) != 1);
	}
	return i;
}
