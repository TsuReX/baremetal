#include <stdint.h>
#include <stddef.h>

#include <config.h>
#include <time.h>
#include <control.h>
#include <scheduler.h>
#include <console.h>
#include <adc.h>
#include <context.h>
#include <freq_meter.h>
#include <drivers.h>
#include <ringbuf.h>

#define SCHED_PERIOD	2000

static volatile uint32_t sched_time = 0;
void scheduler_process(void)
{
	if (sched_time % SCHED_PERIOD == 0) {
		ok_led_toggle();
		sched_time = 0;
	}

	if (sched_time % 100 == 0) {
		console_start_transmission();
	}

	++sched_time;
}

int main(void)
{
	soc_config();

	board_config();

	ok_led_off();
	fail_led_off();
	mdelay(1000);
	ok_led_on();
	fail_led_on();
	mdelay(1000);

	bp_on();
	mdelay(1000);

//	if (bp_pwr_gd() != 0)
//		goto fail;

    console_init();

    adc0_init();

    scheduler_init();

    nmi_trigger();

    struct freq_meter_t fm9 = {.channel = fm_channel_9};
    fm_init(&fm9);

	while (1) {

		adc0_convert();
		fm_clock_start_count(&fm9);
		mdelay(1000U);
		print("Counter chan 9 value: %ld\n\r", fm_clock_stop_count(&fm9));
//		mdelay(100U);
		print("\n\rADC0 values:\n\r");
		size_t channel = 0;
		for (; channel < ADC0_CHANNEL_COUNT; ++channel) {
			uint32_t value = adc0_get_value(channel);
			switch(channel){
			case 3:
					value = (value * 805) / 500;
					break;
			case 4:
					value = (value * 805) / 400;
					break;
			case 5:
					value = (value * 805 * 11) / 1000;
					break;
			}
			print("ADC0 channel %d value %ld\n\r", channel, value);
		}
//		print("\033[15A");
		break;
	}
	print("Enter command: ");
	while(1) {
		mdelay(50);
		console_process();
	}
//fail:
	while (1) {
		fail_led_toggle();
		mdelay(500);

	}

    return 0;
}

