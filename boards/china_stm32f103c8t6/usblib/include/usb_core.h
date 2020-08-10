/**
  ******************************************************************************
  * @file    usb_core.h
  * @author  MCD Application Team
  * @version V4.0.0
  * @date    28-August-2012
  * @brief   Standard protocol processing functions prototypes
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2012 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */


/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USB_CORE_H
#define __USB_CORE_H

#include <stdint.h>
#include <drivers.h>

/* Includes ------------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
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
/* All the request process routines return a value of this type
   If the return value is not SUCCESS or NOT_READY,
   the software will STALL the correspond endpoint */
typedef enum _RESULT {
  USB_SUCCESS = 0,    /* Process successfully */
  USB_ERROR,
  USB_UNSUPPORT,
  USB_NOT_READY       /* The process has not been finished, endpoint will be
                         NAK to further request */
} RESULT;

typedef struct _ENDPOINT_INFO {
	/* When send data out of the device,
	CopyData() is used to get data buffer 'Length' bytes data
	if Length is 0,
	CopyData() returns the total length of the data
	if the request is not supported, returns 0
	(NEW Feature )
	 if CopyData() returns -1, the calling routine should not proceed
	 further and will resume the SETUP process by the class device
	if Length is not 0,
	CopyData() returns a pointer to indicate the data location
	Usb_wLength is the data remain to be sent,
	Usb_wOffset is the Offset of original data
	When receive data from the host,
	CopyData() is used to get user data buffer which is capable
	of Length bytes data to copy data from the endpoint buffer.
	if Length is 0,
	CopyData() returns the available data length,
	if Length is not 0,
	CopyData() returns user buffer address
	Usb_rLength is the data remain to be received,
	Usb_rPointer is the Offset of data buffer
	*/
	uint16_t  remaining_data_size;
	uint16_t  data_buffer_offset;
	uint16_t  single_transfer_size;
	uint8_t   *(*data_copy)(uint16_t Length);
} ENDPOINT_INFO;

/*-*-*-*-*-*-*-*-*-*-*-* Definitions for device level -*-*-*-*-*-*-*-*-*-*-*-*/

typedef struct _DEVICE {
	uint8_t Total_Endpoint;     /* Number of endpoints that are used */
	uint8_t Total_Configuration;/* Number of configuration available */
} DEVICE;

typedef union {
	uint16_t w;

	struct BW {
		uint8_t bb1;
		uint8_t bb0;
	} bw;

} uint16_t_uint8_t;

typedef struct _DEVICE_INFO {
	uint8_t				bm_request_type;	/* bmRequestType */
	uint8_t				b_request;			/* bRequest */
	uint16_t_uint8_t	w_value;			/* wValue */
	uint16_t_uint8_t	w_index;			/* wIndex */
	uint16_t_uint8_t	w_length;			/* wLength */

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

	/* Procedure of process on setup stage of a class specified request with data stage */
	/* All class specified requests with data stage are processed in Class_Data_Setup
	Class_Data_Setup()
	responses to check all special requests and fills ENDPOINT_INFO
	according to the request
	If IN tokens are expected, then wLength & wOffset will be filled
	with the total transferring bytes and the starting position
	If OUT tokens are expected, then rLength & rOffset will be filled
	with the total expected bytes and the starting position in the buffer

	If the request is valid, Class_Data_Setup returns SUCCESS, else UNSUPPORT

	CAUTION:
	Since GET_CONFIGURATION & GET_INTERFACE are highly related to
	the individual classes, they will be checked and processed here.
	*/
	RESULT (*class_setup_with_data_process)(uint8_t RequestNo);

	/* Procedure of process on setup stage of a class specified request without data stage */
	/* All class specified requests without data stage are processed in Class_NoData_Setup
	Class_NoData_Setup
	responses to check all special requests and perform the request

	CAUTION:
	Since SET_CONFIGURATION & SET_INTERFACE are highly related to
	the individual classes, they will be checked and processed here.
	*/
	RESULT (*class_setup_without_data_process)(uint8_t RequestNo);

	/*Class_Get_Interface_Setting
	This function is used by the file usb_core.c to test if the selected Interface
	and Alternate Setting (uint8_t Interface, uint8_t AlternateSetting) are supported by
	the application.
	This function is writing by user. It should return "SUCCESS" if the Interface
	and Alternate Setting are supported by the application or "UNSUPPORT" if they
	are not supported. */

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

#define Usb_rLength Usb_wLength
#define Usb_rOffset Usb_wOffset

#define USBwValue		w_value.w
#define USBwValue0		w_value.bw.bb0
#define USBwValue1		w_value.bw.bb1
#define USBwIndex		w_index.w
#define USBwIndex0		w_index.bw.bb0
#define USBwIndex1		w_index.bw.bb1
#define USBwLength		w_length.w
#define USBwLength0		w_length.bw.bb0
#define USBwLength1		w_length.bw.bb1

/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
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

/* cells saving status during interrupt servicing */
extern __IO uint16_t ep0_rx_state;
extern __IO uint16_t ep0_tx_state;

#endif /* __USB_CORE_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
