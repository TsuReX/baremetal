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

    scheduler_init();

    adc0_init();

    nmi_trigger();

    uint32_t local_counter = 0;
    while (1) {
//    	ADC_DoSoftwareTriggerConvSeqA(ADC0);
//    	adc_result_info_t adc_result;
//
    	mdelay(1000U);
    	print("Local iteration : %d\n\r", local_counter++);
//
//    	ADC_GetChannelConversionResult(ADC0, 4, &adc_result);
//    	print("ADC0_3 value %d mV\n\r", adc_result.result);
//
//    	ADC_GetChannelConversionResult(ADC0, 4, &adc_result);
//    	print("ADC0_4 value %d mV\n\r", adc_result.result);
//
//    	ADC_GetChannelConversionResult(ADC0, 3, &adc_result);
//    	print("ADC0_5 value %d mV\n\r", adc_result.result);

    	print("ADC0_3 value %d mV\n\r", adc0_3_getval() * 805 / 500);
    	print("ADC0_4 value %d mV\n\r", adc0_4_getval() * 805 / 400);
    	print("ADC0_5 value %d mV\n\r", adc0_5_getval() * 805 * 11 / 1000);
    }
    return 0;
}

