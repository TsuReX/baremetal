#include <string.h>
#include "time.h"
#include "drivers.h"
#include "console.h"
#include "spi.h"
#include "init.h"
#include "config.h"
#include "debug.h"
#include "spi_flash.h"

void max7219_check(void)
{
	mdelay(1000);
	uint16_t	cmd = 0x0;

	cmd = 0x0701;
	spi_chip_activate(FLASH_CHANNEL);
	spi_dma_data_send((uint8_t*)&cmd, sizeof(cmd));
	spi_chip_deactivate(FLASH_CHANNEL);

	cmd = 0x0F0A;
	spi_chip_activate(FLASH_CHANNEL);
	spi_dma_data_send((uint8_t*)&cmd, sizeof(cmd));
	spi_chip_deactivate(FLASH_CHANNEL);

	cmd = 0xFF09;
	spi_chip_activate(FLASH_CHANNEL);
	spi_dma_data_send((uint8_t*)&cmd, sizeof(cmd));
	spi_chip_deactivate(FLASH_CHANNEL);

	cmd = 0x0F0B;
	spi_chip_activate(FLASH_CHANNEL);
	spi_dma_data_send((uint8_t*)&cmd, sizeof(cmd));
	spi_chip_deactivate(FLASH_CHANNEL);

	cmd = 0x010C;
	spi_chip_activate(FLASH_CHANNEL);
	spi_dma_data_send((uint8_t*)&cmd, sizeof(cmd));
	spi_chip_deactivate(FLASH_CHANNEL);

//	mdelay(2000);
	uint8_t val = 0;
	while (1) {
		cmd = (val << 8) | 0x1;
		spi_chip_activate(FLASH_CHANNEL);
		spi_dma_data_send((uint8_t*)&cmd, sizeof(cmd));
		spi_chip_deactivate(FLASH_CHANNEL);

		cmd = (val << 8) | 0x2;
		spi_chip_activate(FLASH_CHANNEL);
		spi_dma_data_send((uint8_t*)&cmd, sizeof(cmd));
		spi_chip_deactivate(FLASH_CHANNEL);

		cmd = (val << 8) | 0x3;
		spi_chip_activate(FLASH_CHANNEL);
		spi_dma_data_send((uint8_t*)&cmd, sizeof(cmd));
		spi_chip_deactivate(FLASH_CHANNEL);

		cmd = (val << 8) | 0x4;
		spi_chip_activate(FLASH_CHANNEL);
		spi_dma_data_send((uint8_t*)&cmd, sizeof(cmd));
		spi_chip_deactivate(FLASH_CHANNEL);

		cmd = (val << 8) | 0x5;
		spi_chip_activate(FLASH_CHANNEL);
		spi_dma_data_send((uint8_t*)&cmd, sizeof(cmd));
		spi_chip_deactivate(FLASH_CHANNEL);

		cmd = (val << 8) | 0x6;
		spi_chip_activate(FLASH_CHANNEL);
		spi_dma_data_send((uint8_t*)&cmd, sizeof(cmd));
		spi_chip_deactivate(FLASH_CHANNEL);

		cmd = (val << 8) | 0x7;
		spi_chip_activate(FLASH_CHANNEL);
		spi_dma_data_send((uint8_t*)&cmd, sizeof(cmd));
		spi_chip_deactivate(FLASH_CHANNEL);

		cmd = (val << 8) | 0x8;
		spi_chip_activate(FLASH_CHANNEL);
		spi_dma_data_send((uint8_t*)&cmd, sizeof(cmd));
		spi_chip_deactivate(FLASH_CHANNEL);

		val = (val + 1) % 9;

		mdelay(500);
	}

}

void sfdp_check(void)
{
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
}
#define FUNC 1
int main(void)
{
	soc_init();

	board_init();

	console_init();

	spi_init();

#if (FUNC == 0)
	sfdp_check();
#elif (FUNC == 1)
	max7219_check();
	__WFI();
#endif

	return 0;
}
