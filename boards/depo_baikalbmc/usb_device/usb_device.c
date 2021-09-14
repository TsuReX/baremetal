#include "usb_device.h"
#include "usbd_core.h"
#include "usbd_desc.h"
#include "usbd_cdc.h"
#include "usbd_cdc_if.h"

#include "debug.h"


USBD_HandleTypeDef hUsbDeviceFS;

/**
  * Init USB device Library, add supported class and start the library
  * @retval None
  */
void MX_USB_DEVICE_Init(void)
{
	/* Init Device Library, add supported class and start the library. */

	if (USBD_Init(&hUsbDeviceFS, &FS_Desc, DEVICE_FS) != USBD_OK) {
		printk(DEBUG, "USBD_Init error\r\n");
		return;
	}

	if (USBD_RegisterClass(&hUsbDeviceFS, &USBD_CDC) != USBD_OK) {
		printk(DEBUG, "USBD_RegisterClass error\r\n");
		return;
	}

	if (USBD_CDC_RegisterInterface(&hUsbDeviceFS, &USBD_Interface_fops_FS) != USBD_OK) {
		printk(DEBUG, "USBD_CDC_RegisterInterface error\r\n");
		return;
	}

	if (USBD_Start(&hUsbDeviceFS) != USBD_OK) {
		printk(DEBUG, "USBD_Start error\r\n");
		return;
	}
}
