#include <stdint.h>
#include <stddef.h>

#include <config.h>
#include <time.h>
#include <control.h>
#include <scheduler.h>
#include <console.h>

#define SCHED_PERIOD	2000

//static volatile uint32_t counter = 0;
static volatile uint32_t sched_time = 0;
void scheduler_process(void)
{
//	if (sched_time % SCHED_PERIOD == 0) {
//		GPIO_PortToggle(GPIO, 5, 3);
//		sched_time = 0;
//		d_print("Iteration : %d\n\r", counter++);
//	}

	if (sched_time % 50 == 0) {
		console_process();
	}
	++sched_time;
}

int main(void)
{
	soc_config();

	board_config();

//	power_on_hl1();
//    mdelay(2000U);
//
//    power_on_hl2();
//    mdelay(2000U);
//
//    power_on_bp();
//    mdelay(10000U);
//
//    power_off_bp();

    console_init();

    scheduler_init();

    uint32_t local_counter = 0;
    while (1) {
    	print("Local iteration : %d\n\r", local_counter++);
    	mdelay(1000U);
    }
    return 0;
}

