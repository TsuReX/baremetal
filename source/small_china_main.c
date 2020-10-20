/**
 * @file	src/main.c
 *
 * @brief	Объявление точки вхождения в C-код.
 *
 * @author	Vasily Yurchenko <vasily.v.yurchenko@yandex.ru>
 */

#include "drivers.h"
#include "config.h"
#include "console.h"
#include "scheduler.h"
#include "spi.h"
#include "communication.h"
#include "max3421e.h"
#include "utils.h"

#include "usb_core.h"

uint8_t		ep_index;
uint16_t	wInterrupt_Mask;
DEVICE_INFO *usb_device_info;
DEVICE_PROP *usb_device_property;
uint16_t	SaveState ;
DEVICE_INFO	device_info;
USER_STANDARD_REQUESTS  *usb_standard_requests;

uint8_t comm_buff[16] = {0,1,2,3,4,5,6,7,8,9,0xA,0xB,0xC,0xD,0xE,0xF};

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

void spi_flash_test(void) {

	uint8_t buffer[SPI_ID_BYTES + 1] = {0,0,0,0};
	size_t i = 0;

	buffer[0] = SPI_CMD_RDID;

	spi_chip_activate();

	for (i = 0; i < sizeof(buffer) / sizeof(buffer[0]); ++i) {

		while (!LL_SPI_IsActiveFlag_TXE(SPI1));
		LL_SPI_TransmitData8(SPI1, buffer[i]);

		while (!LL_SPI_IsActiveFlag_RXNE(SPI1));
		buffer[i] = LL_SPI_ReceiveData8(SPI1);
	}

	spi_chip_deactivate();

	d_print("MC_SPI_FLASH_ID (Hex) ");
	for (i = 1; i < sizeof(buffer) / sizeof(buffer[0]); ++i)
		d_print("%02X ", buffer[i]);
}

void spi_usb_test(void)
{
//	LL_GPIO_ResetOutputPin(GPIOB, LL_GPIO_PIN_0);

	/* 0. FDUPSPI */
	kb_usb_fullduplex_spi_set();

	/* 1. REVISON */
	kb_usb_revision_read();

	/* 2. CHIPRES, OSCOKIRQ */
	kb_usb_chip_reset();

	/* 2.1 GPOUT0 */
	kb_usb_power_enable();

	/* 3. HOST, DPPULLDN, DMPULLDN */
	kb_usb_mode_set();

	mdelay(4000);

	kb_usb_device_detect();
//	kb_usb_device_poll_detection_cycle();
//	kb_usb_device_irq_detection_cycle();

	kb_usb_bus_reset();

	kb_usb_sof_start();

	kb_usb_bus_reset();

	kb_usb_setup_set_address(0x34);

	kb_usb_hs_in_send(0x00);

	kb_usb_setup_get_dev_descr(0x34);

	mdelay(500);

	kb_usb_setup_get_conf_descr(0x34);

	mdelay(500);

	kb_usb_setup_get_full_conf(0x34);

	/* 4. BUSRST, SOFKAENAB, FRAMEIRQ */
	/* 5. CONDETIRQ, SAMPLEBUS, JSTATUS, KTATUS */
	/* 6. SETUP HS-IN */

}

void usb_init(void)
{

	NVIC_SetPriorityGrouping(2);

	NVIC_SetPriority(USB_LP_CAN1_RX0_IRQn, 2);
	NVIC_EnableIRQ(USB_LP_CAN1_RX0_IRQn);

	NVIC_SetPriority(USBWakeUp_IRQn, 1);
	NVIC_EnableIRQ(USBWakeUp_IRQn);

	usb_device_info = &device_info;
	usb_device_info->control_state = IN_DATA;
	usb_device_property = &property;
	usb_standard_requests = &standard_requests;

//	  EXTI_ClearITPendingBit(EXTI_Line18);
//	  EXTI_InitStructure.EXTI_Line = EXTI_Line18;
//	  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
//	  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
//	  EXTI_Init(&EXTI_InitStructure);

	LL_EXTI_ClearFlag_0_31(LL_EXTI_LINE_18);
	LL_EXTI_EnableRisingTrig_0_31(LL_EXTI_LINE_18);
	LL_EXTI_EnableIT_0_31(LL_EXTI_LINE_18);

	usb_device_property->init();
}
/**
 * @brief	C-code entry point.
 *
 * @retval	0 in case of success, otherwise error code
 */
int main(void)
{
	soc_init();

	board_init();

	console_init();

//	scheduler_init();

	spi_init();
//	spi_flash_test();
	spi_usb_test();


//	comm_init(&comm_buff, sizeof(comm_buff));
//	comm_start();

//	usb_init();

//	uint32_t i = 0;
	while (1) {
		LL_mDelay(DELAY_500_MS * 2);
//		print("Main thread iteration %ld\r\n", i++);
		/*ina3221_print_voltage_current();*/
//		LL_GPIO_TogglePin(GPIOA, LL_GPIO_PIN_0);
		LL_GPIO_TogglePin(GPIOC, LL_GPIO_PIN_13);
//		LL_GPIO_TogglePin(GPIOC, LL_GPIO_PIN_14);
//		LL_GPIO_TogglePin(GPIOC, LL_GPIO_PIN_15);
	}
}
