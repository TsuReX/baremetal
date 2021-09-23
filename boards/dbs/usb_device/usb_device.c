#include "usb_device.h"
#include "usbd_core.h"
#include "usbd_desc.h"
#include "usbd_cdc.h"
#include "usbd_cdc_if.h"

#include "debug.h"

USBD_HandleTypeDef usb_device;
uint32_t usb_inited = 0;

/**
  * Init USB device Library, add supported class and start the library
  * @retval None
  */
void usb_init(void)
{
	/* Init Device Library, add supported class and start the library. */

	if (usb_device_init(&usb_device, &device_descriptors, DEVICE_FS) != USBD_OK) {
		printk(DEBUG, "USBD_Init error\r\n");
		return;
	}

	if (usb_device_register_class(&usb_device, &usb_communication_device_class) != USBD_OK) {
		printk(DEBUG, "USBD_RegisterClass error\r\n");
		return;
	}

	if (usb_device_register_cdc_operations(&usb_device, &cdc_operations) != USBD_OK) {
		printk(DEBUG, "USBD_CDC_RegisterInterface error\r\n");
		return;
	}

	if (usb_device_start(&usb_device) != USBD_OK) {
		printk(DEBUG, "USBD_Start error\r\n");
		return;
	}
	usb_inited = 1;
}
