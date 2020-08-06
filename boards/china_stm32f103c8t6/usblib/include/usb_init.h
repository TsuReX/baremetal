/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USB_INIT_H
#define __USB_INIT_H

#include <usb_lib.h>

void usb_init(void);

/* External variables --------------------------------------------------------*/
/*  The number of current endpoint, it will be used to specify an endpoint */
extern uint8_t	ep_index;
/*  The number of current device, it is an index to the Device_Table */
/*extern uint8_t	Device_no; */
/*  Points to the DEVICE_INFO structure of current device */
/*  The purpose of this register is to speed up the execution */
extern DEVICE_INFO*	usb_device_info;
/*  Points to the DEVICE_PROP structure of current device */
/*  The purpose of this register is to speed up the execution */
extern DEVICE_PROP*	usb_device_property;
/*  Temporary save the state of Rx & Tx status. */
/*  Whenever the Rx or Tx state is changed, its value is saved */
/*  in this variable first and will be set to the EPRB or EPRA */
/*  at the end of interrupt process */
extern USER_STANDARD_REQUESTS *usb_standard_requests;

extern uint16_t	SaveState ;
extern uint16_t wInterrupt_Mask;

#endif /* __USB_INIT_H */
