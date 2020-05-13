/**
 * @file	src/console.c
 *
 * @brief	Определены функции определяющие поведение текстовой консоли
 * 			и настройки необходимых для ее работы аппаратных посистем (USART1 и DMA4-5).
 *
 * @author	Vasily Yurchenko <vasily.v.yurchenko@yandex.ru>
 */

#include <string.h>
#include <stdarg.h>
#include <stdio.h>

#include <ringbuf.h>
#include <console.h>
#include <drivers.h>
#include <getopt.h>

#define USART_RX_DMA_CHANNEL 0
#define USART_TX_DMA_CHANNEL 1


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


#define CMD_NAME_MAX_LEN	32
//#define CMD_ARG_MAX_LEN	128
#define CMD_ARGS_MAX_COUNT	16
#define CMD_LINE_MAX_LEN	128

/** DMA контроллер занят передачей. */
#define RING_BUF_DMA_BUSY_ERR	1
/** Кольцевой буфер пуст. */
#define RING_BUF_EMPTY_ERR 2

/** Кольцевой буфер приема данных через USART 1. */
static struct ring_buf rx_rb;
/** Кольцевой буфер передачи данных через USART 1. */
static struct ring_buf tx_rb;

DMA_ALLOCATE_HEAD_DESCRIPTORS(s_dma_descriptor_table0, FSL_FEATURE_DMA_MAX_CHANNELS);

static dma_descriptor_t *s_dma_descriptor_table[] = {s_dma_descriptor_table0};
__attribute__((aligned(16))) static dma_descriptor_t second_dma_descriptor;

static dma_descriptor_t *first_desc;
static dma_descriptor_t *second_desc = &second_dma_descriptor;

static size_t chr_pos = 0;
static char cmd_line[CMD_LINE_MAX_LEN];

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
		d_print("DMA error on channel %d\r\n", (1 << USART_TX_DMA_CHANNEL) - 1);
		d_print("USART0->STAT %ld\r\n", USART0->STAT);
		d_print("USART0->FIFOSTAT %ld\r\n", USART0->FIFOSTAT);

	}

	/* Сбросить все запросы на обработку прерываний. */
	DMA0->COMMON[0].INTA = 0xFFFFFFFF;
	DMA0->COMMON[0].INTB = 0xFFFFFFFF;
	DMA0->COMMON[0].ERRINT = 0xFFFFFFFF;

}

void FLEXCOMM0_IRQHandler(void)
{
	uint32_t usart0_fifostat = USART0->FIFOSTAT;

	if ((usart0_fifostat& USART_FIFOSTAT_RXERR(1)) != 0) {
//		d_print("USART0 error on reception\r\n");
//		d_print("USART0->STAT %ld\r\n", USART0->STAT);
//		d_print("USART0->FIFOSTAT %ld\r\n", USART0->FIFOSTAT);
	}

	if ((usart0_fifostat & USART_FIFOSTAT_TXERR(1)) != 0) {
//		d_print("USART0 error on transmitting\r\n");
//		d_print("USART0->STAT %ld\r\n", USART0->STAT);
//		d_print("USART0->FIFOSTAT %ld\r\n", USART0->FIFOSTAT);
	}

	if ((usart0_fifostat & USART_FIFOSTAT_RXNOTEMPTY(1)) != 0) {
		rb_store_data(&rx_rb, (const void*)&USART0->FIFORD, 1);
	}

	/* Сбросить все запросы на прерывания (TXERR и RXERR). */
	USART0->FIFOSTAT = 0xFFFFFFFF;

	__DSB();
}


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
	/* Если DMA на данном канале еще работает, то завершиться с ошибкой. */
	if ((DMA0->COMMON[0].ACTIVE & (1 << USART_TX_DMA_CHANNEL)) != 0UL)
		return -RING_BUF_DMA_BUSY_ERR;

//	if ((USART0->STAT & USART_STAT_TXIDLE_MASK) == 0)
//		d_print("Before transmission USART0 NOT EMPTY\n\r");

	/* Выключить данный канал DMA. */
	DMA0->COMMON[0].ENABLECLR = 1 << USART_TX_DMA_CHANNEL;

	size_t head = rb->array_head;
	size_t tail = rb->array_tail;

	/* Если буфер для передачи пуст, то вернуться с ошибкой. */
	if (tail == head)
		return -RING_BUF_EMPTY_ERR;

	first_desc = &s_dma_descriptor_table[0][1];
//	d_print("before transmission tx_rb.array_tail %d\r\n", tx_rb.array_tail);
//	d_print("before transmission tx_rb.array_head %d\r\n", tx_rb.array_head);
	if (head < tail) {

		/* TODO: Убрать
		 * Данная конструкция используется исключительно в тестовых целях и
		 * служит для предотвращения передачи всего буфера,
		 * это позволяет накопить данные в буфере с перехлестом,
		 * что в свою очередь ведет к использованию цепочечной передаче DMA,
		 * функционирование которой проверяется. */
		uint32_t size = tail - head;
		if (size > 20)
			size -=20;

		first_desc->xfercfg =
				DMA_CHANNEL_XFER(RELOAD_DIS, CLRTRIG_EN, INTA_EN, INTB_DIS, kDMA_Transfer8BitWidth, kDMA_AddressInterleave1xWidth, kDMA_AddressInterleave0xWidth, size);
		first_desc->srcEndAddr =
				DMA_DESCRIPTOR_END_ADDRESS((uint32_t *)&rb->buffer[head], kDMA_AddressInterleave1xWidth,
											(size) * kDMA_Transfer8BitWidth, kDMA_Transfer8BitWidth);
		first_desc->dstEndAddr = (void *)&USART0->FIFOWR;
//		first_desc->linkToNextDesc = 0;

		/* Настройка цепочечной(связанной) передачи при переходе данных через конец буфера. */
	} else { /* head > tail */

		first_desc->xfercfg =
				DMA_CHANNEL_XFER(RELOAD_EN, CLRTRIG_DIS, INTA_EN, INTB_DIS, WIDTH_1, kDMA_AddressInterleave1xWidth, kDMA_AddressInterleave0xWidth, RING_BUF_SIZE - head);
		first_desc->srcEndAddr =
				DMA_DESCRIPTOR_END_ADDRESS((uint32_t *)&rb->buffer[head], kDMA_AddressInterleave1xWidth,
											(RING_BUF_SIZE - head) * kDMA_Transfer8BitWidth, kDMA_Transfer8BitWidth);
		first_desc->dstEndAddr = (void *)&USART0->FIFOWR;
		first_desc->linkToNextDesc = second_desc;

		second_desc->xfercfg =
				DMA_CHANNEL_XFER(RELOAD_DIS, CLRTRIG_EN, INTA_DIS, INTB_EN, kDMA_Transfer8BitWidth, kDMA_AddressInterleave1xWidth, kDMA_AddressInterleave0xWidth, tail);
		second_desc->srcEndAddr =
				DMA_DESCRIPTOR_END_ADDRESS((uint32_t *)(rb->buffer), kDMA_AddressInterleave1xWidth,
											tail * kDMA_Transfer8BitWidth, kDMA_Transfer8BitWidth);
		second_desc->dstEndAddr = (void *)&USART0->FIFOWR;
//		second_desc->linkToNextDesc = 0;

	}
	/* настроить данный канал DMA на взаимодействие с периферийным устройством и указать,
	 * что запросы на разрешение передачи будут поступать от аппаратуры. */
	DMA0->CHANNEL[USART_TX_DMA_CHANNEL].CFG |= DMA_CHANNEL_CFG_PERIPHREQEN_MASK;
	/* Так как конфигурация передачи для нулевого дескриптора размещается не в памяти,
	 * а в регистре данного канала DMA, то устанавливаем нужную конфигурацию. */
	DMA0->CHANNEL[USART_TX_DMA_CHANNEL].XFERCFG = first_desc->xfercfg;
	/* Включаем данный канал DMA. */
	DMA0->COMMON[0].ENABLESET = 1 << USART_TX_DMA_CHANNEL;
	/* Так как даже при наличии аппаратного запроса (HARDWARE REQUEST, не путать с HARDWARE TRIGGER)
	 * передача не запуститься без програмного переключения,
	 * то в регистре конфигурации данного канала активируется соответствующий бит и передача начинается. */
	DMA0->CHANNEL[USART_TX_DMA_CHANNEL].XFERCFG |= DMA_CHANNEL_XFERCFG_SWTRIG_MASK;

	return 0;
}

/*
 * @brief Настройка DMA0.
 */
static void console_dma0_init(void)
{
	/* Включение тактирование блока управления DMA0. */
	SYSCON->AHBCLKCTRLSET[0] = SYSCON_AHBCLKCTRL_DMA(1);

	/* Установить DMA в состояние сброса. */
	SYSCON->PRESETCTRLSET[0] = SYSCON_PRESETCTRL_DMA0_RST(1);
	while (0u == (SYSCON->PRESETCTRL[0] & SYSCON_PRESETCTRL_DMA0_RST(1))) {
	}

	/* Вывести DMA из состояния сброса. */
	SYSCON->PRESETCTRLCLR[0] = SYSCON_PRESETCTRL_DMA0_RST(1);
	while (SYSCON_PRESETCTRL_DMA0_RST(1) == (SYSCON->PRESETCTRL[0] & SYSCON_PRESETCTRL_DMA0_RST(1))) {
	}

	/* Указываем, где размещаетсяв памяти основная таблица дескрипторов всех каналов DMA0.
	 * ВАЖНО! Таблица основных дескрипторов и все дескрипторы цепочечных передач ДОЛЖНы быть выровнены по границе 16 байтов. */
	DMA0->SRAMBASE = (uint32_t)s_dma_descriptor_table[0];

	/* Включение контроллера DMA0. */
	DMA0->CTRL |= DMA_CTRL_ENABLE(1);

//	DMA0->COMMON[0].ENABLESET = 1 << USART_TX_DMA_CHANNEL;

	/* Включение прерываний на данном канале.  */
	DMA0->COMMON[0].INTENSET = 1 << USART_TX_DMA_CHANNEL;

	/* Включение прерываний в NVIC для контроллера DMA0.  */
	NVIC_SetPriority(DMA0_IRQn, DMA0_IRQn);
	NVIC_EnableIRQ(DMA0_IRQn);

}

/*
 * @brief Настройка USART1 для работы на скорости 115200, включение прерываний.
 */
static void console_usart0_init(void)
{

    SYSCON->AHBCLKCTRLSET[1] = SYSCON_AHBCLKCTRL_FLEXCOMM0(1);

	/* Установить FLEXCOMM0 в состояние сброса. */
	SYSCON->PRESETCTRLSET[1] = SYSCON_PRESETCTRL_FC0_RST(1);
	while (0u == (SYSCON->PRESETCTRL[1] & SYSCON_PRESETCTRL_FC0_RST(1))) {
	}

	/* Вывести FLEXCOMM0 из состояния сброса. */
	SYSCON->PRESETCTRLCLR[1] = SYSCON_PRESETCTRL_FC0_RST(1);
	while (SYSCON_PRESETCTRL_FC0_RST(1) == (SYSCON->PRESETCTRL[1] & SYSCON_PRESETCTRL_FC0_RST(1))) {
	}

    /* initialize flexcomm to USART mode */
    FLEXCOMM0->PSELID = FLEXCOMM_PERIPH_USART;

	/* Устанавливаем скорость передачи 115200. */
	USART0->BRG = 7; /* FRO 12Mhz / (BRG + 1) = 1.5M */
	USART0->OSR = 12; /* 1.5M / (OSR + 1) = 115384 (115200) */

	/* Конфигурирование передающего канала TX. */

	/* empty and enable txFIFO */
	USART0->FIFOCFG |= USART_FIFOCFG_EMPTYTX_MASK | USART_FIFOCFG_ENABLETX_MASK;
	/* setup trigger level */
	USART0->FIFOTRIG &= ~(USART_FIFOTRIG_TXLVL_MASK);
	USART0->FIFOTRIG |= USART_FIFOTRIG_TXLVL(kUSART_TxFifo0);
	/* enable trigger interrupt, не используется приработе через DMA */
	USART0->FIFOTRIG |= USART_FIFOTRIG_TXLVLENA_MASK;
	/* Включение приема через DMA.  */
	USART0->FIFOCFG |= USART_FIFOCFG_DMATX(1);

	/* Конфигурирование принимающего канала RX. */

	/* empty and enable rxFIFO */
	USART0->FIFOCFG |= USART_FIFOCFG_EMPTYRX_MASK | USART_FIFOCFG_ENABLERX_MASK;
	/* setup trigger level */
	USART0->FIFOTRIG &= ~(USART_FIFOTRIG_RXLVL_MASK);
	USART0->FIFOTRIG |= USART_FIFOTRIG_RXLVL(kUSART_RxFifo1);
	/* enable trigger interrupt */
	USART0->FIFOTRIG |= USART_FIFOTRIG_RXLVLENA_MASK;


	/* setup configuration and enable USART */
	USART0->CFG =	USART_CFG_PARITYSEL(kUSART_ParityDisabled)
					| USART_CFG_STOPLEN(kUSART_OneStopBit)
					| USART_CFG_DATALEN(kUSART_8BitsPerChar)
					| USART_CFG_LOOP(0)
					| USART_CFG_ENABLE_MASK;

	USART0->FIFOINTENSET = USART_FIFOINTENSET_RXERR(1);
	USART0->FIFOINTENSET = USART_FIFOINTENSET_TXERR(1);
	USART0->FIFOINTENSET = USART_FIFOINTENSET_RXLVL(1);
//	USART0->FIFOINTENSET = USART_FIFOINTENSET_TXLVL(1);

	/* Включение прерываний в NVIC для контроллера DMA0.  */
	NVIC_SetPriority(FLEXCOMM0_IRQn, FLEXCOMM0_IRQn);
	NVIC_EnableIRQ(FLEXCOMM0_IRQn);

}

/*
 *	Настройка USART 0.
 */
void console_init(void)
{
	/* Настройка кольцевых буферов */
	rb_init_ring_buffer(&rx_rb);
	rb_init_ring_buffer(&tx_rb);

	console_usart0_init();

    d_print("\033c\033[J");
    d_print("USART0 initialized\n\r");

    console_dma0_init();

	d_print("DMA initialized\n\r");

	d_print("Console initialized\n\r");
}

struct cmd {
	char cmd_name[CMD_NAME_MAX_LEN];
	int32_t (*handler)(size_t argc, const char **argv);
};

int32_t do_invade(size_t argc, const char **argv)
{
//	print("%s\r\n", __func__);

//	size_t i = 0;
//	for (; i < argc; ++i) {
//		print("argv[%d]: %s\r\n", i, argv[i]);
//	}

	print("The World is ours!\r\n");
	return 0;
}

struct cmd commands_array[] = {
		{"invade", do_invade},
//		{"", 0x0}
};

static uint32_t cmd_name_execute(size_t argc, const char** argv)
{
	size_t cmd_num = 0;

	/* TODO: Реализовать поиск команды в списке команд. */
//	print("%s\r\n", __func__);
	for (; cmd_num < ARRAY_SIZE(commands_array); ++cmd_num) {
		if (strcmp(argv[0],commands_array[cmd_num].cmd_name) == 0)
			return commands_array[cmd_num].handler(argc, argv);
	}
	print("Hmm, Your command not found. Did You mean to conquer the World?\r\n");
	return 0;
}

static uint32_t cmd_line_parse(char* cmd_line, char** argv)
{
//	print("%s\r\n", __func__);

	size_t arg_num = 0;
	char *head = cmd_line;
	char *tail = cmd_line;

	while (*head != 0) {

		if (arg_num == CMD_ARGS_MAX_COUNT)
			return arg_num;

		if (*head == ' ') {
			++head;
			continue;
		}

		tail = head;

		while (*tail != 0){
			if (*tail == ' ') {
				/* ВНИМАНИЕ! Модифицируется cmd_line! */
				*tail = 0;
				argv[arg_num] = head;
//				print("arg_num: %d %s \r\n", arg_num, head);
				++arg_num;
				head = ++tail;
				break;
			}
			++tail;
		}

		if (head == tail)
			continue;

		argv[arg_num] = head;
//		print("arg_num: %d %s \r\n", arg_num, head);
		return ++arg_num;


	}

	return arg_num;
}

static void cmd_line_process(char* cmd_line)
/*static void cmd_line_process(const char* cmd_line)
 * Функция cmd_line_parse может изменять аргумент,
 * поэтому тип cmd_line не может быть константным. */
{

	char *argv[CMD_ARGS_MAX_COUNT];
	size_t argc = 0;

//	print("%s\r\n", __func__);
//	print("Received command: %s, length %d\r\n", cmd_line, strlen(cmd_line));

	argc = cmd_line_parse(cmd_line, argv);

	cmd_name_execute(argc, (const char**)argv);
}

void console_start_transmission()
{
	rb_dma_usart_transmit_async(&tx_rb);
}

/*
 * @brief	Обрабатывает приемный буфер с символами, передает буфер с символами на передачу.
 * 			Функция должна вызывается с необходимой периодичностью.
 *
 * @retval	TODO: Описать
 */
int32_t console_process(void)
{
	uint8_t	chr;

	while (1) {
		/* Приемный буфер пуст. */
		if (rb_get_data_size(&rx_rb) == 0)
			break;

		rb_get_data(&rx_rb, &chr, 1);

		/* Буфер команды заполнен. */
		if (chr_pos == (CMD_LINE_MAX_LEN - 1) && chr != '\r') {
			/* Сбросить cmd_line, chr, chr_pos и rx_rb до конца или до возврата каретки */
			while(rb_get_data_size(&rx_rb) != 0) {
				rb_get_data(&rx_rb, &chr, 1);
				if (chr == '\r')
					break;
			}
			chr_pos = 0;

			print("\r\nCommand length is exceeded!\r\n");
			break;
		}

		/* Получен символ возврата каретки. */
		if (chr == '\r') {
			print("\r\n");
			if (chr_pos != 0)
				cmd_line_process(cmd_line);
			print("Enter command: ");
			chr_pos = 0;
			break;
		}

		/* Получен непечатаемый специальный символ.
		 * В случае приема специального символа стирания backspace (chr == 0x08)
		 * необходимо удалить с экрана поледний(курсор пока всегда на нем стоит) символ строки,
		 * это возможно реализовать удалив последний символ из cmd_line, вернуть каретку в начало
		 * и вывести cmd_line (уже без последнего символа). */
	/*
		if (chr == '\?') {

		}
	*/

		/* Получен печатный символ. Условие не предусматривает обработку сложный составных управляющих символов,
		 * поэтому после отбрасывания первого непечатаемого символа следующие за ним символы выводятся,
		 * так как они обычные печатные и экранируются первым непечатным символом. */
		if ((chr > 31) && (chr < 127)) {

			cmd_line[chr_pos] = chr;
			++chr_pos;
			cmd_line[chr_pos] = 0;

			rb_store_data(&tx_rb, &chr, 1);
		}
	}

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

    /* Check whether txFIFO is enabled */
    if (!(USART0->FIFOCFG & USART_FIFOCFG_ENABLETX_MASK)) {
        return;
    }
    size_t i = 0;
    for (; i < sz; ++i) {
        /* Loop until txFIFO get some space for new data */
        while (!(USART0->FIFOSTAT & USART_FIFOSTAT_TXNOTFULL_MASK)) {
        }
        USART0->FIFOWR = str[i];

    }
    /* Wait to finish transfer */
    while (!(USART0->STAT & USART_STAT_TXIDLE_MASK)) {
    }
}

