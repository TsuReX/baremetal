/**
 * @file	src/main.c
 *
 * @brief	Объявление точки вхождения в C-код.
 *
 * @author	Vasily Yurchenko <vasily.v.yurchenko@yandex.ru>
 */

#include "i2c.h"
#include "console.h"
#include "config.h"
#include "scheduler.h"
#include "drivers.h"

void scheduler_process(void)
{
	LL_GPIO_TogglePin(GPIOB, LL_GPIO_PIN_3);
	console_process();
}

void i2c_test(void) {

	print("%s()", __func__);

	/* INA3221 */
	uint8_t chip_addr = 0x80;
	uint8_t reg_addr = 0xFF; /*0xFE*/

	/* DS3231M */
	//uint8_t chip_addr = 0xD0;
	//uint8_t reg_addr = 0x00;

	uint8_t buffer[1] = {0xFF};

	int32_t ret_val = i2c_read(chip_addr, reg_addr, buffer, sizeof(buffer));

	print("i2c_read(chip_addr=0x%02X, reg_addr=0x%02X, buffer_size=0x%02X)=%d\r\n", chip_addr, reg_addr, sizeof(buffer), ret_val);
}

/**
 * @brief	C-code entry point.
 *
 * @retval	0 in case of success, otherwise error code
 */
int main(void)
{
	/** Configure internal subsystems of SoC. */
	soc_config();

	/** Configure board's peripherals. */
	board_config();

	console_init();

	scheduler_init();

	i2c_init();

	i2c_test();

	uint32_t i = 0;
	while (1) {
		LL_mDelay(DELAY_500_MS * 2);
		print("Main thread iteration %d\r", i++);
	}
}
