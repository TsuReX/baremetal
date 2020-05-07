/**
 * @file	src/console.c
 *
 * @brief	Определены функции определяющие поведение текстовой консоли
 * 			и настройки необходимых для ее работы аппаратных посистем (USART1 и DMA4-5).
 *
 * @author	Vasily Yurchenko <vasily.v.yurchenko@yandex.ru>
 */

#include <stdarg.h>
#include <stdio.h>

#include "ringbuf.h"
#include "console.h"
#include "drivers.h"

/* Содержащаяся в данном файле настройка и вариант использования USART1 будут специфичны для текстовой консоли.
 * Настройки и вариант использования USART1 для spi-manager будут иными и будут располагаться в другом файле.
 * USART1 может быть использован только либо для текстовой консоли, либо для spi-manager.*/

/** Количество байтов передаваемых через USART1. */
#define SIZE_TO_TRANSMIT	32
/** Количество миллисекунд,
 * за которое должна завершиться передача байта даных через USART1. */
#define TRANSMIT_TIMEOUT	20

/** Размер передаваемого буфера в байтах.*/
#define USART1_TX_BUF_SIZE	0x1
/** Размер приемного буфера в байтах.*/
#define USART1_RX_BUF_SIZE	0x1

#define USART_RX_DMA_CHANNEL 0
#define USART_TX_DMA_CHANNEL 1

///** Приемный буфер.*/
//static uint8_t usart1_tx_buf[USART1_TX_BUF_SIZE];
///** Передаваемый буфер.*/
//static uint8_t usart1_rx_buf[USART1_RX_BUF_SIZE];

/** Кольцевой буфер приема данных через USART 1. */
static struct ring_buf rx_rb;
/** Кольцевой буфер передачи данных через USART 1. */
static struct ring_buf tx_rb;

#define RELOAD_DIS	0
#define RELOAD_EN	1

#define CLRTRIG_DIS	0
#define CLRTRIG_EN	1

#define	INTA_DIS	0
#define INTA_EN		1

#define WIDTH_1		1

#define	INTB_DIS	0
#define	INTB_EN		1

#define SRC_INC_1	1

#define DST_INC_0	0

DMA_ALLOCATE_HEAD_DESCRIPTORS(s_dma_descriptor_table0, FSL_FEATURE_DMA_MAX_CHANNELS);
static dma_descriptor_t *s_dma_descriptor_table[] = {s_dma_descriptor_table0};
__attribute__((aligned(16))) static dma_descriptor_t second_dma_descriptor;
static dma_descriptor_t *first_desc;
static dma_descriptor_t *second_desc = &second_dma_descriptor;

/** DMA контроллер занят передачей. */
#define RING_BUF_DMA_BUSY_ERR	1
/** Кольцевой буфер пуст. */
#define RING_BUF_EMPTY_ERR 2

/**
 * @brief	Инициирует передачу данных через связку DMA0-USART0.
 *
 * @param[in]	rb	дескриптор кольцевого буфера
 *
 * @retval	0						передача запущена успешно
 * 			-RING_BUF_DMA_BUSY_ERR	передача не запущена по причине занятого контроллера DMA
 * 			-RING_BUF_EMPTY_ERR		передача не запущена по причине пустого буфера
 *
 */
int32_t rb_dma_usart_transmit_async(struct ring_buf *rb)
{

	if ((DMA0->COMMON[0].ACTIVE & (1 << USART_TX_DMA_CHANNEL)) != 0UL)
		return -RING_BUF_DMA_BUSY_ERR;

//	if ((USART0->STAT & USART_STAT_TXIDLE_MASK) == 0)
//		d_print("Before transmission USART0 NOT EMPTY\n\r");

	DMA0->COMMON[0].ENABLECLR = 1 << USART_TX_DMA_CHANNEL;

	size_t head = rb->array_head;
	size_t tail = rb->array_tail;

	if (tail == head)
		return -RING_BUF_EMPTY_ERR;

	first_desc = &s_dma_descriptor_table[0][1];
//	d_print("before transmission tx_rb.array_tail %d\r\n", tx_rb.array_tail);
//	d_print("before transmission tx_rb.array_head %d\r\n", tx_rb.array_head);
	if (head < tail) {

		uint32_t size = tail - head;
		if (size > 20)
			size -=20;

		first_desc->xfercfg =
				DMA_CHANNEL_XFER(RELOAD_DIS, CLRTRIG_EN, INTA_EN, INTB_DIS, WIDTH_1, SRC_INC_1, DST_INC_0, size);
		first_desc->srcEndAddr = DMA_DESCRIPTOR_END_ADDRESS((uint32_t *)&rb->buffer[head], SRC_INC_1, (size) * WIDTH_1, WIDTH_1);
		first_desc->dstEndAddr = (void *)&USART0->FIFOWR;
		first_desc->linkToNextDesc = 0;

	} else { /* head > tail */

//		if ((USART0->STAT & USART_STAT_TXIDLE_MASK) == 0)
//			d_print("USART0 NOT EMPTY\n\r");
//		d_print("Chain descriptor, head %d, tail %d, size %d\n\r", head, tail, RING_BUF_SIZE - head);
		first_desc->xfercfg =
				DMA_CHANNEL_XFER(RELOAD_EN, CLRTRIG_DIS, INTA_EN, INTB_DIS, WIDTH_1, SRC_INC_1, DST_INC_0, RING_BUF_SIZE - head);
		first_desc->srcEndAddr = DMA_DESCRIPTOR_END_ADDRESS((uint32_t *)&rb->buffer[head], SRC_INC_1, (RING_BUF_SIZE - head) * WIDTH_1, WIDTH_1);
		first_desc->dstEndAddr = (void *)&USART0->FIFOWR;
		first_desc->linkToNextDesc = second_desc;

		second_desc->xfercfg =
				DMA_CHANNEL_XFER(RELOAD_DIS, CLRTRIG_EN, INTA_DIS, INTB_EN, WIDTH_1, SRC_INC_1, DST_INC_0, tail);
		second_desc->srcEndAddr = DMA_DESCRIPTOR_END_ADDRESS((uint32_t *)(rb->buffer), SRC_INC_1, tail * WIDTH_1, WIDTH_1);
		second_desc->dstEndAddr = (void *)&USART0->FIFOWR;
		second_desc->linkToNextDesc = 0;
//		second_desc->xfercfg |= DMA_CHANNEL_XFERCFG_SWTRIG_MASK;

	}

	DMA0->CHANNEL[USART_TX_DMA_CHANNEL].CFG |= DMA_CHANNEL_CFG_PERIPHREQEN_MASK;
	DMA0->CHANNEL[USART_TX_DMA_CHANNEL].XFERCFG = first_desc->xfercfg;
	DMA0->COMMON[0].ENABLESET = 1 << USART_TX_DMA_CHANNEL;
	DMA0->CHANNEL[USART_TX_DMA_CHANNEL].XFERCFG |= DMA_CHANNEL_XFERCFG_SWTRIG_MASK;

	return 0;
}

void DMA0_IRQHandler(void)
{

	if ((DMA0->COMMON[0].INTA & (1 << USART_TX_DMA_CHANNEL)) != 0) {
		rb_throw_last_data(&tx_rb, ((first_desc->xfercfg >> 16) & 0x3FF) + 1);
//		d_print("first_desc exhausted, transfer count %ld\r\n", ((first_desc->xfercfg >> 16) & 0x3FF) + 1);
//		d_print("after transmission tx_rb size %d\n\r", rb_get_data_size(&tx_rb));
//		d_print("after transmission tx_rb.array_tail %d\r\n", tx_rb.array_tail);
//		d_print("after transmission tx_rb.array_head %d\r\n", tx_rb.array_head);

	}

	if ((DMA0->COMMON[0].INTB & (1 << USART_TX_DMA_CHANNEL)) != 0) {
		rb_throw_last_data(&tx_rb, ((second_desc->xfercfg >> 16) & 0x3FF) + 1);
//		d_print("second_desc exhausted, transfer count %ld\r\n", ((second_desc->xfercfg >> 16) & 0x3FF) + 1);
//		d_print("after transmission tx_rb size %d\n\r", rb_get_data_size(&tx_rb));
	}

	if ((DMA0->COMMON[0].ERRINT & (1 << USART_TX_DMA_CHANNEL)) != 0) {
//		d_print("DMA error on channel %d\r\n", (1 << USART_TX_DMA_CHANNEL) - 1);
//		d_print("USART0->STAT %ld\r\n", USART0->STAT);
//		d_print("USART0->FIFOSTAT %ld\r\n", USART0->FIFOSTAT);

	}

	DMA0->COMMON[0].INTA = 0xFFFFFFFF;
	DMA0->COMMON[0].INTB = 0xFFFFFFFF;
	DMA0->COMMON[0].ERRINT = 0xFFFFFFFF;
//	d_print("%s\r\n", __func__);
}


/*
 * @brief Настройка USART1 для работы на скорости 115200, включение прерываний.
 */
static void console_usart0_init(void)
{
	usart_config_t usart0_config;

	USART_GetDefaultConfig(&usart0_config);

	usart0_config.baudRate_Bps = 115200;
    usart0_config.enableTx     = true;

    USART_Init(USART0, &usart0_config, 12000000);

    USART0->FIFOCFG |= USART_FIFOCFG_DMATX_MASK;

    d_print("\033c\033[J");
    d_print("USART0 initialized\n\r");

	SYSCON->AHBCLKCTRLSET[0] = SYSCON_AHBCLKCTRL_DMA(1);
	/* Установить DMA в состояние сброса. */
	SYSCON->PRESETCTRLSET[0] = SYSCON_PRESETCTRL_DMA0_RST(1);
	while (0u == (SYSCON->PRESETCTRL[0] & SYSCON_PRESETCTRL_DMA0_RST(1))) {
	}

	/* Вывести DMA из состояния сброса. */
	SYSCON->PRESETCTRLCLR[0] = SYSCON_PRESETCTRL_DMA0_RST(1);
	while (SYSCON_PRESETCTRL_DMA0_RST(1) == (SYSCON->PRESETCTRL[0] & SYSCON_PRESETCTRL_DMA0_RST(1))) {
	}

	DMA0->SRAMBASE = (uint32_t)s_dma_descriptor_table[0];

	/* enable dma peripheral */
	DMA0->CTRL |= DMA_CTRL_ENABLE(1);

	DMA0->COMMON[0].ENABLESET = 1 << USART_TX_DMA_CHANNEL;

	NVIC_SetPriority(DMA0_IRQn, 16);
	NVIC_EnableIRQ(DMA0_IRQn);
	/* Enable channel interrupt */
	DMA0->COMMON[0].INTENSET = 1 << USART_TX_DMA_CHANNEL;


	d_print("DMA initialized\n\r");
}

/*
 * @brief Отключение USART1.
 */
static void console_usart0_close(void)
{

}

/*
 * @brief	Включает канал 4 DMA1 и передатчик USART1 для начала передачи данных.
 * 			Данные копируются из памяти с помощью канала 4 DMA 1 в буфер передатчика USART1.
 * 			Выполнение передачи происходит асинхронно относительно основной программы и
 * 			на данный момент никак не контролируется успешность выполнения этой операции.
 * 			После завершения
 */
static void console_start_transmission()
{
//	d_print("before transmission tx_rb size %d\n\r", rb_get_data_size(&tx_rb));
	rb_dma_usart_transmit_async(&tx_rb);
//	d_print("rb_dma_usart_transmit_async returned: %ld \n\r", rb_dma_usart_transmit_async(&tx_rb));
}

/*
 * @brief	Выключает канал 4 DMA1 и передатчик USART1.
 * 			Выключение может происходить идним из нескольких способов:
 * 			синхронным и асинхронным.
 * 			В данный момент происходит синхронное принудительное отключение.
 */
static void console_stop_transmission()
{
	/* TODO: Реализовать функцию после того, как будет решено,
	 * какой вариант функции console_start_transmission (синхронный или асинхронный)
	 * будет использоваться.
	 * В случае синхронной передачи в данной функции не будет необходимости. */

	/* Выключить передающий канал 4 DMA1. */
	/* LL_DMA_DisableChannel(DMA1, LL_DMA_CHANNEL_4); */

	/* TODO: Изучить вопрос выключения канала TX USART,
	 * интересует момент выключения в момент передачи.
	 * Вероятно, необходимо дождаться завершения.
	 * Реализаций может быть две:
	 * - синхронная - ожидать в цикле (может привести к бесконечному ожиданию),
	 * либо же отключить принудительно, что может породить ошибки.
	 * - асинхронная - установить флаг необходимости завершения,
	 * а в обработчике окончания передачи (он должен быть включен) выключить канал.
	 * Реализовать. */

	/* Выключить передатчик USART1. */
	/* LL_USART_DisableDirectionTx(USART1); */
}

/*
 * @brief	Включает канал 5 DMA1 и приемник USART 1 для приема данных по готовности.
 */
//static void console_start_reception()
//{
//	/* Включить приемный канал 5 DMA1. */
////	LL_DMA_EnableChannel(DMA1, LL_DMA_CHANNEL_5);
//
//	/* Включить приемник USART1. */
////	LL_USART_EnableDirectionRx(USART1);
//}

/*
 * @brief	Отключает канал 5 DMA1 и приемник USART 1.
 */
static void console_stop_reception()
{
	/* TODO: Проанализировать работу функции и
	 * при необходимости реализовать отключение
	 * передачи с ожиднием флагов завершеня
	 * для избежания взникновения ошибок. */

	/* Включить приемник USART1. */
//	LL_USART_DisableDirectionRx(USART1);

	/* Включить приемный канал 5 DMA1. */
//	LL_DMA_DisableChannel(DMA1, LL_DMA_CHANNEL_5);
}

/*
 *	Настройка USART 1.
 *	Для работы USART 1 настраиваются AHB1, GPIO9/10, USART1, DMA4/5, NVIC.
 */
void console_init(void)
{
	/* Настройка USART1. */
	console_usart0_init();

	/* Настройка кольцевых буферов */
	rb_init_ring_buffer(&rx_rb);
	rb_init_ring_buffer(&tx_rb);

	d_print("Console initialized\n\r");
}

/*
 * @brief	Завершение работы консоли, выключение устройств и освобождение ресурсов.
 */
void console_close(void)
{
	console_stop_reception();

	console_stop_transmission();

	console_usart0_close();

	/* TODO: Рассмотреть необходимость отключения DMA. */

	/* TODO: Рассмотреть необходимость отключения GPIO. */
}
/*
 * @brief	Обрабатывает приемный буфер с символами, передает буфер с символами на передачу.
 * 			Функция должна вызывается с необходимой периодичностью.
 *
 * @retval	TODO: Описать
 */
int32_t console_process(void)
{
//	console_start_transmission();

	/* TODO: Обработать находящиеся в rx_rb принятые данные. */
	/* На данный момент обработка данных заключается в их выдаче - программная закольцовка.
	 * Данные перекладываются из кольцевого буфера приемника в кольцевой буфер передатчика.
	 * После чего следует вывод сообщения о том, что была вызвана функция console_process(). */

	uint8_t	data_buf[64];

	/** Перложить данные из приемного буфера в выходной. */
	while (rb_get_data_size(&rx_rb) != 0) {
		size_t data_size = rb_get_data(&rx_rb, data_buf, SIZE_TO_TRANSMIT);
		rb_store_data(&tx_rb, data_buf, data_size);
	}

	console_start_transmission();

	/* Возвращаемое значение может отражать факт переполнения кольцевого буфера приема. */
	return 0;
}

void print(const char *format, ...)
{
	va_list argptr;
	char	str[512];
	int		sz;

	va_start(argptr, format);
	sz = vsnprintf(str, 512, format, argptr);

	if (sz > rb_get_data_fsize(&tx_rb)) {
		d_print("WARNING! TX ring buffer overflow\r\n");
	}

	if (sz > 0) {
		rb_store_data(&tx_rb, str, sz);
	}
	va_end(argptr);
}

void d_print(const char *format, ...)
{
	va_list 	argptr;
	char		str[512];
	int			sz;

	va_start(argptr, format);
	sz = vsnprintf(str, 512, format, argptr);
	va_end(argptr);

	if (sz <= 0) {
		return;
	}

	USART_WriteBlocking(USART0, (uint8_t *)str, sz);
}

