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
/*#include "i2c.h"
#include "ds3231m.h"
#include "ina3221.h"
#include "gpio.h"
*/
#include "usb_endp.h"
#include "usb_lib.h"
#include "usb_desc.h"

/*  The number of current endpoint, it will be used to specify an endpoint */
 uint8_t	ep_index;
uint16_t	wInterrupt_Mask;

 /*  The number of current device, it is an index to the Device_Table */
/* uint8_t	Device_no; */
/*  Points to the DEVICE_INFO structure of current device */
/*  The purpose of this register is to speed up the execution */
DEVICE_INFO *usb_device_info;

/*  Points to the DEVICE_PROP structure of current device */
/*  The purpose of this register is to speed up the execution */
DEVICE_PROP *usb_device_property;

/*  Temporary save the state of Rx & Tx status. */
/*  Whenever the Rx or Tx state is changed, its value is saved */
/*  in this variable first and will be set to the EPRB or EPRA */
/*  at the end of interrupt process */
uint16_t	SaveState ;
DEVICE_INFO	device_info;
USER_STANDARD_REQUESTS  *usb_standard_requests;

void usb_init(void)
{
	NVIC_SetPriority(USB_LP_CAN1_RX0_IRQn, 0);
	NVIC_EnableIRQ(USB_LP_CAN1_RX0_IRQn);

	NVIC_SetPriority(USBWakeUp_IRQn, 0);
	NVIC_EnableIRQ(USBWakeUp_IRQn);

	usb_device_info = &device_info;
	usb_device_info->control_state = IN_DATA;
	usb_device_property = &property;
	usb_standard_requests = &standard_requests;
	/* Initialize devices one by one */
	usb_device_property->init();
}
/**
 * @brief	C-code entry point.
 *
 * @retval	0 in case of success, otherwise error code
 */
int main(void)
{
	/** Configure internal subsystems of SoC. */
	soc_init();

	/** Configure board's peripherals. */
	board_init();

	console_init();

	scheduler_init();

	/*i2c_init();*/

	/*gpio_setup_it();*/

	usb_init();

	uint32_t i = 0;
	while (1) {
		LL_mDelay(DELAY_500_MS * 2);
		print("Main thread iteration %ld\r\n", i++);
		/*ina3221_print_voltage_current();*/
//		LL_GPIO_TogglePin(GPIOA, LL_GPIO_PIN_0);
//		LL_GPIO_TogglePin(GPIOC, LL_GPIO_PIN_13);
//		LL_GPIO_TogglePin(GPIOC, LL_GPIO_PIN_14);
//		LL_GPIO_TogglePin(GPIOC, LL_GPIO_PIN_15);
	}
}
