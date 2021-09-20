#ifndef __USB_DEVICE__H__
#define __USB_DEVICE__H__

#ifdef __cplusplus
 extern "C" {
#endif

#include "stm32f4xx.h"
#include "stm32f4xx_hal.h"
#include "usbd_def.h"

/** USB Device initialization function. */
void usb_init(void);

#ifdef __cplusplus
}
#endif

#endif /* __USB_DEVICE__H__ */

