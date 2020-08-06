#include <usb_init.h>

/*  The number of current endpoint, it will be used to specify an endpoint */
 uint8_t	ep_index;

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
uint16_t	wInterrupt_Mask;
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
	usb_standard_requests = &user_standard_requests;
	/* Initialize devices one by one */
	usb_device_property->Init();
}
