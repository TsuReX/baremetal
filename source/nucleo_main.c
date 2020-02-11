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

static inline uint16_t swap_bytes_in_word(uint16_t word)
{
	uint8_t tmp = ((uint8_t *)&word)[0];
	((uint8_t *)&word)[0] = ((uint8_t *)&word)[1];
	((uint8_t *)&word)[1] = tmp;
	return word;
}

void scheduler_process(void)
{
	LL_GPIO_TogglePin(GPIOB, LL_GPIO_PIN_3);
	console_process();
}

void ds3231m_read_regs(void)
{
	print("%s()\r\n", __func__);

	/* DS3231M */
	//uint8_t chip_addr = 0xD0;
	//uint8_t reg_addr = 0x00;

}

void ina3221_read_regs(void)
{

	print("%s()\r\n", __func__);

	/* INA3221 */
	uint8_t		ina3221_addr = 0x80;
	uint8_t		reg_addr = 0x00; /* 0x00 - 0x11, 0xFE, 0xFF */
	uint16_t	reg_val = 0xFFFF;

	int32_t ret_val = 0;
	for (;reg_addr <= 0x11; ++reg_addr) {
		ret_val = i2c_read(ina3221_addr, reg_addr, (uint8_t *)&reg_val, sizeof(reg_val));

		if (ret_val != 2)
			print("i2c_read(chip_addr=0x%02X, reg_addr=0x%02X, buffer_size=0x%02X)=%d\r\n", ina3221_addr, reg_addr, sizeof(reg_val), ret_val);
		else
			print("reg_addr=0x%02X, reg_val=0x%04X\r\n", reg_addr, swap_bytes_in_word(reg_val));
		LL_mDelay(DELAY_50_MS);
	}
	reg_addr = 0xFE;
	ret_val = i2c_read(ina3221_addr, reg_addr, (uint8_t *)&reg_val, sizeof(reg_val));

	if (ret_val != 2)
		print("i2c_read(chip_addr=0x%02X, reg_addr=0x%02X, buffer_size=0x%02X)=%d\r\n", ina3221_addr, reg_addr, sizeof(reg_val), ret_val);
	else
		print("reg_addr=0x%02X, reg_val=0x%04X\r\n", reg_addr, swap_bytes_in_word(reg_val));

	reg_addr = 0xFF;
	ret_val = i2c_read(ina3221_addr, reg_addr, (uint8_t *)&reg_val, sizeof(reg_val));

	if (ret_val != 2)
		print("i2c_read(chip_addr=0x%02X, reg_addr=0x%02X, buffer_size=0x%02X)=%d\r\n", ina3221_addr, reg_addr, sizeof(reg_val), ret_val);
	else
		print("reg_addr=0x%02X, reg_val=0x%04X\r\n", reg_addr, swap_bytes_in_word(reg_val));

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

	ina3221_read_regs();

	uint32_t i = 0;
	while (1) {
		LL_mDelay(DELAY_500_MS * 2);
		print("Main thread iteration %d\r", i++);
	}
}
