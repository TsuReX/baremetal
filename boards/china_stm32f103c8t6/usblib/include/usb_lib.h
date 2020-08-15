#ifndef __USB_LIB_H
#define __USB_LIB_H

//#include "usb_core.h"
#include "usb_def.h"
#include "usb_mem.h"
#include "usb_regs.h"
#include "usb_sil.h"
#include "usb_type.h"
#include "usb_conf.h"

typedef enum _RESUME_STATE {
	RESUME_EXTERNAL,
	RESUME_INTERNAL,
	RESUME_LATER,
	RESUME_WAIT,
	RESUME_START,
	RESUME_ON,
	RESUME_OFF,
	RESUME_ESOF
} RESUME_STATE;

typedef enum _DEVICE_STATE {
	UNCONNECTED,
	ATTACHED,
	POWERED,
	SUSPENDED,
	ADDRESSED,
	CONFIGURED
} DEVICE_STATE;

typedef enum _RESULT {
	USB_SUCCESS = 0,    /* Process successfully */
	USB_ERROR,
	USB_UNSUPPORT,
	USB_NOT_READY       /* The process has not been finished, endpoint will be
                         NAK to further request */
} RESULT;

void suspend(void);
void Resume_Init(void);
void resume(RESUME_STATE eResumeSetVal);
RESULT PowerOn(void);
RESULT PowerOff(void);

extern  __IO uint32_t bDeviceState; /* USB device status */
//extern __IO bool fSuspendEnabled;  /* true when suspend is possible */

#endif /* __USB_LIB_H */
