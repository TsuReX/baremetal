#include <usb_endp.h>
#include <usb_lib.h>

void ep_in_handle (void)
{  
}

void ep_out_handle(void)
{
//	uint16_t USB_Rx_Cnt;
//	USB_Rx_Cnt = USB_SIL_Read(EP3_OUT, USB_Rx_Buffer);


	/* Enable the receive of data on EP3 */
	_SetEPRxValid(ENDP3);
}
