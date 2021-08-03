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

	printk(DEBUG, "nucleo_stm32f401ret6u\r\n");

	while(1) {

	}

	return 0;
}
