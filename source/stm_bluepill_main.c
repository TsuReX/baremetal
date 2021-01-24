#include <string.h>
#include "time.h"
#include "drivers.h"
#include "console.h"
#include "spi.h"
#include "init.h"
#include "config.h"
#include "debug.h"
#include "spi_flash.h"

int main(void)
{
	soc_init();

	board_init();

	console_init();

	spi_init();

	mdelay(500);

//	spi_flash_test();
	uint8_t sfdp[256];
	memset(sfdp, 0xFF, sizeof(sfdp));
	spi_chip_activate(FLASH_CHANNEL);
	spi_flash_sfdp_read(sfdp, 16);
	spi_chip_deactivate(FLASH_CHANNEL);

	while(1)
	{
		mdelay(1500);
		LL_GPIO_SetOutputPin(GPIOC, LL_GPIO_PIN_13);
		mdelay(1500);
		LL_GPIO_ResetOutputPin(GPIOC, LL_GPIO_PIN_13);
	}

	return 0;
}
