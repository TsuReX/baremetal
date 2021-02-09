#include "time.h"
#include "drivers.h"
#include "console.h"
#include "spi.h"
#include "max3421e.h"
#include "kbmsusb.h"
#include "init.h"
#include "config.h"
#include "communication.h"

void kb_ms_power_on(void);
int32_t device_detect_init(uint32_t usb_channel, uint8_t usb_dev_addr);

int main(void)
{
/*****************************************************************************************/
	soc_init();
	board_init();
	mdelay(100);
//	console_init();
	comm_init(0, 0);
	comm_start();

//	spi_init();

	/* Fire DEBUG_LED */
	PORT_SetBits(MDR_PORTD, PORT_Pin_7);

//	mdelay(2000);
//	max3421e_fullduplex_spi_set(KEYBOARD_CHANNEL);
//	max3421e_chip_reset(KEYBOARD_CHANNEL);
//	kb_ms_power_on();
//	device_detect_init(KEYBOARD_CHANNEL, 0x34);

//	spi_usb_transmission_start();

/*****************************************************************************************/
	PORT_SetBits(MDR_PORTD, PORT_Pin_7);
	while(1)
	{
#if 0
		max3421e_chip_activate(KEYBOARD_CHANNEL);
		mdelay(1);
		max3421e_rev_read();
		max3421e_chip_deactivate(KEYBOARD_CHANNEL);

		max3421e_chip_activate(MOUSE_CHANNEL);
		mdelay(1);
		max3421e_rev_read();
		max3421e_chip_deactivate(MOUSE_CHANNEL);
#endif

#if 1
		PORT_SetBits(MDR_PORTE, PORT_Pin_0);
		PORT_SetBits(MDR_PORTB, PORT_Pin_8);
		PORT_SetBits(MDR_PORTF, PORT_Pin_0);
		PORT_SetBits(MDR_PORTF, PORT_Pin_1);
		PORT_SetBits(MDR_PORTF, PORT_Pin_3);
#endif

#if 1
		PORT_SetBits(MDR_PORTC, PORT_Pin_0);
		PORT_ResetBits(MDR_PORTC, PORT_Pin_1);
#endif
		PORT_SetBits(MDR_PORTD, PORT_Pin_7);
		mdelay(50);
#if 1
		PORT_ResetBits(MDR_PORTE, PORT_Pin_0);
		PORT_ResetBits(MDR_PORTB, PORT_Pin_8);
		PORT_ResetBits(MDR_PORTF, PORT_Pin_0);
		PORT_ResetBits(MDR_PORTF, PORT_Pin_1);
		PORT_ResetBits(MDR_PORTF, PORT_Pin_3);
#endif

#if 1
		PORT_ResetBits(MDR_PORTC, PORT_Pin_0);
		PORT_SetBits(MDR_PORTC, PORT_Pin_1);
#endif
		PORT_ResetBits(MDR_PORTD, PORT_Pin_7);
		mdelay(50);
	}

	return 0;
}
