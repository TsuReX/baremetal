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


	uint8_t sfdp[256];
	size_t sfdp_size = 16;
	memset(sfdp, 0xFF, sizeof(sfdp));
	spi_chip_activate(FLASH_CHANNEL);
	spi_flash_sfdp_read(sfdp, sfdp_size);
	spi_chip_deactivate(FLASH_CHANNEL);

	spi_flash_test();

	uint32_t sfdp_presence = 0;
	if (strncmp("SFDP", (const char*)sfdp, 4) == 0)
		sfdp_presence = 1;

	size_t i = 0;
	for (;i < sfdp_size; i++) {
		printk(INFO, "0x%02X\r\n", sfdp[i]);
	}

	if (sfdp_presence == 0) {
		while(1) {
			mdelay(500);
			LL_GPIO_TogglePin(GPIOC, LL_GPIO_PIN_13);
		}
	} else {

		LL_GPIO_ResetOutputPin(GPIOC, LL_GPIO_PIN_13);
		__WFI();
	}

	return 0;
}
