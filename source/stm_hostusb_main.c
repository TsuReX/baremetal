#include "time.h"
#include "drivers.h"
#include "console.h"
#include "spi.h"
#include "max3421e.h"
#include "kbmsusb.h"
#include "init.h"
#include "config.h"

int main(void)
{
	soc_init();

	board_init();

	console_init();

	spi_init();

	mdelay(500);

	/* Deassert RESET_N from MAX3421E_0. */
	LL_GPIO_SetOutputPin(GPIOA, LL_GPIO_PIN_3);

	/* Deassert RESET_N from MAX3421E_1. */
	LL_GPIO_SetOutputPin(GPIOA, LL_GPIO_PIN_4);

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
