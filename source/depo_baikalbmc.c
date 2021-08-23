#include "time.h"
#include "drivers.h"
#include "console.h"
#include "init.h"
#include "config.h"
#include "debug.h"

int main(void)
{
	soc_init();

	board_init();

	mdelay(100);

	console_init();

	log_level_set(DEBUG);

	printk(DEBUG, "depo_baikalbmc\r\n");

	while(1) {
		printk(DEBUG, "cycle\r\n");
		LL_GPIO_SetOutputPin(GPIOA, LL_GPIO_PIN_6);
		LL_GPIO_ResetOutputPin(GPIOA, LL_GPIO_PIN_7);
		mdelay(500);
		LL_GPIO_ResetOutputPin(GPIOA, LL_GPIO_PIN_6);
		LL_GPIO_SetOutputPin(GPIOA, LL_GPIO_PIN_7);
		mdelay(500);
	}

	return 0;
}
