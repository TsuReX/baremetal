#include "drivers.h"
#include "console.h"
#include "spi.h"
#include "max3421e.h"
#include "config.h"
#include "utils.h"

int main(void)
{
	uint32_t i = 0x004FFFFF;
	for (; i != 0; --i) {
		__ISB();
	}
/*****************************************************************************************/

	soc_init();

	board_init();

	console_init();

	spi_init();
	/* 0. FDUPSPI */
	kb_usb_fullduplex_spi_set();

	/* 1. REVISON */
	kb_usb_revision_read();

	/* 2. CHIPRES, OSCOKIRQ */
	kb_usb_chip_reset();

	/* 2.1 GPOUT0 */
	kb_usb_power_enable();

	/* 3. HOST, DPPULLDN, DMPULLDN */
	kb_usb_mode_set();

	mdelay(7000);

	/* 4. BUSRST, SOFKAENAB, FRAMEIRQ */
	kb_usb_bus_reset();

	mdelay(300);

	/* 5. CONDETIRQ, SAMPLEBUS, JSTATUS, KTATUS */
	kb_usb_device_detect();
//	kb_usb_device_detection_cycle();

	/* 6. SETUP HS-IN */
	kb_usb_setup_set_address();
	kb_usb_setup_get_dev_descr();

	/* 7. BULK-IN */
/*****************************************************************************************/

/*****************************************************************************************/

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
