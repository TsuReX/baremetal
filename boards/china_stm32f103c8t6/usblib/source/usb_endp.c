#include "usb_endp.h"
#include "usb_lib.h"
#include "console.h"

void ep_in_handle (void)
{
	d_print("%s()\r\n",  __func__);
	_SetEPTxValid(ENDP1);
}

void ep_out_handle(void)
{
	d_print("%s()\r\n",  __func__);
	_SetEPRxValid(ENDP1);
}
