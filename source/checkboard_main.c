#include <stdint.h>
#include <stddef.h>

#include <config.h>
#include <time.h>
#include <control.h>
#include <scheduler.h>

#define SCHED_PERIOD	2000

static volatile uint32_t sched_time = 0;

void scheduler_process(void)
{
	if (++sched_time % SCHED_PERIOD == 0){
		GPIO_PortToggle(GPIO, 5, 3);
		sched_time = 0;
	}
}

int main(void)
{
	soc_config();

	board_config();

	power_on_hl1();
    mdelay(2000U);

    power_on_hl2();
    mdelay(2000U);

    power_on_bp();
    mdelay(10000U);

    power_off_bp();

    scheduler_init();

}

