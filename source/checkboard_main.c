#include <stdint.h>
#include <stddef.h>

#include <config.h>
#include <time.h>
#include <control.h>
#include <scheduler.h>
#include <console.h>
#include <adc.h>
#include <context.h>

#include <fsl_adc.h>

#define SCHED_PERIOD	2000

//static volatile uint32_t counter = 0;
static volatile uint32_t sched_time = 0;
void scheduler_process(void)
{
	if (sched_time % SCHED_PERIOD == 0) {
		GPIO_PortToggle(GPIO, 5, 3);
		sched_time = 0;
//		d_print("Iteration : %d\n\r", counter++);
	}

	if (sched_time % 50 == 0) {
		console_process();
	}
	++sched_time;

//	adc0_convert();
}

int main(void)
{
	soc_config();

	board_config();

	power_on_hl1();
//    mdelay(2000U);
//
//    power_on_hl2();
//    mdelay(2000U);
//
    power_on_bp();
//    mdelay(10000U);
//
//    power_off_bp();

    console_init();

    adc0_init();

    scheduler_init();

    nmi_trigger();

    uint32_t local_counter = 0;
    while (1) {

    	adc0_convert();
    	mdelay(1000U);
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
			print("ADC0 channel %d value %d \n\r", channel, value);
    	}
//    	print("%c%c%c",0x8D,0x8D,0x8D);
    }
    return 0;
}

