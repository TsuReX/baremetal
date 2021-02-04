#include <string.h>
#include "time.h"
#include "drivers.h"
#include "console.h"
#include "spi.h"
#include "init.h"
#include "config.h"
#include "debug.h"
#include "spi_flash.h"

#define DECMODE		0x0009
#define INTENSITY	0x000A
#define SCANLIM		0x000B
#define SHUTDOWN	0x000C

void max7219_digit_value_set(size_t dig_num, uint32_t value)
{
	dig_num = (dig_num & 0x7) + 1; /* (0-7)->(1-8) */
	value &= 0xF;

	uint16_t cmd = (value << 8) | dig_num;
	spi_chip_activate(0);
	spi_dma_data_send((uint8_t*)&cmd, sizeof(cmd));
	spi_chip_deactivate(0);

}

void max7219_init(void)
{
	uint16_t	cmd = 0x0;

	cmd = (0x0F << 8) | INTENSITY;
	spi_chip_activate(FLASH_CHANNEL);
	spi_dma_data_send((uint8_t*)&cmd, sizeof(cmd));
	spi_chip_deactivate(FLASH_CHANNEL);

	cmd = (0xFF << 8) | DECMODE;
	spi_chip_activate(FLASH_CHANNEL);
	spi_dma_data_send((uint8_t*)&cmd, sizeof(cmd));
	spi_chip_deactivate(FLASH_CHANNEL);

	cmd = (0x0F << 8) | SCANLIM;
	spi_chip_activate(FLASH_CHANNEL);
	spi_dma_data_send((uint8_t*)&cmd, sizeof(cmd));
	spi_chip_deactivate(FLASH_CHANNEL);

	cmd = (0x01 << 8) | SHUTDOWN;
	spi_chip_activate(FLASH_CHANNEL);
	spi_dma_data_send((uint8_t*)&cmd, sizeof(cmd));
	spi_chip_deactivate(FLASH_CHANNEL);

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

	log_level_set(DEBUG);

	spi_init();

#if (FUNC == 0)
	sfdp_check();
#elif (FUNC == 1)
	mdelay(1000);
	max7219_init();
	size_t dig_num = 0;
	size_t value = 0;
	while (1) {
		for (dig_num = 0; dig_num < 8; ++dig_num) {
			max7219_digit_value_set(dig_num, dig_num + value);
		}
		value++;
		mdelay(200);
	}
#endif

	return 0;
}
