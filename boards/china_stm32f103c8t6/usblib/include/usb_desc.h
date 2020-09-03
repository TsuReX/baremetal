#ifndef __USB_DESC_H
#define __USB_DESC_H

#define DEV_DESCRIPTOR_TYPE				0x01
#define CONFIG_DESCRIPTOR_TYPE			0x02
#define USB_STRING_DESCRIPTOR_TYPE		0x03
#define USB_INTERFACE_DESCRIPTOR_TYPE	0x04
#define ENDP_DESCRIPTOR_TYPE			0x05

#define HID_DESCRIPTOR_TYPE				0x21
#define HID_REPORT_DESCRIPTOR_TYPE		0x22

#define RHID_SIZ_HID_DESC				0x09
#define RHID_OFF_HID_DESC				0x12

#define DEV_DESCRIPTOR_SIZE				0x12
//#define CONFIG_DESCRIPTOR_SIZE			0x22
#define CONFIG_DESCRIPTOR_SIZE			0x3B
#define KEYBOARD_REPORT_DESCRIPTOR_SIZE	0x3F
#define MOUSE_REPORT_DESCRIPTOR_SIZE	0x32
#define RHID_SIZ_STRING_LANGID			0x04
#define RHID_SIZ_STRING_VENDOR			0x0A
#define RHID_SIZ_STRING_PRODUCT			0x16
#define RHID_SIZ_STRING_SERIAL			0x1A

#define STANDARD_ENDPOINT_DESC_SIZE		0x09

#define DEVICE_VER_H					0x01
#define DEVICE_VER_L					0x00

#define EP0_MAX_PACKET_SIZE				0x40
#define EP1_MAX_PACKET_SIZE				0x8
#define EP2_MAX_PACKET_SIZE				0x3

/* Exported functions ------------------------------------------------------- */
extern const uint8_t rhid_device_descriptor[DEV_DESCRIPTOR_SIZE];
extern const uint8_t config_descriptor[CONFIG_DESCRIPTOR_SIZE];
extern const uint8_t keyboard_rhid_report_descriptor[KEYBOARD_REPORT_DESCRIPTOR_SIZE];
extern const uint8_t mouse_rhid_report_descriptor[MOUSE_REPORT_DESCRIPTOR_SIZE];
extern const uint8_t rhid_string_lang_id[RHID_SIZ_STRING_LANGID];
extern const uint8_t rhid_string_vendor[RHID_SIZ_STRING_VENDOR];
extern const uint8_t rhid_string_product[RHID_SIZ_STRING_PRODUCT];
extern uint8_t rhid_string_serial[RHID_SIZ_STRING_SERIAL];

#endif /* __USB_DESC_H */
