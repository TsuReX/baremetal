#include <stdint.h>
#include <stddef.h>

#include <config.h>
#include <time.h>
#include <control.h>
#include <scheduler.h>
#include <console.h>
#include <adc.h>
#include <context.h>

#include <drivers.h>

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

static int sctimer_init(void)
{
    const uint32_t port4_pin0_config = (/* Pin is configured as SCT0_GPI1 */
                                         IOCON_FUNC5 |
                                         /* No addition pin function */
                                         IOCON_MODE_INACT |
                                         /* Enables digital function */
                                         IOCON_DIGITAL_EN |
                                         /* Input filter disabled */
                                         IOCON_INPFILT_OFF);

    IOCON_PinMuxSet(IOCON, 4U, 0U, port4_pin0_config);

    sctimer_config_t sctimer_config;
    SCTIMER_GetDefaultConfig(&sctimer_config);

    sctimer_config.clockMode = kSCTIMER_Sampled_ClockMode;
    sctimer_config.clockSelect = kSCTIMER_Clock_On_Rise_Input_7;
    sctimer_config.prescale_l = 250;

    SCTIMER_Init(SCT0, &sctimer_config);

    SCTIMER_StartTimer(SCT0, kSCTIMER_Counter_L);

	print("SCTimer initialized\n\r");
}

int main(void)
{
	soc_config();

	board_config();

	power_on_hl1();

    power_on_bp();

    console_init();

    adc0_init();

    scheduler_init();

    nmi_trigger();

    sctimer_init();

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
			print("ADC0 channel %d value %d\t\t\n\r", channel, value);
    	}
    	print("SCTimer %d\t\t\t\n\r", SCT0->COUNT);
    	print("\033[15A");
    }

    return 0;
}

