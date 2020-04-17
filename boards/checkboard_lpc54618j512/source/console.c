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

/** Приемный буфер.*/
static uint8_t usart1_tx_buf[USART1_TX_BUF_SIZE];
/** Передаваемый буфер.*/
static uint8_t usart1_rx_buf[USART1_RX_BUF_SIZE];

/** Кольцевой буфер приема данных через USART 1. */
static struct ring_buf rx_rb;
/** Кольцевой буфер передачи данных через USART 1. */
static struct ring_buf tx_rb;

dma_handle_t dma_handle;
/*
 * @brief	Настройка портов GPIO 9/10 для реализации каналов приема и передачи USART1.
 */
static void console_gpio_init(void)
{
    /* Enables the clock for the IOCON block. 0 = Disable; 1 = Enable.: 0x01u */
    CLOCK_EnableClock(kCLOCK_Iocon);

    const uint32_t port1_pin5_config = (IOCON_FUNC1 | IOCON_DIGITAL_EN | IOCON_INPFILT_OFF);
    /* PORT0 PIN29 (coords: B13) is configured as FC0_RXD_SDA_MOSI */
    IOCON_PinMuxSet(IOCON, 1U, 5U, port1_pin5_config);

    const uint32_t port1_pin6_config = (IOCON_FUNC1 | IOCON_DIGITAL_EN | IOCON_INPFILT_OFF);
    /* PORT0 PIN30 (coords: A2) is configured as FC0_TXD_SCL_MISO */
    IOCON_PinMuxSet(IOCON, 1U, 6U, port1_pin6_config);
}

void dma_irq_handler(dma_handle_t *handle, void *param, bool transferDone, uint32_t tcds)
{
    int i;
    if (tcds == kDMA_IntB)
    {
    	d_print("kDMA_IntB\n\r");
    }
    if (tcds == kDMA_IntA)
    {
    	d_print("kDMA_IntA\n\r");
    }
}

/*
 * @brief Настройка USART1 для работы на скорости 115200, включение прерываний.
 */
static void console_usart0_init(void)
{
	usart_config_t config;

    USART_GetDefaultConfig(&config);
    config.baudRate_Bps = 115200;
    config.enableTx     = true;
    config.enableRx     = true;

    USART_Init(USART0, &config, CLOCK_GetFlexCommClkFreq(0));
//    USART_EnableRxDMA(USART0, true);
    USART_EnableTxDMA(USART0, true);

    d_print("DEBUG: USART0 initialized\n\r");

    DMA_Init(DMA0);

    DMA_EnableChannel(DMA0, USART_TX_DMA_CHANNEL);

	DMA_CreateHandle(&dma_handle, DMA0, USART_TX_DMA_CHANNEL);

//	DMA_SetCallback(&dma_handle, dma_irq_handler, NULL);

	d_print("DEBUG: DMA initialized\n\r");
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
	uint8_t dstbuf[64];
	size_t data_size = rb_get_data(&tx_rb, (void*)dstbuf, ARRAY_SIZE(dstbuf));

	if (data_size == 0)
		return;
	__attribute__((aligned(16))) dma_descriptor_t dma_descriptor;
    dma_transfer_config_t dma_transfer_config;
	DMA_PrepareTransfer(&dma_transfer_config, (void*)dstbuf, (void *)&USART0->FIFOWR, 1, data_size,
							kDMA_MemoryToPeripheral, NULL);

	DMA_SubmitTransfer(&dma_handle, &dma_transfer_config);

	dma_transfer_config.xfercfg.intA = true;
	dma_transfer_config.xfercfg.intB = false;
	DMA_CreateDescriptor(&dma_descriptor, &dma_transfer_config.xfercfg, &usart1_tx_buf,
							(void *)&USART0->FIFORD, NULL);

	DMA_StartTransfer(&dma_handle);
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
static void console_start_reception()
{
	/* Включить приемный канал 5 DMA1. */
//	LL_DMA_EnableChannel(DMA1, LL_DMA_CHANNEL_5);

	/* Включить приемник USART1. */
//	LL_USART_EnableDirectionRx(USART1);
}

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
	/* Настройка GPIO9/10. */
	console_gpio_init();

	/* Настройка USART1. */
	console_usart0_init();

	/* Настройка и включение приемного канала 5 DMA1. */
//	console_dma1_ch5_init(usart1_rx_buf, 1);

	/* Настройка кольцевых буферов */
//	rb_init_ring_buffer(&rx_rb);
//	rb_init_ring_buffer(&tx_rb);

//	console_start_reception();

	d_print("DEBUG: Console initialized\n\r");
}

/*
 * @brief	Завершение работы консоли, выключение устройств и освобождение ресурсов.
 */
void console_close(void)
{
	console_stop_reception();

	console_start_transmission();

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
		uint8_t	data_buf[64];
		/** Вывести имеющиеся данные для освобождения буфера. */
		while (rb_get_data_size(&tx_rb) != 0) {
			size_t data_size = rb_get_data(&tx_rb, data_buf, SIZE_TO_TRANSMIT);
			USART_WriteBlocking(USART0, data_buf, data_size);
		}
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
	uint32_t	ms_timeout = TRANSMIT_TIMEOUT;

	va_start(argptr, format);
	sz = vsnprintf(str, 512, format, argptr);
	va_end(argptr);

	if (sz <= 0) {
		return;
	}

	USART_WriteBlocking(USART0, str, sz);
}

