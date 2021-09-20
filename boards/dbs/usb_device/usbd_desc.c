#include "usbd_core.h"
#include "usbd_desc.h"
#include "usbd_conf.h"

#define USBD_VID						1155
#define USBD_LANGID_STRING				1033
#define USBD_MANUFACTURER_STRING		"STMicroelectronics"
#define USBD_PID_FS						22336
#define USBD_PRODUCT_STRING_FS			"STM32 Virtual ComPort"
#define USBD_CONFIGURATION_STRING_FS	"CDC Config"
#define USBD_INTERFACE_STRING_FS		"CDC Interface"

#define USB_SIZ_BOS_DESC				0x0C

static void Get_SerialNum(void);
static void IntToUnicode(uint32_t value, uint8_t * pbuf, uint8_t len);

uint8_t * device_descriptor_get(USBD_SpeedTypeDef speed, uint16_t *length);
uint8_t * lang_id_descriptor_get(USBD_SpeedTypeDef speed, uint16_t *length);
uint8_t * manufacturer_descriptor_get(USBD_SpeedTypeDef speed, uint16_t *length);
uint8_t * product_descriptor_get(USBD_SpeedTypeDef speed, uint16_t *length);
uint8_t * serial_descriptor_get(USBD_SpeedTypeDef speed, uint16_t *length);
uint8_t * config_descriptor_get(USBD_SpeedTypeDef speed, uint16_t *length);
uint8_t * interface_descriptor_get(USBD_SpeedTypeDef speed, uint16_t *length);
#if (USBD_LPM_ENABLED == 1)
uint8_t * USBD_FS_USR_BOSDescriptor(USBD_SpeedTypeDef speed, uint16_t *length);
#endif /* (USBD_LPM_ENABLED == 1) */

USBD_DescriptorsTypeDef device_descriptors = {
	device_descriptor_get,
	lang_id_descriptor_get,
	manufacturer_descriptor_get,
	product_descriptor_get,
	serial_descriptor_get,
	config_descriptor_get,
	interface_descriptor_get,
#if (USBD_LPM_ENABLED == 1)
	USBD_FS_USR_BOSDescriptor
#endif /* (USBD_LPM_ENABLED == 1) */
};

/** USB standard device descriptor. */
uint8_t device_desc[USB_LEN_DEV_DESC] =
{
	0x12,                       /*bLength */
	USB_DESC_TYPE_DEVICE,       /*bDescriptorType*/
#if (USBD_LPM_ENABLED == 1)
	0x01,                       /*bcdUSB */ /* changed to USB version 2.01
											 in order to support LPM L1 suspend
											 resume test of USBCV3.0*/
#else
	0x00,                       /*bcdUSB */
#endif /* (USBD_LPM_ENABLED == 1) */
	0x02,
	0x02,                       /*bDeviceClass*/
	0x02,                       /*bDeviceSubClass*/
	0x00,                       /*bDeviceProtocol*/
	USB_MAX_EP0_SIZE,           /*bMaxPacketSize*/
	LOBYTE(USBD_VID),           /*idVendor*/
	HIBYTE(USBD_VID),           /*idVendor*/
	LOBYTE(USBD_PID_FS),        /*idProduct*/
	HIBYTE(USBD_PID_FS),        /*idProduct*/
	0x00,                       /*bcdDevice rel. 2.00*/
	0x02,
	USBD_IDX_MFC_STR,           /*Index of manufacturer  string*/
	USBD_IDX_PRODUCT_STR,       /*Index of product string*/
	USBD_IDX_SERIAL_STR,        /*Index of serial number string*/
	USBD_MAX_NUM_CONFIGURATION  /*bNumConfigurations*/
};

/* USB_DeviceDescriptor */
/** BOS descriptor. */
#if (USBD_LPM_ENABLED == 1)
__ALIGN_BEGIN uint8_t USBD_FS_BOSDesc[USB_SIZ_BOS_DESC] __ALIGN_END =
{
  0x5,
  USB_DESC_TYPE_BOS,
  0xC,
  0x0,
  0x1,  /* 1 device capability*/
        /* device capability*/
  0x7,
  USB_DEVICE_CAPABITY_TYPE,
  0x2,
  0x2,  /* LPM capability bit set*/
  0x0,
  0x0,
  0x0
};
#endif /* (USBD_LPM_ENABLED == 1) */

/** USB lang identifier descriptor. */
uint8_t lang_id_desc[USB_LEN_LANGID_STR_DESC] =
{
	USB_LEN_LANGID_STR_DESC,
	USB_DESC_TYPE_STRING,
	LOBYTE(USBD_LANGID_STRING),
	HIBYTE(USBD_LANGID_STRING)
};

/* Internal string descriptor. */
uint8_t str_desc[USBD_MAX_STR_DESC_SIZ];

uint8_t string_serial[USB_SIZ_STRING_SERIAL] = {
	USB_SIZ_STRING_SERIAL,
	USB_DESC_TYPE_STRING,
};

/**
  * @brief  Return the device descriptor
  * @param  speed : Current device speed
  * @param  length : Pointer to data length variable
  * @retval Pointer to descriptor buffer
  */
uint8_t * device_descriptor_get(USBD_SpeedTypeDef speed, uint16_t *length)
{
  UNUSED(speed);
  *length = sizeof(device_desc);
  return device_desc;
}

/**
  * @brief  Return the LangID string descriptor
  * @param  speed : Current device speed
  * @param  length : Pointer to data length variable
  * @retval Pointer to descriptor buffer
  */
uint8_t * lang_id_descriptor_get(USBD_SpeedTypeDef speed, uint16_t *length)
{
	UNUSED(speed);
	*length = sizeof(lang_id_desc);
	return lang_id_desc;
}

/**
  * @brief  Return the product string descriptor
  * @param  speed : Current device speed
  * @param  length : Pointer to data length variable
  * @retval Pointer to descriptor buffer
  */
uint8_t * product_descriptor_get(USBD_SpeedTypeDef speed, uint16_t *length)
{
	if (speed == 0) {
		USBD_GetString((uint8_t *)USBD_PRODUCT_STRING_FS, str_desc, length);
	} else {
		USBD_GetString((uint8_t *)USBD_PRODUCT_STRING_FS, str_desc, length);
	}
	return str_desc;
}

/**
  * @brief  Return the manufacturer string descriptor
  * @param  speed : Current device speed
  * @param  length : Pointer to data length variable
  * @retval Pointer to descriptor buffer
  */
uint8_t * manufacturer_descriptor_get(USBD_SpeedTypeDef speed, uint16_t *length)
{
	UNUSED(speed);
	USBD_GetString((uint8_t *)USBD_MANUFACTURER_STRING, str_desc, length);
	return str_desc;
}

/**
  * @brief  Return the serial number string descriptor
  * @param  speed : Current device speed
  * @param  length : Pointer to data length variable
  * @retval Pointer to descriptor buffer
  */
uint8_t * serial_descriptor_get(USBD_SpeedTypeDef speed, uint16_t *length)
{
	UNUSED(speed);
	*length = USB_SIZ_STRING_SERIAL;

	/* Update the serial number string descriptor with the data from the unique
	* ID */
	Get_SerialNum();
	/* USER CODE BEGIN USBD_FS_SerialStrDescriptor */

	/* USER CODE END USBD_FS_SerialStrDescriptor */
	return (uint8_t *) string_serial;
}

/**
  * @brief  Return the configuration string descriptor
  * @param  speed : Current device speed
  * @param  length : Pointer to data length variable
  * @retval Pointer to descriptor buffer
  */
uint8_t * config_descriptor_get(USBD_SpeedTypeDef speed, uint16_t *length)
{
	if (speed == USBD_SPEED_HIGH) {
		USBD_GetString((uint8_t *)USBD_CONFIGURATION_STRING_FS, str_desc, length);
	} else {
		USBD_GetString((uint8_t *)USBD_CONFIGURATION_STRING_FS, str_desc, length);
	}
	return str_desc;
}

/**
  * @brief  Return the interface string descriptor
  * @param  speed : Current device speed
  * @param  length : Pointer to data length variable
  * @retval Pointer to descriptor buffer
  */
uint8_t * interface_descriptor_get(USBD_SpeedTypeDef speed, uint16_t *length)
{
	if (speed == 0) {
		USBD_GetString((uint8_t *)USBD_INTERFACE_STRING_FS, str_desc, length);
	} else {
		USBD_GetString((uint8_t *)USBD_INTERFACE_STRING_FS, str_desc, length);
	}
	return str_desc;
}

#if (USBD_LPM_ENABLED == 1)
/**
  * @brief  Return the BOS descriptor
  * @param  speed : Current device speed
  * @param  length : Pointer to data length variable
  * @retval Pointer to descriptor buffer
  */
uint8_t * USBD_FS_USR_BOSDescriptor(USBD_SpeedTypeDef speed, uint16_t *length)
{
  UNUSED(speed);
  *length = sizeof(USBD_FS_BOSDesc);
  return (uint8_t*)USBD_FS_BOSDesc;
}
#endif /* (USBD_LPM_ENABLED == 1) */

/**
  * @brief  Create the serial number string descriptor
  * @param  None
  * @retval None
  */
static void Get_SerialNum(void)
{
	uint32_t deviceserial0, deviceserial1, deviceserial2;

	deviceserial0 = *(uint32_t *) DEVICE_ID1;
	deviceserial1 = *(uint32_t *) DEVICE_ID2;
	deviceserial2 = *(uint32_t *) DEVICE_ID3;

	deviceserial0 += deviceserial2;

	if (deviceserial0 != 0) {
		IntToUnicode(deviceserial0, &string_serial[2], 8);
		IntToUnicode(deviceserial1, &string_serial[18], 4);
	}
}

/**
  * @brief  Convert Hex 32Bits value into char
  * @param  value: value to convert
  * @param  pbuf: pointer to the buffer
  * @param  len: buffer length
  * @retval None
  */
static void IntToUnicode(uint32_t value, uint8_t * pbuf, uint8_t len)
{
	uint8_t idx = 0;

	for (idx = 0; idx < len; idx++) {
		if (((value >> 28)) < 0xA) {
			pbuf[2 * idx] = (value >> 28) + '0';
		} else {
			pbuf[2 * idx] = (value >> 28) + 'A' - 10;
		}

		value = value << 4;

		pbuf[2 * idx + 1] = 0;
	}
}
