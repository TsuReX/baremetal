#include "usb_endp.h"
#include "usb_lib.h"
#include "usb_desc.h"
#include "console.h"
#include "debug.h"

//static uint32_t keyboard_cntr = 1;
//static uint32_t mouse_cntr = 1;

void keyboard_ep_in_handle (void)
{
//	printk(DEBUG, "%s()\r\n",  __func__);

//	keyboard.keycode_1 = 'Q';

//	copy_to_usb((uint8_t*)&keyboard, _GetEPTxAddr(ENDP1), sizeof(struct keyboard_state));

//	if (keyboard_cntr < 4) {
//	_SetEPTxCount(ENDP1, EP1_MAX_PACKET_SIZE);
//	_SetEPTxStatus(ENDP1, EP_TX_VALID);
//	} else {
//		_SetEPTxStatus(ENDP1, EP_TX_NAK);
//	}
}

void mouse_ep_in_handle (void)
{
//	printk(DEBUG, "%s()\r\n",  __func__);

//	mouse.x_displacement = mouse_cntr;
//	mouse.y_displacement = mouse_cntr++ + 100;
//
//	copy_to_usb((uint8_t*)&mouse, _GetEPTxAddr(ENDP2), sizeof(struct mouse_state));
//	if (mouse_cntr++ < 3) {
//	_SetEPTxCount(ENDP2, EP2_MAX_PACKET_SIZE);
//	_SetEPTxStatus(ENDP2, EP_TX_VALID);
//	} else {
//		_SetEPTxStatus(ENDP1, EP_TX_NAK);
//	}
}

void keyboard_ep_out_handle (void)
{
	printk(DEBUG, "%s()\r\n",  __func__);
	_SetEPRxValid(ENDP1);
}

void mouse_ep_out_handle (void)
{
	printk(DEBUG, "%s()\r\n",  __func__);
	_SetEPRxValid(ENDP2);
}
