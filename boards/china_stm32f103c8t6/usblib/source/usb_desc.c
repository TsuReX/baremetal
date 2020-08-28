#include "usb_lib.h"
#include "usb_desc.h"

/* USB Standard Device Descriptor */
const uint8_t rhid_device_descriptor[DEV_DESCRIPTOR_SIZE] = {

	DEV_DESCRIPTOR_SIZE,		// bLength
	DEV_DESCRIPTOR_TYPE,		// bDescriptorType -  - Device descriptor
	0x00, 0x02,                 // bcdUSB - . 2.0
	0x00,                       // bDeviceClass
	0x00,                       // bDeviceSubClass
	0x00,                       // bDeviceProtocol
	0x40,                       // bMaxPacketSize - Endpoint 0
//	0x83, 0x04,                 // idVendor (0x0483)
//	0x11, 0x57,                 // idProduct (0x5711)
	0xEF, 0x17,
	0x8C, 0x60,
	DEVICE_VER_L, DEVICE_VER_H,	// bcdDevice rel. DEVICE_VER_H.DEVICE_VER_L
	0x01,						// Index of string descriptor describing manufacturer
	0x02,						// Index of string descriptor describing product
	0x03,						// Index of string descriptor describing the device serial number
	0x01						// bNumConfigurations -
}; /* CustomHID_DeviceDescriptor */


/*   All Descriptors (Configuration, Interface, Endpoint, Class, Vendor */
/* USB Configuration Descriptor */
const uint8_t config_descriptor[CONFIG_DESCRIPTOR_SIZE] = {

	0x09, 							// bLength:
	CONFIG_DESCRIPTOR_TYPE,			// bDescriptorType:
	CONFIG_DESCRIPTOR_SIZE, 0x00,	// wTotalLength:
	0x01,							// bNumInterfaces:
	0x01,							// bConfigurationValue:
	0x00,							// iConfiguration:
	0xA0,							// bmAttributes:  USB
	0x32,							// MaxPower 100 mA:

/* USB Standard Interface Descriptor */
	0x09,							// bLength:
	USB_INTERFACE_DESCRIPTOR_TYPE,	// bDescriptorType:
	0x00,							// bInterfaceNumber:
	0x00,							// bAlternateSetting:
	0x01,							// bNumEndpoints -
	0x03,							// bInterfaceClass:
	0x01,							// bInterfaceSubClass :
	0x01,							// nInterfaceProtocol :
	0x00,							// iInterface:

/* USB HID Descriptor */
	0x09,							// bLength:
	HID_DESCRIPTOR_TYPE,			// bDescriptorType:
	0x01, 0x01,						// bcdHID:
	0x00,							// bCountryCode:
	0x01,							// bNumDescriptors:
	HID_REPORT_DESCRIPTOR_TYPE,		// bDescriptorType:
	REPORT_DESCRIPTOR_SIZE, 0x00,	// wItemLength:

/* USB Standard Endpoint Descriptor (endpoint 1)*/
	0x07,							// bLength:
	ENDP_DESCRIPTOR_TYPE,			// bDescriptorType:
	0x81,							// bEndpointAddress:
	0x03,							// bmAttributes:
	0x08, 0x00,						// wMaxPacketSize:  Bytes max
	0xFA,							// bInterval: Polling Interval (250 ms)
};

const uint8_t rhid_report_descriptor[REPORT_DESCRIPTOR_SIZE + 3] = {
		0x05, 0x01,
		0x09, 0x06,
		0xA1, 0x01,
		0x05, 0x07,
		0x19, 0xE0,
		0x29, 0xE7,
		0x15, 0x00,
		0x25, 0x01,
		0x75, 0x01,
		0x95, 0x08,
		0x81, 0x02,
		0x95, 0x01,
		0x75, 0x08,
		0x81, 0x01,
		0x95, 0x05,
		0x75, 0x01,
		0x05, 0x08,
		0x19, 0x01,
		0x29, 0x05,
		0x91, 0x02,
		0x95, 0x01,
		0x75, 0x03,
		0x91, 0x01,
		0x95, 0x06,
		0x75, 0x08,
		0x15, 0x00,
		0x25, 0x65,
		0x05, 0x07,
		0x19, 0x00,
		0x29, 0x65,
		0x81, 0x00,
		0xC0, 0x11,
		0x22, 0x33
};

/* USB String Descriptors (optional) */
const uint8_t rhid_string_lang_id[RHID_SIZ_STRING_LANGID] = {
	RHID_SIZ_STRING_LANGID,
	USB_STRING_DESCRIPTOR_TYPE,
	0x09,
	0x04
}; /* LangID = 0x0409: U.S. English */

const uint8_t rhid_string_vendor[RHID_SIZ_STRING_VENDOR] = {
	RHID_SIZ_STRING_VENDOR, /* Size of Vendor string */
	USB_STRING_DESCRIPTOR_TYPE,  /* bDescriptorType*/
	/* Manufacturer: "STMicroelectronics" */
	'R', 0, 'a', 0, 'J', 0, 'a', 0
};

const uint8_t rhid_string_product[RHID_SIZ_STRING_PRODUCT] = {
	RHID_SIZ_STRING_PRODUCT,          /* bLength */
	USB_STRING_DESCRIPTOR_TYPE,        /* bDescriptorType */
	'R', 0, 'H', 0, 'I', 0, 'D', 0, ' ', 0, 'S', 0, 'T', 0,
	'M', 0, '3', 0, '2', 0
};

uint8_t rhid_string_serial[RHID_SIZ_STRING_SERIAL] = {
	RHID_SIZ_STRING_SERIAL,           /* bLength */
	USB_STRING_DESCRIPTOR_TYPE,        /* bDescriptorType */
	'R', 0, 'H', 0, '-', 0, '0', 0, '0', 0, '0', 0, '1', 0
};
