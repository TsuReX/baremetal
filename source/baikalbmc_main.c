#include <string.h>
#include "time.h"
#include "drivers.h"
#include "console.h"
#include "spi.h"
#include "init.h"
#include "config.h"
#include "debug.h"
#include "spi_flash.h"

int32_t power_on(void)
{
	LL_GPIO_SetOutputPin(DRVRST_PORT, DRVRST_PIN);
	mdelay(10);
	LL_GPIO_ResetOutputPin(CORE_CLOCK_PORT, CORE_CLOCK_PIN);
	mdelay(10);

	LL_GPIO_SetOutputPin(EN_VDD_PORT, EN_VDD_PIN);
	mdelay(100);
	uint32_t pg = LL_GPIO_IsInputPinSet(PG_VDD_PORT, PG_VDD_PIN);
	printk(DEBUG, "PG_VDD_PIN 0x%01lX\r\n", pg);
	if (!pg)
		return -1;

	LL_GPIO_SetOutputPin(EN_VLL_PORT, EN_VLL_PIN);
	mdelay(100);
	pg = LL_GPIO_IsInputPinSet(PG_VLL_PORT, PG_VLL_PIN);
	printk(DEBUG, "PG_VLL_PIN 0x%01lX\r\n", pg);
	if (!pg)
		return -2;

	LL_GPIO_SetOutputPin(EN_VDRAM_PORT, EN_VDRAM_PIN);
	mdelay(100);
	pg = LL_GPIO_IsInputPinSet(PG_VDRAM_PORT, PG_VDRAM_PIN);
	printk(DEBUG, "PG_VDRAM_PIN 0x%01lX\r\n", pg);
	if (!pg)
		return -3;

	LL_GPIO_SetOutputPin(EN_VL_PORT, EN_VL_PIN);
	mdelay(100);
	pg = LL_GPIO_IsInputPinSet(PG_VL_PORT, PG_VL_PIN);
	printk(DEBUG, "PG_VL_PIN 0x%01lX\r\n", pg);
	if (!pg)
		return -4;

	LL_GPIO_SetOutputPin(EN_VCORE_PORT, EN_VCORE_PIN);
	mdelay(100);
	pg = LL_GPIO_IsInputPinSet(PG_VCORE_PORT, PG_VCORE_PIN);
	printk(DEBUG, "PG_VCORE_PIN 0x%01lX\r\n", pg);
	if (!pg)
		return -5;

	LL_GPIO_SetOutputPin(CORE_CLOCK_PORT, CORE_CLOCK_PIN);
	mdelay(100);

	LL_GPIO_SetOutputPin(EN_AUX_PORT, EN_AUX_PIN);
	mdelay(100);
	pg = LL_GPIO_IsInputPinSet(PG_AUX_PORT, PG_AUX_PIN);
	printk(DEBUG, "PG_AUX_PIN 0x%01lX\r\n", pg);
//	if (!pg)
//		return -6;

	LL_GPIO_SetOutputPin(EN_VCC_PORT, EN_VCC_PIN);
	mdelay(100);
	pg = LL_GPIO_IsInputPinSet(PG_VCC_PORT, PG_VCC_PIN);
	printk(DEBUG, "PG_VCC_PIN 0x%01lX\r\n", pg);
	if (!pg)
		return -7;

	LL_GPIO_ResetOutputPin(DRVRST_PORT, DRVRST_PIN);

	return 0;
}

void power_off(void)
{
	LL_GPIO_SetOutputPin(DRVRST_PORT, DRVRST_PIN);

	LL_GPIO_ResetOutputPin(CORE_CLOCK_PORT, CORE_CLOCK_PIN);

	LL_GPIO_ResetOutputPin(EN_VCC_PORT, EN_VCC_PIN);

	LL_GPIO_ResetOutputPin(EN_AUX_PORT, EN_AUX_PIN);

	LL_GPIO_ResetOutputPin(EN_VDRAM_PORT, EN_VDRAM_PIN);

	LL_GPIO_ResetOutputPin(EN_VLL_PORT, EN_VLL_PIN);

	LL_GPIO_ResetOutputPin(EN_VCORE_PORT, EN_VCORE_PIN);

	LL_GPIO_ResetOutputPin(EN_VL_PORT, EN_VL_PIN);

	LL_GPIO_ResetOutputPin(EN_VDD_PORT, EN_VDD_PIN);
}

int main(void)
{
	soc_init();

	board_init();

	power_off();

	console_init();

	log_level_set(DEBUG);

	printk(DEBUG, "BAIKAL BMC\r\n");

	printk(DEBUG, "Start power up\r\n");
	int32_t ret_val = power_on();

	if (ret_val != 0) {
		printk(DEBUG, "Error step %ld\r\n", ret_val);
		power_off();
	}

	while(1) {
//		printk(DEBUG, "BAIKAL BMC LOOP\r\n");
		mdelay(500);

		LL_GPIO_ResetOutputPin(LED_PWR_PORT, LED_PWR_PIN);
		mdelay(100);
		LL_GPIO_ResetOutputPin(LED_STAT_PORT, LED_STAT_PIN);
		mdelay(100);
		LL_GPIO_ResetOutputPin(LED_AUX_PORT, LED_AUX_PIN);
		mdelay(100);
		LL_GPIO_SetOutputPin(LED_PWR_PORT, LED_PWR_PIN);
		mdelay(100);
		LL_GPIO_SetOutputPin(LED_STAT_PORT, LED_STAT_PIN);
		mdelay(100);
		LL_GPIO_SetOutputPin(LED_AUX_PORT, LED_AUX_PIN);

	}

	return 0;
}
