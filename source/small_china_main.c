/**
 * @file	src/main.c
 *
 * @brief	Объявление точки вхождения в C-код.
 *
 * @author	Vasily Yurchenko <vasily.v.yurchenko@yandex.ru>
 */
#include <string.h>
#include "drivers.h"
#include "config.h"
#include "console.h"
#include "scheduler.h"
#include "spi.h"
#include "communication.h"
#include "max3421e.h"
#include "utils.h"

#include "kbmsusb.h"
#include "usb_core.h"

uint8_t		ep_index;
uint16_t	wInterrupt_Mask;
DEVICE_INFO *usb_device_info;
DEVICE_PROP *usb_device_property;
uint16_t	SaveState ;
DEVICE_INFO	device_info;
USER_STANDARD_REQUESTS  *usb_standard_requests;

uint8_t comm_buff[sizeof(struct kbms_data)] = {0,1,2,3,4,5,6,7,8,9,0xA,0xB};

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

//#define USB_DEVICE
#define USB_MASTER

void max3421e_chip_activate(uint32_t chip_num)
{
	switch(chip_num) {
		case KEYBOARD_CHANNEL:
			LL_GPIO_ResetOutputPin(GPIOA, LL_GPIO_PIN_4);
//#warning "Implement"
			break;

		case MOUSE_CHANNEL:
			LL_GPIO_ResetOutputPin(GPIOA, LL_GPIO_PIN_4);
//#warning "Implement"
			break;
	};
}

void max3421e_chip_deactivate(uint32_t chip_num)
{
	switch(chip_num) {
		case KEYBOARD_CHANNEL:
			LL_GPIO_SetOutputPin(GPIOA, LL_GPIO_PIN_4);
//#warning "Implement"

			break;

		case MOUSE_CHANNEL:
			LL_GPIO_SetOutputPin(GPIOA, LL_GPIO_PIN_4);
//#warning "Implement"
			break;
	};
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

#if defined(USB_MASTER)
	console_init();
	spi_init();
	spi_usb_transmission_start();
	while (1) {
		LL_mDelay(DELAY_500_MS * 2);
		LL_GPIO_TogglePin(GPIOC, LL_GPIO_PIN_13);

	}
#elif defined(USB_DEVICE)

	comm_init(&comm_buff, sizeof(comm_buff));
	comm_start();
//	d_print("USB Device\r\n");
	usb_init();

#else
	console_init();
#endif

	while(1){
		;
	}
}
