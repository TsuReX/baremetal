/**
 * @file	src/main.c
 *
 * @brief	Объявление точки вхождения в C-код.
 *
 * @author	Vasily Yurchenko <vasily.v.yurchenko@yandex.ru>
 */
#include <string.h>

#include "drivers.h"
#include "communication.h"
#include "init.h"
#include "kbmsusb.h"
#include "usb_core.h"

uint8_t		ep_index;
uint16_t	wInterrupt_Mask;
DEVICE_INFO *usb_device_info;
DEVICE_PROP *usb_device_property;
uint16_t	SaveState ;
DEVICE_INFO	device_info;
USER_STANDARD_REQUESTS  *usb_standard_requests;

uint8_t comm_buff[sizeof(struct kbms_data)] = {0,1,2,3,4,5,6,7,8,9,0xA,0xB,0xC};

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

#if 0

	EXTI_ClearITPendingBit(EXTI_Line18);
	EXTI_InitStructure.EXTI_Line = EXTI_Line18;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);

#endif

	LL_EXTI_ClearFlag_0_31(LL_EXTI_LINE_18);
	LL_EXTI_EnableRisingTrig_0_31(LL_EXTI_LINE_18);
	LL_EXTI_EnableIT_0_31(LL_EXTI_LINE_18);

	usb_device_property->init();
}

int main(void)
{
	soc_init();

	board_init();

	comm_init(&comm_buff, sizeof(comm_buff));

	comm_start();

	usb_init();
	LL_GPIO_SetOutputPin(GPIOB, LL_GPIO_PIN_7);
	while(1) {
//		__WFI();
	}
}
