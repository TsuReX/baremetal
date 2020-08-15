#ifndef __USB_CORE_H
#define __USB_CORE_H

#include <stdint.h>
#include <drivers.h>
#include <usb_lib.h>

typedef enum _CONTROL_STATE {
	WAIT_SETUP,       /* 0 */
	SETTING_UP,       /* 1 */
	IN_DATA,          /* 2 */
	OUT_DATA,         /* 3 */
	LAST_IN_DATA,     /* 4 */
	LAST_OUT_DATA,    /* 5 */
	WAIT_STATUS_IN,   /* 7 */
	WAIT_STATUS_OUT,  /* 8 */
	STALLED,          /* 9 */
	PAUSE             /* 10 */
} CONTROL_STATE;    /* The state machine states of a control pipe */

typedef struct OneDescriptor {
	uint8_t *Descriptor;
	uint16_t Descriptor_Size;
} ONE_DESCRIPTOR, *PONE_DESCRIPTOR;

typedef struct _ENDPOINT_INFO {
	uint16_t  remaining_data_size;
	uint16_t  data_buffer_offset;
	uint16_t  single_transfer_size;
	uint8_t   *(*data_copy)(uint16_t Length);
} ENDPOINT_INFO;

typedef struct _DEVICE {
	uint8_t Total_Endpoint;     /* Number of endpoints that are used */
	uint8_t Total_Configuration;/* Number of configuration available */
} DEVICE;

//typedef union {
//	uint16_t w;
//
//	struct BW {
//		uint8_t bb1;
//		uint8_t bb0;
//	} bw;
//
//} uint16_t_uint8_t;

typedef struct _DEVICE_INFO {
	uint8_t		bm_request_type;	/* bmRequestType */
	uint8_t		b_request;			/* bRequest */
	uint16_t	w_value;			/* wValue */
	uint16_t	w_index;			/* wIndex */
	uint16_t	w_length;			/* wLength */

	uint8_t				control_state;				/* of type CONTROL_STATE */
	uint8_t				Current_Feature;
	uint8_t				Current_Configuration;		/* Selected configuration */
	uint8_t				Current_Interface;			/* Selected interface of current configuration */
	uint8_t				Current_AlternateSetting;	/* Selected Alternate Setting of current interface*/

	ENDPOINT_INFO		ep0_ctrl_info;

} DEVICE_INFO;

typedef struct _DEVICE_PROP {
	void (*init)(void);        /* Initialize the device */
	void (*reset)(void);       /* Reset routine of this device */

	/* Device dependent process after the status stage */
	void (*status_in_process)(void);
	void (*status_out_process)(void);
	RESULT (*class_setup_with_data_process)(uint8_t RequestNo);

	RESULT (*class_setup_without_data_process)(uint8_t RequestNo);

	RESULT  (*Class_Get_Interface_Setting)(uint8_t Interface, uint8_t AlternateSetting);

	uint8_t* (*GetDeviceDescriptor)(uint16_t Length);
	uint8_t* (*GetConfigDescriptor)(uint16_t Length);
	uint8_t* (*GetStringDescriptor)(uint16_t Length);

	uint8_t MaxPacketSize;

} DEVICE_PROP;

typedef struct _USER_STANDARD_REQUESTS {
	void (*User_GetConfiguration)(void);       /* Get Configuration */
	void (*User_SetConfiguration)(void);       /* Set Configuration */
	void (*User_GetInterface)(void);           /* Get Interface */
	void (*User_SetInterface)(void);           /* Set Interface */
	void (*User_GetStatus)(void);              /* Get Status */
	void (*User_ClearFeature)(void);           /* Clear Feature */
	void (*User_SetEndPointFeature)(void);     /* Set Endpoint Feature */
	void (*User_SetDeviceFeature)(void);       /* Set Device Feature */
	void (*User_SetDeviceAddress)(void);       /* Set Device Address */
} USER_STANDARD_REQUESTS;

uint8_t ep0_setup_process(void);
uint8_t ep0_finish_processing(void);
uint8_t ep0_out_process(void);
uint8_t ep0_in_process(void);

RESULT Standard_SetEndPointFeature(void);
RESULT Standard_SetDeviceFeature(void);

uint8_t *Standard_GetConfiguration(uint16_t Length);
RESULT Standard_SetConfiguration(void);
uint8_t *Standard_GetInterface(uint16_t Length);
RESULT Standard_SetInterface(void);
uint8_t *Standard_GetDescriptorData(uint16_t Length, PONE_DESCRIPTOR pDesc);

uint8_t *Standard_GetStatus(uint16_t Length);
RESULT Standard_ClearFeature(void);
void SetDeviceAddress(uint8_t);
void NOP_Process(void);

extern	DEVICE_PROP property;
extern	USER_STANDARD_REQUESTS standard_requests;
extern	DEVICE  Device_Table;
extern	DEVICE_INFO device_info;

extern __IO uint16_t ep0_rx_state;
extern __IO uint16_t ep0_tx_state;

#endif /* __USB_CORE_H */
