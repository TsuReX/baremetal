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
#include "console.h"
#include "cobs.h"


static void		*usart1_rx_buf;
static size_t	usart1_rx_buf_len;

static void		*usart2_rx_buf;
static size_t	usart2_rx_buf_len;

static void		*usart3_rx_buf;
static size_t	usart3_rx_buf_len;

static void gpio_usart1_init(void)
{
	LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOA);

	/* USART1 TX */
	LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_9, LL_GPIO_MODE_ALTERNATE);
	LL_GPIO_SetPinSpeed(GPIOA, LL_GPIO_PIN_9, LL_GPIO_SPEED_FREQ_HIGH);
	LL_GPIO_SetPinOutputType(GPIOA, LL_GPIO_PIN_9, LL_GPIO_OUTPUT_PUSHPULL);
	LL_GPIO_SetPinPull(GPIOA, LL_GPIO_PIN_9, LL_GPIO_PULL_UP);

	/* USART1 RX */
	LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_10, LL_GPIO_MODE_FLOATING);
	LL_GPIO_SetPinSpeed(GPIOA, LL_GPIO_PIN_10, LL_GPIO_SPEED_FREQ_HIGH);
	LL_GPIO_SetPinPull(GPIOA, LL_GPIO_PIN_10, LL_GPIO_PULL_UP);

}

/*
 * @brief	Настройка портов GPIOA 2/3 для реализации каналов приема и передачи USART2.
 */
static void gpio_usart2_init(void)
{
	LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOA);

	/* USART2 TX */
	LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_2, LL_GPIO_MODE_ALTERNATE);
	LL_GPIO_SetPinSpeed(GPIOA, LL_GPIO_PIN_2, LL_GPIO_SPEED_FREQ_HIGH);
	LL_GPIO_SetPinOutputType(GPIOA, LL_GPIO_PIN_2, LL_GPIO_OUTPUT_PUSHPULL);
	LL_GPIO_SetPinPull(GPIOA, LL_GPIO_PIN_2, LL_GPIO_PULL_UP);

	/* USART2 RX */
	LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_3, LL_GPIO_MODE_FLOATING);
	LL_GPIO_SetPinSpeed(GPIOA, LL_GPIO_PIN_3, LL_GPIO_SPEED_FREQ_HIGH);
	LL_GPIO_SetPinPull(GPIOA, LL_GPIO_PIN_3, LL_GPIO_PULL_UP);

}

/*
 * @brief	Настройка портов GPIOB 10/11 для реализации каналов приема и передачи USART3.
 */
static void gpio_usart3_init(void)
{
	LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOB);

	/* USART3 TX */
	LL_GPIO_SetPinMode(GPIOB, LL_GPIO_PIN_10, LL_GPIO_MODE_ALTERNATE);
	LL_GPIO_SetPinSpeed(GPIOB, LL_GPIO_PIN_10, LL_GPIO_SPEED_FREQ_HIGH);
	LL_GPIO_SetPinOutputType(GPIOB, LL_GPIO_PIN_10, LL_GPIO_OUTPUT_PUSHPULL);
	LL_GPIO_SetPinPull(GPIOB, LL_GPIO_PIN_10, LL_GPIO_PULL_UP);

	/* USART3 RX */
	LL_GPIO_SetPinMode(GPIOB, LL_GPIO_PIN_11, LL_GPIO_MODE_FLOATING);
	LL_GPIO_SetPinSpeed(GPIOB, LL_GPIO_PIN_11, LL_GPIO_SPEED_FREQ_HIGH);
	LL_GPIO_SetPinPull(GPIOB, LL_GPIO_PIN_11, LL_GPIO_PULL_UP);

}

/*
 * @brief Настройка USART1 для работы на скорости 1500000, включение прерываний.
 */
static void usart1_init(void)
{
	LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_USART1);

	LL_USART_ConfigCharacter(USART1, LL_USART_DATAWIDTH_8B, LL_USART_PARITY_NONE, LL_USART_STOPBITS_1);

	LL_USART_SetBaudRate(USART1, HCLK_FREQ >> 1, 1000000);

#if 1
	NVIC_SetPriority(USART1_IRQn, 0);
	NVIC_EnableIRQ(USART1_IRQn);
#endif

	LL_USART_EnableIT_RXNE(USART1);

//	LL_USART_EnableDMAReq_RX(USART1);

//	LL_USART_EnableDirectionTx(USART1);
	LL_USART_EnableDirectionRx(USART1);

	LL_USART_Enable(USART1);
}

/*
 * @brief Настройка USART1 для работы на скорости 1500000, включение прерываний.
 */
static void usart2_init(void)
{
	LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_USART2);

	LL_USART_ConfigCharacter(USART2, LL_USART_DATAWIDTH_8B, LL_USART_PARITY_NONE, LL_USART_STOPBITS_1);

	LL_USART_SetBaudRate(USART2, HCLK_FREQ >> 1, 1500000);

#if 0
	NVIC_SetPriority(USART2_IRQn, 0);
	NVIC_EnableIRQ(USART2_IRQn);
#endif

	LL_USART_EnableDMAReq_RX(USART2);

	LL_USART_EnableDirectionTx(USART2);
	LL_USART_EnableDirectionRx(USART2);

	LL_USART_Enable(USART2);
}

/*
 * @brief Настройка USART1 для работы на скорости 1500000, включение прерываний.
 */
static void usart3_init(void)
{
	LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_USART3);

	LL_USART_ConfigCharacter(USART3, LL_USART_DATAWIDTH_8B, LL_USART_PARITY_NONE, LL_USART_STOPBITS_1);

	LL_USART_SetBaudRate(USART3, HCLK_FREQ >> 1, 1500000);

#if 0
	NVIC_SetPriority(USART3_IRQn, 0);
	NVIC_EnableIRQ(USART3_IRQn);
#endif

	LL_USART_EnableDMAReq_RX(USART3);

	LL_USART_EnableDirectionTx(USART3);
	LL_USART_EnableDirectionRx(USART3);

	LL_USART_Enable(USART3);
}

static void dma_ch5_usart1_rx_init()
{
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
							(uint32_t)&USART1->DR,
							(uint32_t)usart1_rx_buf,
							LL_DMA_DIRECTION_PERIPH_TO_MEMORY);

	LL_DMA_SetDataLength(DMA1, LL_DMA_CHANNEL_5, usart1_rx_buf_len);

	LL_DMA_EnableIT_TC(DMA1, LL_DMA_CHANNEL_5);

	NVIC_SetPriority(DMA1_Channel5_IRQn, 0);
	NVIC_EnableIRQ(DMA1_Channel5_IRQn);
}

static void dma_ch6_usart2_rx_init()
{
	/* Настройка канала приема. */
	LL_DMA_ConfigTransfer(DMA1, LL_DMA_CHANNEL_6,
							LL_DMA_DIRECTION_PERIPH_TO_MEMORY |
							LL_DMA_PRIORITY_HIGH              |
							LL_DMA_MODE_CIRCULAR              |
							LL_DMA_PERIPH_NOINCREMENT         |
							LL_DMA_MEMORY_INCREMENT           |
							LL_DMA_PDATAALIGN_BYTE            |
							LL_DMA_MDATAALIGN_BYTE);

	LL_DMA_ConfigAddresses(DMA1, LL_DMA_CHANNEL_6,
							(uint32_t)&USART2->DR,
							(uint32_t)usart2_rx_buf,
							LL_DMA_DIRECTION_PERIPH_TO_MEMORY);

	LL_DMA_SetDataLength(DMA1, LL_DMA_CHANNEL_6, usart2_rx_buf_len);

	LL_DMA_EnableIT_TC(DMA1, LL_DMA_CHANNEL_6);

	NVIC_SetPriority(DMA1_Channel6_IRQn, 0);
	NVIC_EnableIRQ(DMA1_Channel6_IRQn);
}

static void dma_ch3_usart3_rx_init()
{
	/* Настройка канала приема. */
	LL_DMA_ConfigTransfer(DMA1, LL_DMA_CHANNEL_3,
							LL_DMA_DIRECTION_PERIPH_TO_MEMORY |
							LL_DMA_PRIORITY_HIGH              |
							LL_DMA_MODE_CIRCULAR              |
							LL_DMA_PERIPH_NOINCREMENT         |
							LL_DMA_MEMORY_INCREMENT           |
							LL_DMA_PDATAALIGN_BYTE            |
							LL_DMA_MDATAALIGN_BYTE);

	LL_DMA_ConfigAddresses(DMA1, LL_DMA_CHANNEL_3,
							(uint32_t)&USART3->DR,
							(uint32_t)usart3_rx_buf,
							LL_DMA_DIRECTION_PERIPH_TO_MEMORY);

	LL_DMA_SetDataLength(DMA1, LL_DMA_CHANNEL_3, usart3_rx_buf_len);

	LL_DMA_EnableIT_TC(DMA1, LL_DMA_CHANNEL_3);

	NVIC_SetPriority(DMA1_Channel3_IRQn, 0);
	NVIC_EnableIRQ(DMA1_Channel3_IRQn);
}

void comm_init(void* dst_buffer, size_t dst_buffer_size)
{

	usart1_rx_buf_len = dst_buffer_size;
	usart1_rx_buf = dst_buffer;

	usart2_rx_buf_len = dst_buffer_size;
	usart2_rx_buf = dst_buffer;

	usart2_rx_buf_len = dst_buffer_size;
	usart2_rx_buf = dst_buffer;

	LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_DMA1);

	gpio_usart1_init();
	usart1_init();
	dma_ch5_usart1_rx_init();

	gpio_usart2_init();
	usart2_init();
	dma_ch6_usart2_rx_init();

	gpio_usart3_init();
	usart3_init();
	dma_ch3_usart3_rx_init();
}

void comm_start(void)
{
//	LL_DMA_EnableChannel(DMA1, LL_DMA_CHANNEL_5);

	LL_USART_EnableDirectionRx(USART1);
	LL_USART_EnableDirectionTx(USART1);
}

void comm_stop(void)
{
	/* Включить приемный канал 5 DMA1. */
//	LL_DMA_DisableChannel(DMA1, LL_DMA_CHANNEL_5);

	/* Включить приемник USART1. */
	LL_USART_DisableDirectionRx(USART1);
	LL_USART_DisableDirectionTx(USART1);
}

uint32_t header = 0;
uint32_t tail = 1;
size_t buf_ind = 0;
struct kbms_data kbms;
#define MARKER	0xFF
void usart1_irq_handler(void)
{
	uint8_t data = LL_USART_ReceiveData8(USART1);

	if (header == 0 && data == MARKER) {
		buf_ind = 0;
		((uint8_t*)usart1_rx_buf)[buf_ind++] = data;
		header = 1;
	} else if (header == 1 && data == MARKER && ((uint8_t*)usart1_rx_buf)[buf_ind - 1] != MARKER) {
		tail = 1;
		header = 0;
		((uint8_t*)usart1_rx_buf)[buf_ind] = data;
		/* TODO: Data processing */
		if ((buf_ind + 1) == sizeof(struct kbms_data) + 3) {
			cobs_decode(MARKER, usart1_rx_buf, buf_ind + 1, &kbms);

			if (kbms.hid_num == 1) {
				LL_GPIO_TogglePin(GPIOB, LL_GPIO_PIN_7);
				copy_to_usb((uint8_t*)&kbms.kb_data, _GetEPTxAddr(ENDP1), sizeof(struct keyboard_state));
				_SetEPTxCount(ENDP1, EP1_MAX_PACKET_SIZE);
				_SetEPTxStatus(ENDP1, EP_TX_VALID);

				copy_to_usb((uint8_t*)&kbms.ms_data, _GetEPTxAddr(ENDP2), sizeof(struct mouse_state));
				_SetEPTxCount(ENDP2, EP2_MAX_PACKET_SIZE);
				_SetEPTxStatus(ENDP2, EP_TX_VALID);

			}
		}
	} else if (header == 1 && data != MARKER) {

		((uint8_t*)usart1_rx_buf)[buf_ind++] = data;
	}

	LL_USART_ClearFlag_RXNE(USART1);
}
#if 0
/*
 * USART1 RX handler
 */
void dma1_channel5_irq_handler(void)
{
	struct kbms_data* kbms = (struct kbms_data*)usart1_rx_buf;

	if (kbms->hid_num == 2) {
		LL_GPIO_TogglePin(GPIOB, LL_GPIO_PIN_7);
		copy_to_usb((uint8_t*)&kbms->kb_data, _GetEPTxAddr(ENDP1), sizeof(struct keyboard_state));
		_SetEPTxCount(ENDP1, EP1_MAX_PACKET_SIZE);
		_SetEPTxStatus(ENDP1, EP_TX_VALID);

		copy_to_usb((uint8_t*)&kbms->ms_data, _GetEPTxAddr(ENDP2), sizeof(struct mouse_state));
		_SetEPTxCount(ENDP2, EP2_MAX_PACKET_SIZE);
		_SetEPTxStatus(ENDP2, EP_TX_VALID);

	}

	WRITE_REG(DMA1->IFCR, (DMA_IFCR_CGIF5 | DMA_IFCR_CTCIF5 | DMA_IFCR_CHTIF5 | DMA_IFCR_CTEIF5));
}
#endif

#if 0
/*
 * USART2 RX handler
 */
void dma1_channel6_irq_handler(void)
{
	if (((struct kbms_data*)usart2_rx_buf)->hid_num == 2) {
		copy_to_usb((uint8_t*)&((struct kbms_data*)usart2_rx_buf)->kb_data, _GetEPTxAddr(ENDP1), sizeof(struct keyboard_state));
		_SetEPTxCount(ENDP1, EP1_MAX_PACKET_SIZE);
		_SetEPTxStatus(ENDP1, EP_TX_VALID);

		copy_to_usb((uint8_t*)&((struct kbms_data*)usart2_rx_buf)->ms_data, _GetEPTxAddr(ENDP2), sizeof(struct mouse_state));
		_SetEPTxCount(ENDP2, EP2_MAX_PACKET_SIZE);
		_SetEPTxStatus(ENDP2, EP_TX_VALID);
	}
	WRITE_REG(DMA1->IFCR, (DMA_IFCR_CGIF6 | DMA_IFCR_CTCIF6 | DMA_IFCR_CHTIF6 | DMA_IFCR_CTEIF6));
}
#endif

#if 0
/*
 * USART3 RX handler
 */
void dma1_channel3_irq_handler(void)
{
	if (((struct kbms_data*)usart3_rx_buf)->hid_num == 1) {
		copy_to_usb((uint8_t*)&((struct kbms_data*)usart3_rx_buf)->kb_data, _GetEPTxAddr(ENDP1), sizeof(struct keyboard_state));
		_SetEPTxCount(ENDP1, EP1_MAX_PACKET_SIZE);
		_SetEPTxStatus(ENDP1, EP_TX_VALID);

		copy_to_usb((uint8_t*)&((struct kbms_data*)usart3_rx_buf)->ms_data, _GetEPTxAddr(ENDP2), sizeof(struct mouse_state));
		_SetEPTxCount(ENDP2, EP2_MAX_PACKET_SIZE);
		_SetEPTxStatus(ENDP2, EP_TX_VALID);
	}
	WRITE_REG(DMA1->IFCR, (DMA_IFCR_CGIF3 | DMA_IFCR_CTCIF3 | DMA_IFCR_CHTIF3 | DMA_IFCR_CTEIF3));
}
#endif
