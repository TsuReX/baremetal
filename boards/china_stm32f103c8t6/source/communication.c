/*
 * communication.c
 *
 *  Created on: Sep 8, 2020
 *      Author: user
 */


#include "communication.h"
#include "config.h"

#include "kbmsusb.h"
#include "usb_regs.h"
#include "usb_desc.h"
#include "usb_mem.h"

static size_t	buffer_size;
static void*	buffer;

void comm_init(void* dst_buffer, size_t dst_buffer_size)
{

	buffer_size = dst_buffer_size;
	buffer = dst_buffer;

	LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOA);

/*	LL_GPIO_SetPinMode(GPIOB, LL_GPIO_PIN_7, LL_GPIO_MODE_ALTERNATE);
	LL_GPIO_SetPinSpeed(GPIOB, LL_GPIO_PIN_7, LL_GPIO_SPEED_FREQ_HIGH);
	LL_GPIO_SetPinOutputType(GPIOB, LL_GPIO_PIN_7, LL_GPIO_OUTPUT_PUSHPULL);
	LL_GPIO_SetPinPull(GPIOB, LL_GPIO_PIN_7, LL_GPIO_PULL_UP);

	LL_GPIO_SetPinMode(GPIOB, LL_GPIO_PIN_6, LL_GPIO_MODE_ALTERNATE);
	LL_GPIO_SetPinSpeed(GPIOB, LL_GPIO_PIN_6, LL_GPIO_SPEED_FREQ_HIGH);
	LL_GPIO_SetPinOutputType(GPIOB, LL_GPIO_PIN_6, LL_GPIO_OUTPUT_PUSHPULL);
	LL_GPIO_SetPinPull(GPIOB, LL_GPIO_PIN_6, LL_GPIO_PULL_UP);

	LL_GPIO_AF_EnableRemap_USART1();
*/
	LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_9, LL_GPIO_MODE_ALTERNATE);
	LL_GPIO_SetPinSpeed(GPIOA, LL_GPIO_PIN_9, LL_GPIO_SPEED_FREQ_HIGH);
	LL_GPIO_SetPinOutputType(GPIOA, LL_GPIO_PIN_9, LL_GPIO_OUTPUT_PUSHPULL);
	LL_GPIO_SetPinPull(GPIOA, LL_GPIO_PIN_9, LL_GPIO_PULL_UP);

	LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_10, LL_GPIO_MODE_FLOATING);
	LL_GPIO_SetPinSpeed(GPIOA, LL_GPIO_PIN_10, LL_GPIO_SPEED_FREQ_HIGH);
	LL_GPIO_SetPinPull(GPIOA, LL_GPIO_PIN_10, LL_GPIO_PULL_UP);

	/************************************************************************************************/
	LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_USART1);

	LL_USART_ConfigCharacter(USART1, LL_USART_DATAWIDTH_8B, LL_USART_PARITY_NONE, LL_USART_STOPBITS_1);

	LL_USART_SetBaudRate(USART1, HCLK_FREQ >> 1, /*115200*8*/1500000);

	/*LL_USART_EnableOverrunDetect(USART1);*/
	/*LL_USART_EnableDMADeactOnRxErr(USART1);*/
	LL_USART_EnableDMAReq_RX(USART1);
	/*LL_USART_EnableDMAReq_TX(USART1);*/
	/*LL_USART_EnableIT_RXNE(USART1);*/

	NVIC_SetPriority(USART1_IRQn, 0);
	NVIC_EnableIRQ(USART1_IRQn);

//	LL_USART_EnableDirectionTx(USART1);

	LL_USART_Enable(USART1);

	/************************************************************************************************/
	LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_DMA1);

	/* Настройка канала приема. */
	LL_DMA_ConfigTransfer(DMA1, LL_DMA_CHANNEL_5,
							LL_DMA_DIRECTION_PERIPH_TO_MEMORY |
							LL_DMA_PRIORITY_HIGH              |
							LL_DMA_MODE_CIRCULAR              |
							LL_DMA_PERIPH_NOINCREMENT         |
							LL_DMA_MEMORY_INCREMENT           |
							LL_DMA_PDATAALIGN_BYTE            |
							LL_DMA_MDATAALIGN_BYTE);

	LL_DMA_ConfigAddresses(DMA1, LL_DMA_CHANNEL_5,
							LL_USART_DMA_GetRegAddr(USART1),
							(uint32_t)dst_buffer,
							LL_DMA_GetDataTransferDirection(DMA1, LL_DMA_CHANNEL_5));

	LL_DMA_SetDataLength(DMA1, LL_DMA_CHANNEL_5, dst_buffer_size);

	/** Генерация прерывания по приему половины буфера. */
	/*LL_DMA_EnableIT_HT(DMA1, LL_DMA_CHANNEL_5);*/

	/** Генерация прерывания по приему всего буфера. */
	LL_DMA_EnableIT_TC(DMA1, LL_DMA_CHANNEL_5);

	/* Генерация прерывания по ошибке во время приема. */
	/*LL_DMA_EnableIT_TE(DMA1, LL_DMA_CHANNEL_5);*/

	NVIC_SetPriority(DMA1_Channel5_IRQn, 0);
	NVIC_EnableIRQ(DMA1_Channel5_IRQn);
}

void comm_start(void)
{
	LL_DMA_EnableChannel(DMA1, LL_DMA_CHANNEL_5);

	LL_USART_EnableDirectionRx(USART1);
	LL_USART_EnableDirectionTx(USART1);
}

void comm_stop(void)
{
	/* Включить приемный канал 5 DMA1. */
	LL_DMA_DisableChannel(DMA1, LL_DMA_CHANNEL_5);

	/* Включить приемник USART1. */
	LL_USART_DisableDirectionRx(USART1);
	LL_USART_DisableDirectionTx(USART1);
}

void dma1_channel5_irq_handler(void)
{

	copy_to_usb((uint8_t*)&((struct kbms_data*)buffer)->kb_data, _GetEPTxAddr(ENDP1), sizeof(struct keyboard_state));
	_SetEPTxCount(ENDP1, EP1_MAX_PACKET_SIZE);
	_SetEPTxStatus(ENDP1, EP_TX_VALID);

	copy_to_usb((uint8_t*)&((struct kbms_data*)buffer)->ms_data, _GetEPTxAddr(ENDP2), sizeof(struct mouse_state));
	_SetEPTxCount(ENDP2, EP2_MAX_PACKET_SIZE);
	_SetEPTxStatus(ENDP2, EP_TX_VALID);

//	uint32_t	ms_timeout = 10000;
//	size_t		i;
//
//	for (i = 0; i < buffer_size; ++i) {
//		LL_USART_TransmitData8(USART1, ((uint8_t*)buffer)[i]);
//
//		do {
////			LL_mDelay(1);
//			--ms_timeout;
//			if (ms_timeout == 0)
//				/* TODO: Рассмотреть возможные варианты действий в случае превышения таймаута. */
//				break;
//		} while (LL_USART_IsActiveFlag_TXE(USART1) != 1);
//	}

	WRITE_REG(DMA1->IFCR, (DMA_IFCR_CGIF5 | DMA_IFCR_CTCIF5 | DMA_IFCR_CHTIF5 | DMA_IFCR_CTEIF5));
}
