#include "time.h"
#include "drivers.h"
#include "console.h"
#include "spi.h"
#include "max3421e.h"
#include "kbmsusb.h"
#include "init.h"
#include "config.h"
#include "debug.h"
#include "spi_flash.h"

int main(void)
{
	soc_init();

	board_init();

	console_init();

	log_level_set(DEBUG);

	spi_init();

	mdelay(50);

	/* Deassert RESET_N from MAX3421E_0. */
	LL_GPIO_SetOutputPin(GPIOA, LL_GPIO_PIN_3);

	/* Deassert RESET_N from MAX3421E_1. */
	LL_GPIO_SetOutputPin(GPIOA, LL_GPIO_PIN_4);

	spi_usb_transmission_start();
	while(1)
	{
		udelay(500000);
		LL_GPIO_SetOutputPin(GPIOC, LL_GPIO_PIN_13);
		udelay(500000);
		LL_GPIO_ResetOutputPin(GPIOC, LL_GPIO_PIN_13);
	}

	return 0;
}
