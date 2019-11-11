/**
 * @file	include/usart1.h
 *
 * @brief	Определения функций для работы с устройством USART1.
 *
 * @author	Vasily Yurchenko <vasily.v.yurchenko@yandex.ru>
 */

#include "usart1.h"
#include "main.h"

/** Размер передаваемого буфера в байтах.*/
#define USART1_TX_BUF_SIZE	0x1
/** Размер приемного буфера в байтах.*/
#define USART1_RX_BUF_SIZE	0x1

/** Приемный буфер.*/
static uint8_t usart1_tx_buf[USART1_TX_BUF_SIZE];
/** Передаваемый буфер.*/
static uint8_t usart1_rx_buf[USART1_RX_BUF_SIZE];

void usart1_config(void)
{

	/* Настройка GPIO */

	LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOA);

	LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_9, LL_GPIO_MODE_ALTERNATE);
	LL_GPIO_SetAFPin_0_7(GPIOA, LL_GPIO_PIN_9, LL_GPIO_AF_7);
	LL_GPIO_SetPinSpeed(GPIOA, LL_GPIO_PIN_9, LL_GPIO_SPEED_FREQ_HIGH);
	LL_GPIO_SetPinOutputType(GPIOA, LL_GPIO_PIN_9, LL_GPIO_OUTPUT_PUSHPULL);
	LL_GPIO_SetPinPull(GPIOA, LL_GPIO_PIN_9, LL_GPIO_PULL_UP);

	LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_10, LL_GPIO_MODE_ALTERNATE);
	LL_GPIO_SetAFPin_0_7(GPIOA, LL_GPIO_PIN_10, LL_GPIO_AF_7);
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

	LL_USART_DisableIT_CTS(USART1);

	LL_USART_EnableOverrunDetect(USART1);

	LL_USART_EnableDMADeactOnRxErr(USART1);

	LL_USART_ConfigAsyncMode(USART1);

	while((!(LL_USART_IsActiveFlag_TEACK(USART2))) || (!(LL_USART_IsActiveFlag_REACK(USART2)))) {}

	/* Настройка NVIC */

	NVIC_SetPriority(DMA1_Channel4_IRQn, 0);
	NVIC_EnableIRQ(DMA1_Channel4_IRQn);

	NVIC_SetPriority(DMA1_Channel5_IRQn, 0);
	NVIC_EnableIRQ(DMA1_Channel5_IRQn);

	NVIC_SetPriority(USART1_IRQn, 0);
	NVIC_EnableIRQ(USART1_IRQn);

	/* Настройка DMA */

	LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_DMA1);

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
						 LL_USART_DMA_GetRegAddr(USART2, LL_USART_DMA_REG_DATA_TRANSMIT),
						 LL_DMA_GetDataTransferDirection(DMA1, LL_DMA_CHANNEL_4));

	LL_DMA_SetDataLength(DMA1, LL_DMA_CHANNEL_4, USART1_TX_BUF_SIZE);


	LL_DMA_ConfigTransfer(DMA1, LL_DMA_CHANNEL_5,
						LL_DMA_DIRECTION_PERIPH_TO_MEMORY |
						LL_DMA_PRIORITY_HIGH              |
						LL_DMA_MODE_NORMAL                |
						LL_DMA_PERIPH_NOINCREMENT         |
						LL_DMA_MEMORY_INCREMENT           |
						LL_DMA_PDATAALIGN_BYTE            |
						LL_DMA_MDATAALIGN_BYTE);

	LL_DMA_ConfigAddresses(DMA1, LL_DMA_CHANNEL_5,
						 LL_USART_DMA_GetRegAddr(USART2, LL_USART_DMA_REG_DATA_RECEIVE),
						 (uint32_t)&usart1_rx_buf,
						 LL_DMA_GetDataTransferDirection(DMA1, LL_DMA_CHANNEL_5));

	LL_DMA_SetDataLength(DMA1, LL_DMA_CHANNEL_5, USART1_RX_BUF_SIZE);

	LL_DMA_EnableIT_TC(DMA1, LL_DMA_CHANNEL_4);
	LL_DMA_EnableIT_TE(DMA1, LL_DMA_CHANNEL_4);
	LL_DMA_EnableIT_TC(DMA1, LL_DMA_CHANNEL_5);
	LL_DMA_EnableIT_TE(DMA1, LL_DMA_CHANNEL_5);

	LL_DMA_EnableChannel(DMA1, 4);
	LL_DMA_EnableChannel(DMA1, 5);

	LL_USART_Enable(USART1);

}

int uart1_transmit(uint8_t *data, size_t size, size_t ms_timeout)
{
	uint32_t stop_time = SysTick->VAL + ms_timeout;
	size_t i;
	for (i = 0; i < size; ++i) {
		LL_USART_TransmitData8(USART1, data[i]);
		do {
			if (stop_time < SysTick->VAL)
				return -ERR_TIMEOUT;
		} while (LL_USART_IsActiveFlag_TXE(USART1) != 1);
	}
	return i;
}
