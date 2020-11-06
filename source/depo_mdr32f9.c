#include "drivers.h"
#include "console.h"
#include "spi.h"
#include "max3421e.h"
#include "kbmsusb.h"
#include "config.h"
#include "utils.h"
#include "communication.h"

void max3421e_chip_activate(uint32_t chip_num)
{
	switch(chip_num) {
		case KEYBOARD_CHANNEL:
			PORT_ResetBits(MDR_PORTE, PORT_Pin_0);
			break;

		case MOUSE_CHANNEL:
			PORT_ResetBits(MDR_PORTB, PORT_Pin_8);
			break;
	};
}

void max3421e_chip_deactivate(uint32_t chip_num)
{
	switch(chip_num) {
		case KEYBOARD_CHANNEL:
			PORT_SetBits(MDR_PORTE, PORT_Pin_0);
			break;

		case MOUSE_CHANNEL:
			PORT_SetBits(MDR_PORTB, PORT_Pin_8);
			break;
	};
}

int main(void)
{
	uint32_t i = 0x004FFFFF;
	for (; i != 0; --i) {
		__ISB();
	}
/*****************************************************************************************/
	soc_init();
	board_init();
	mdelay(100);
//	console_init();
	comm_init(0, 0);
	comm_start();

	spi_init();
	PORT_SetBits(MDR_PORTD, PORT_Pin_7);
	spi_usb_transmission_start();

/*****************************************************************************************/
	PORT_SetBits(MDR_PORTD, PORT_Pin_7);
	while(1)
	{
		PORT_SetBits(MDR_PORTD, PORT_Pin_7);
		mdelay(500);

		PORT_ResetBits(MDR_PORTD, PORT_Pin_7);
		mdelay(500);

	}

	return 0;
}
