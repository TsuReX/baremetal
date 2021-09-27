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
int32_t usb_init(void)
{
	/* Init Device Library, add supported class and start the library. */

	if (usb_device_init(&usb_device, &device_descriptors, DEVICE_FS) != USBD_OK) {
		printk(DEBUG, "usb_device_init error\r\n");
		return -1;
	}

	if (usb_device_register_class(&usb_device, &usb_communication_device_class) != USBD_OK) {
		printk(DEBUG, "usb_device_register_class error\r\n");
		return -2;
	}

	if (usb_device_register_cdc_operations(&usb_device, &cdc_operations) != USBD_OK) {
		printk(DEBUG, "usb_device_register_cdc_operations error\r\n");
		return -3;
	}

	if (usb_device_start(&usb_device) != USBD_OK) {
		printk(DEBUG, "usb_device_start error\r\n");
		return -4;
	}
	usb_inited = 1;
	return 0;
}
