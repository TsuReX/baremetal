#include "time.h"
#include "drivers.h"
#include "console.h"
#include "spi.h"
#include "max3421e.h"
#include "kbmsusb.h"
#include "init.h"
#include "config.h"
#include "communication.h"

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

	/* Fire DEBUG_LED */
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
