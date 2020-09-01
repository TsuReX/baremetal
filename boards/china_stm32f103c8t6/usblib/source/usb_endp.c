#include "usb_endp.h"
#include "usb_lib.h"
#include "console.h"

struct keyboard_state {
	uint8_t modifiers;
	uint8_t reserved;
	uint8_t keycode_1;
	uint8_t keycode_2;
	uint8_t keycode_3;
	uint8_t keycode_4;
	uint8_t keycode_5;
	uint8_t keycode_6;
} ;

struct mouse_state {
	uint8_t buttons;
	uint8_t x_displacement;
	uint8_t y_displacement;
	uint8_t reserved[5];
} ;

struct keyboard_state keyboard = {
//		0x18,
//		0x27,
//		0x36,
//		0x45,
//		0x54,
//		0x63,
//		0x72,
//		0x81,
		0,
		0,
		65,
		0,
		0,
		0,
		0,
		0
};

struct mouse_state mouse = {
		0x90,
		0xAF,
		0xBE,
		{0xCD, 0xDC, 0xEB, 0xFA, 0x09}
};

static uint32_t keyboard_cntr = 1;
static uint32_t mouse_cntr = 1;

void keyboard_ep_in_handle (void)
{
	d_print("%s()\r\n",  __func__);

	keyboard.keycode_1 = keyboard_cntr;

	copy_to_usb((uint8_t*)&keyboard, _GetEPTxAddr(ENDP1), sizeof(struct keyboard_state));

	if (keyboard_cntr++ < 4) {
		_SetEPTxStatus(ENDP1, EP_TX_VALID);
	} else {
		_SetEPTxStatus(ENDP1, EP_TX_NAK);
	}
}

void mouse_ep_in_handle (void)
{
	d_print("%s()\r\n",  __func__);

	mouse.x_displacement = mouse_cntr;
	mouse.y_displacement = mouse_cntr + 100;

	copy_to_usb((uint8_t*)&mouse, _GetEPTxAddr(ENDP2), sizeof(struct mouse_state));

	if (mouse_cntr++ < 3) {
		_SetEPTxStatus(ENDP1, EP_TX_VALID);
	} else {
		_SetEPTxStatus(ENDP1, EP_TX_NAK);
	}
}

void keyboard_ep_out_handle (void)
{
	d_print("%s()\r\n",  __func__);
	_SetEPRxValid(ENDP1);
}

void mouse_ep_out_handle (void)
{
	d_print("%s()\r\n",  __func__);
	_SetEPRxValid(ENDP2);
}
