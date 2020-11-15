#include "drivers.h"
#include "console.h"
#include "spi.h"
#include "max3421e.h"
#include "kbmsusb.h"
#include "init.h"
#include "config.h"
#include "delay.h"

int main(void)
{
	soc_init();

	board_init();

	console_init();

	spi_init();

	spi_usb_transmission_start();

	while(1)
	{
		mdelay(500);
		LL_GPIO_SetOutputPin(GPIOC, LL_GPIO_PIN_13);
		mdelay(500);
		LL_GPIO_ResetOutputPin(GPIOC, LL_GPIO_PIN_13);
	}

	return 0;
}
