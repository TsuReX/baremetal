#ifndef __USB_PROP_H
#define __USB_PROP_H

#include <usb_lib.h>

typedef enum _HID_REQUESTS {
	GET_REPORT = 1,
	GET_IDLE,
	GET_PROTOCOL,

	SET_REPORT = 9,
	SET_IDLE,
	SET_PROTOCOL
} HID_REQUESTS;

void hid_init(void);
void hid_reset(void);
void HID_SetConfiguration(void);
void HID_SetDeviceAddress (void);
void HID_Status_In (void);
void HID_Status_Out (void);
RESULT hid_setup_with_data_process(uint8_t);
RESULT hid_setup_without_data_process(uint8_t);
RESULT HID_Get_Interface_Setting(uint8_t Interface, uint8_t AlternateSetting);
uint8_t *HID_GetDeviceDescriptor(uint16_t );
uint8_t *HID_GetConfigDescriptor(uint16_t);
uint8_t *HID_GetStringDescriptor(uint16_t);
RESULT HID_SetProtocol(void);
uint8_t *HID_GetProtocolValue(uint16_t Length);
RESULT HID_SetProtocol(void);
uint8_t *HID_GetReportDescriptor(uint16_t Length);
uint8_t *HID_GetHIDDescriptor(uint16_t Length);

#define NOP_HID_GetConfiguration          NOP_Process
//#define NOP_HID_SetConfiguration          NOP_Process
#define NOP_HID_GetInterface              NOP_Process
#define NOP_HID_SetInterface              NOP_Process
#define NOP_HID_GetStatus                 NOP_Process
#define NOP_HID_ClearFeature              NOP_Process
#define NOP_HID_SetEndPointFeature        NOP_Process
#define NOP_HID_SetDeviceFeature          NOP_Process
//#define HID_SetDeviceAddress          NOP_Process

#define REPORT_DESCRIPTOR                  0x22

#endif /* __USB_PROP_H */
