/**
 * @file	src/console.c
 *
 * @brief	Определены функции определяющие поведение текстовой консоли
 * 			и настройки необходимых для ее работы аппаратных посистем (USART1 и DMA4-5).
 *
 * @author	Vasily Yurchenko <vasily.v.yurchenko@yandex.ru>
 */

#include "console.h"

#include <stdarg.h>
#include <stdio.h>

#include "drivers.h"
#include "ringbuf.h"

/* Содержащаяся в данном файле настройка и вариант использования USART1 будут специфичны для текстовой консоли.
 * Настройки и вариант использования USART1 для spi-manager будут иными и будут располагаться в другом файле.
 * USART1 может быть использован только либо для текстовой консоли, либо для spi-manager.*/

/** Количество байтов передаваемых через USART1. */
#define SIZE_TO_TRANSMIT	32*2
/** Количество миллисекунд,
 * за которое должна завершиться передача байта даных через USART1. */
#define TRANSMIT_TIMEOUT	20

/** Размер передаваемого буфера в байтах.*/
#define USART1_TX_BUF_SIZE	0x1
/** Размер приемного буфера в байтах.*/
#define USART1_RX_BUF_SIZE	0x1

/** Приемный буфер.*/
//static uint8_t usart1_tx_buf[USART1_TX_BUF_SIZE];
/** Передаваемый буфер.*/
//static uint8_t usart1_rx_buf[USART1_RX_BUF_SIZE];

/** Кольцевой буфер приема данных через USART 1. */
static struct ring_buf rx_rb;
/** Кольцевой буфер передачи данных через USART 1. */
static struct ring_buf tx_rb;

/*
 * @brief	Настройка портов GPIO 9/10 для реализации каналов приема и передачи USART1.
 */
static void console_gpio_init(void)
{
	LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOA);

	LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_9, LL_GPIO_MODE_ALTERNATE);
	LL_GPIO_SetAFPin_8_15(GPIOA, LL_GPIO_PIN_9, LL_GPIO_AF_1);
	LL_GPIO_SetPinSpeed(GPIOA, LL_GPIO_PIN_9, LL_GPIO_SPEED_FREQ_HIGH);
	LL_GPIO_SetPinOutputType(GPIOA, LL_GPIO_PIN_9, LL_GPIO_OUTPUT_PUSHPULL);
	LL_GPIO_SetPinPull(GPIOA, LL_GPIO_PIN_9, LL_GPIO_PULL_UP);

	LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_10, LL_GPIO_MODE_ALTERNATE);
	LL_GPIO_SetAFPin_8_15(GPIOA, LL_GPIO_PIN_10, LL_GPIO_AF_1);
	LL_GPIO_SetPinSpeed(GPIOA, LL_GPIO_PIN_10, LL_GPIO_SPEED_FREQ_HIGH);
	LL_GPIO_SetPinOutputType(GPIOA, LL_GPIO_PIN_10, LL_GPIO_OUTPUT_PUSHPULL);
	LL_GPIO_SetPinPull(GPIOA, LL_GPIO_PIN_10, LL_GPIO_PULL_UP);
}

/*
 * @brief Настройка USART1 для работы на скорости 115200, включение прерываний.
 */
static void console_usart1_init(void)
{
	LL_APB1_GRP2_EnableClock(LL_APB1_GRP2_PERIPH_USART1);

	LL_USART_ConfigCharacter(USART1, LL_USART_DATAWIDTH_8B, LL_USART_PARITY_NONE, LL_USART_STOPBITS_1);
	LL_USART_SetHWFlowCtrl(USART1, LL_USART_HWCONTROL_NONE);
	LL_USART_SetOverSampling(USART1, LL_USART_OVERSAMPLING_16);

	LL_RCC_SetUSARTClockSource(LL_RCC_USART1_CLKSOURCE_PCLK1);
	LL_USART_SetBaudRate(USART1, LL_RCC_GetUSARTClockFreq(LL_RCC_USART1_CLKSOURCE), LL_USART_OVERSAMPLING_16, 1500000);

	/*LL_USART_EnableOverrunDetect(USART1);*/
	/*LL_USART_EnableDMADeactOnRxErr(USART1);*/
	LL_USART_EnableDMAReq_RX(USART1);
	/*LL_USART_EnableDMAReq_TX(USART1);*/
	/*LL_USART_EnableIT_RXNE(USART1);*/

	NVIC_SetPriority(USART1_IRQn, 0);
	NVIC_EnableIRQ(USART1_IRQn);

	LL_USART_EnableDirectionTx(USART1);

	LL_USART_Enable(USART1);
}

/*
 * @brief Отключение USART1.
 */
static void console_usart1_close(void)
{
	LL_USART_DisableDirectionRx(USART1);
	LL_USART_DisableDirectionTx(USART1);

	/*LL_USART_DisableOverrunDetect(USART1);*/
	/*LL_USART_DisableDMADeactOnRxErr(USART1);*/
	LL_USART_DisableDMAReq_RX(USART1);
	LL_USART_DisableDMAReq_TX(USART1);
	/*LL_USART_DisableIT_RXNE(USART1);*/

	/*NVIC_SetPriority(USART1_IRQn, 0);
	NVIC_DisableIRQ(USART1_IRQn);*/

	LL_USART_Disable(USART1);
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
	/* TODO: Испытать синхронную передачу данных через USART1
	 * самим процессором без использования DMA!!!
	 * При необходимости реализовать асинхронную передачу. */

	uint8_t		data_buf[SIZE_TO_TRANSMIT];
	uint32_t	ms_timeout = TRANSMIT_TIMEOUT*10;

	/** Пердавать данные, пока кольцевой буфер не пуст. */
	while (rb_get_data_size(&tx_rb) != 0) {

		/** Передавать блоками по SIZE_TO_TRANSMIT либо оставшемуся количеству байтов. */
		size_t data_size = rb_get_data(&tx_rb, data_buf, SIZE_TO_TRANSMIT);

		size_t i;
		/** Передавать синхронно по байту с таймаутом. */
		for (i = 0; i < data_size; ++i) {
			LL_USART_TransmitData8(USART1, data_buf[i]);

			/** Ожидать окончания передачи ms_timeout миллисекунд.*/
			do {
//				LL_mDelay(1);
				--ms_timeout;
				if (ms_timeout == 0)
					/* TODO: Рассмотреть возможные варианты действий в случае превышения таймаута. */
					break;
			} while (LL_USART_IsActiveFlag_TXE(USART1) != 1);
		}
	}
}

/*
 * @brief	Выключает канал 4 DMA1 и передатчик USART1.
 * 			Выключение может происходить идним из нескольких способов:
 * 			синхронным и асинхронным.
 * 			В данный момент происходит синхронное принудительное отключение.
 */
//static void console_stop_transmission()
//{
//	/* TODO: Реализовать функцию после того, как будет решено,
//	 * какой вариант функции console_start_transmission (синхронный или асинхронный)
//	 * будет использоваться.
//	 * В случае синхронной передачи в данной функции не будет необходимости. */
//
//	/* Выключить передающий канал 4 DMA1. */
//	/* LL_DMA_DisableChannel(DMA1, LL_DMA_CHANNEL_4); */
//
//	/* TODO: Изучить вопрос выключения канала TX USART,
//	 * интересует момент выключения в момент передачи.
//	 * Вероятно, необходимо дождаться завершения.
//	 * Реализаций может быть две:
//	 * - синхронная - ожидать в цикле (может привести к бесконечному ожиданию),
//	 * либо же отключить принудительно, что может породить ошибки.
//	 * - асинхронная - установить флаг необходимости завершения,
//	 * а в обработчике окончания передачи (он должен быть включен) выключить канал.
//	 * Реализовать. */
//
//	/* Выключить передатчик USART1. */
//	/* LL_USART_DisableDirectionTx(USART1); */
//}

/*
 * @brief	Включает канал 5 DMA1 и приемник USART 1 для приема данных по готовности.
 */
//static void console_start_reception()
//{
//	/* Включить приемный канал 5 DMA1. */
//	LL_DMA_EnableChannel(DMA1, LL_DMA_CHANNEL_5);
//
//	/* Включить приемник USART1. */
//	LL_USART_EnableDirectionRx(USART1);
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
	LL_USART_DisableDirectionRx(USART1);

	/* Включить приемный канал 5 DMA1. */
	LL_DMA_DisableChannel(DMA1, LL_DMA_CHANNEL_5);
}

/*
 * @brief	Настройка канала 4 DMA1 на передачу, включение прерываний.
 * 			Генерация прерывайни осуществляется по событиям:
 * 				передана половина объема буфера данных; (Пока не реализовано)
 *				передан весь буфер; (Пока не реализовано)
 *				возникла ошибка передачи. (Пока не реализовано)
 *
 * @param[in]	tx_buf			буфер содержащий данные для передачи;
 *
 * @param[in]	tx_buf_size		объем передаваемых данных в байтах.
 */
//static void console_dma1_ch4_init(void *tx_buf, size_t tx_buf_size)
//{
//	LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_DMA1);
//
//	/* Настройка канала передачи. */
//
//	LL_DMA_ConfigTransfer(DMA1, LL_DMA_CHANNEL_4,
//							LL_DMA_DIRECTION_MEMORY_TO_PERIPH |
//							LL_DMA_PRIORITY_HIGH              |
//							LL_DMA_MODE_NORMAL                |
//							LL_DMA_PERIPH_NOINCREMENT         |
//							LL_DMA_MEMORY_NOINCREMENT           |
//							LL_DMA_PDATAALIGN_BYTE            |
//							LL_DMA_MDATAALIGN_BYTE);
//
//	LL_DMA_ConfigAddresses(DMA1, LL_DMA_CHANNEL_4,
//							(uint32_t)tx_buf,
//							LL_USART_DMA_GetRegAddr(USART1, LL_USART_DMA_REG_DATA_TRANSMIT),
//							LL_DMA_GetDataTransferDirection(DMA1, LL_DMA_CHANNEL_4));
//
//	LL_DMA_SetDataLength(DMA1, LL_DMA_CHANNEL_4, tx_buf_size);
//
//	/* На данный момент нет необходимости в генерации прерываний по передаче данных. */
//
//	/** Генерация прерывания по передаче половины буфера. */
//	/*LL_DMA_EnableIT_HT(DMA1, LL_DMA_CHANNEL_4);*/
//
//	/** Генерация прерывания по передаче всего буфера. */
//	/*LL_DMA_EnableIT_TC(DMA1, LL_DMA_CHANNEL_4);*/
//
//	/* Генерация прерывания по ошибке во время передачи. */
//	/*LL_DMA_EnableIT_TE(DMA1, LL_DMA_CHANNEL_4);*/
//
//	/*NVIC_SetPriority(DMA1_Channel4_IRQn, 0);
//	NVIC_EnableIRQ(DMA1_Channel4_IRQn);
//	*/
//}

/*
 * @brief	Настройка канала 5 DMA1 на прием, включение прерываний
 * 			Генерация прерывайни осуществляется по событиям:
 * 				передана половина объема буфера данных; (Пока не реализовано)
 *				передан весь буфер;
 *				возникла ошибка передачи. (Пока не реализовано)
 *
 * @param[in]	rx_buf			буфер, в который будут помещены принятые данные;
 *
 * @param[in]	rx_buf_size		ожидаемый объем принимаемых данных.
 */
//static void console_dma1_ch5_init(void *rx_buf, size_t rx_buf_size)
//{
//	LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_DMA1);
//
//	/* Настройка канала приема. */
//	LL_DMA_ConfigTransfer(DMA1, LL_DMA_CHANNEL_5,
//							LL_DMA_DIRECTION_PERIPH_TO_MEMORY |
//							LL_DMA_PRIORITY_HIGH              |
//							LL_DMA_MODE_CIRCULAR              |
//							LL_DMA_PERIPH_NOINCREMENT         |
//							LL_DMA_MEMORY_NOINCREMENT           |
//							LL_DMA_PDATAALIGN_BYTE            |
//							LL_DMA_MDATAALIGN_BYTE);
//
//	LL_DMA_ConfigAddresses(DMA1, LL_DMA_CHANNEL_5,
//							LL_USART_DMA_GetRegAddr(USART1, LL_USART_DMA_REG_DATA_RECEIVE),
//							(uint32_t)rx_buf,
//							LL_DMA_GetDataTransferDirection(DMA1, LL_DMA_CHANNEL_5));
//
//	LL_DMA_SetDataLength(DMA1, LL_DMA_CHANNEL_5, rx_buf_size);
//
//	/** Генерация прерывания по приему половины буфера. */
//	/*LL_DMA_EnableIT_HT(DMA1, LL_DMA_CHANNEL_5);*/
//
//	/** Генерация прерывания по приему всего буфера. */
//	LL_DMA_EnableIT_TC(DMA1, LL_DMA_CHANNEL_5);
//
//	/* Генерация прерывания по ошибке во время приема. */
//	/*LL_DMA_EnableIT_TE(DMA1, LL_DMA_CHANNEL_5);*/
//
//	NVIC_SetPriority(DMA1_Channel5_IRQn, 0);
//	NVIC_EnableIRQ(DMA1_Channel5_IRQn);
//}

/*
 *	Настройка USART 1.
 *	Для работы USART 1 настраиваются AHB1, GPIO9/10, USART1, DMA4/5, NVIC.
 */
void console_init(void)
{
	/* Настройка GPIO9/10. */
	console_gpio_init();

	/* Настройка USART1. */
	console_usart1_init();

	/* Настройка и включение приемного канала 5 DMA1. */
//	console_dma1_ch5_init(usart1_rx_buf, 1);

	/* Настройка кольцевых буферов */
	rb_init_ring_buffer(&rx_rb);
	rb_init_ring_buffer(&tx_rb);

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

	console_usart1_close();

	/* TODO: Рассмотреть необходимость отключения DMA. */

	/* TODO: Рассмотреть необходимость отключения GPIO. */
}

int32_t console_process(void)
{
	console_start_transmission();

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

	/*
	 * TODO: необходимо на момент работы данной функции дождаться завершения текущей передачи,
	 * сохранить состояние контроллера USART1, запретить все прерывания, произвести передачу,
	 * а после этого восстановить состояние контроллера USART1.
	 */
	size_t i;
	for (i = 0; i < sz; ++i) {
		LL_USART_TransmitData8(USART1, str[i]);

		/** Ожидать окончания передачи ms_timeout миллисекунд.*/
		do {
			LL_mDelay(1);
			--ms_timeout;
			if (ms_timeout == 0)
				/* TODO: Рассмотреть возможные варианты действий в случае превышения таймаута. */
				break;
		} while (LL_USART_IsActiveFlag_TXE(USART1) != 1);
	}
}

/*
 *	Обработчик прерывания, которое генерируется контроллером DMA 1 канала 4 в следующих случаях:
 *		передана половина объема буфера данных (пока не реализовано);
 *		передан весь буфер (пока не реализовано);
 *		возникла ошибка передачи (пока не реализовано).
 *	Данный канал DMA связан с каналом передачи (TX) USART 1.
 */
void dma1_channel4_irq_handler(void)
{
	/*LL_DMA_IsActiveFlag_GI4(DMA1);
	LL_DMA_IsActiveFlag_TC4(DMA1);
	LL_DMA_IsActiveFlag_HT4(DMA1);
	LL_DMA_IsActiveFlag_TE4(DMA1);*/

	/*uint32_t dma1ch4_if = READ_BIT(DMA1->ISR, (DMA_ISR_GIF4 | DMA_ISR_TCIF4 | DMA_ISR_HTIF4 | DMA_ISR_TEIF4));
	print("%s(): DMA1->ISR 0x%08X\r\n",__func__, dma1ch4_if);*/

	/* На данный момент прерывания по передаче не используются. */

//	WRITE_REG(DMA1->IFCR, (DMA_IFCR_CGIF4 | DMA_IFCR_CTCIF4 | DMA_IFCR_CHTIF4 | DMA_IFCR_CTEIF4));
}

/*
 *	Обработчик прерывания, которое генерируется контроллером DMA 1 канала 5 в следующих случаях:
 *		принята половина указанного объема данных (пока не реализовано);
 *		принят весь объем данных;
 *		возникла ошибка приема (пока не реализовано).
 *	Данный канал DMA связан с каналом приема (RX) USART 1.
 */
void DMA1_Channel5_IRQHandler(void)
{
	/*LL_DMA_IsActiveFlag_GI5(DMA1);
	LL_DMA_IsActiveFlag_TC5(DMA1);
	LL_DMA_IsActiveFlag_HT5(DMA1);
	LL_DMA_IsActiveFlag_TE5(DMA1);*/

	/*uint32_t dma1ch5_if = READ_BIT(DMA1->ISR, (DMA_ISR_GIF5 | DMA_ISR_TCIF5 | DMA_ISR_HTIF5 | DMA_ISR_TEIF5));
	print("%s(): DMA1->ISR 0x%08X\r\n",__func__, dma1ch5_if);*/

	/** Проверять тип прерывания нет необходимости, так как разрешены только прерывания по приему всего буфера. */

//	rb_store_data(&rx_rb, usart1_rx_buf, 1);

//	WRITE_REG(DMA1->IFCR, (DMA_IFCR_CGIF5 | DMA_IFCR_CTCIF5 | DMA_IFCR_CHTIF5 | DMA_IFCR_CTEIF5));
}

/*
 *	Обработчик прерывания, которое генерируется контроллером USART 1 в следующих случаях:
 *		принят байт данных;
 *		принят байт данных при полном буфере.
 */
void USART1_IRQHandler(void)
{
	/* Выбрать все (пока) прерывания.*/
	/*uint32_t usart1_if = READ_BIT(USART1->ISR, 0xFFFFFFFF);
	print("%s(): USART1->ISR 0x%08X\r\n", __func__, usart1_if);*/

	/* На данный момент прерывание не используются. */

	WRITE_REG(USART1->ISR, 0);
}

//void scheduler_process(void)
//{
//	console_process();
//}
