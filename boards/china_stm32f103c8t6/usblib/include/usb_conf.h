#ifndef __USB_CONF_H
#define __USB_CONF_H

/* EP_NUM defines how many endpoints are used by the device */

#define EP_COUNT			(4)

/* buffer table base address */
/* buffer table base address */
#define BTABLE_ADDRESS      (0x00)

/* EP0  */
/* rx/tx buffer base address */
#define ENDP0_RXADDR        (0x18)
#define ENDP0_TXADDR        (0x58)

/* EP1  */
/* tx buffer base address */
#define ENDP1_TXADDR        (0x100)
#define ENDP1_RXADDR        (0x140)

#define IMR_MSK (CNTR_CTRM | CNTR_WKUPM | CNTR_SUSPM | CNTR_ERRM | CNTR_SOFM | CNTR_ESOFM | CNTR_RESETM)

#endif /* __USB_CONF_H */
