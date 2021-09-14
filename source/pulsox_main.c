#include <string.h>
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

	printk(DEBUG, "pulsox_stm32l4r9aii6u\r\n");

	while(1) {
		printk(DEBUG, "pulsox_stm32l4r9aii6u LOOP\r\n");
		mdelay(500);

	}
}
