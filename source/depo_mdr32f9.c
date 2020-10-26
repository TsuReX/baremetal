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
	max3421e_fullduplex_spi_set();

	/* 1. REVISON */
	max3421e_rev_print();

	/* 2. CHIPRES, OSCOKIRQ */
	max3421e_chip_reset();

	/* 2.1 GPOUT0 */
	kb_ms_power_on();

	/* 3. HOST, DPPULLDN, DMPULLDN */
	max3421e_master_mode_set();

	mdelay(7000);

	/* 4. BUSRST, SOFKAENAB, FRAMEIRQ */
	max3421e_usb_bus_reset();

	mdelay(300);

	/* 5. CONDETIRQ, SAMPLEBUS, JSTATUS, KTATUS */
	max3421e_usb_device_detect();
//	kb_usb_device_detection_cycle();

	/* 6. SETUP HS-IN */
	max3421e_usb_device_set_address();
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
