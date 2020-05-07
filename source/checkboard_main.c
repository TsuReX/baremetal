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
		GPIO_PortToggle(GPIO, 5, 3);
		sched_time = 0;

	}

	if (sched_time % 1000 == 0) {
		console_process();
	}
	++sched_time;

}

void rb_test(void)
{
	struct ring_buf rb;
	uint8_t src_buf[16] = {0,1,2,3,4,5,6,7,8,9,0xA,0xB,0xC,0xD,0xE,0xF};
	uint8_t dst_buf[16] = {0};

	rb_init_ring_buffer(&rb);

	uint32_t val = 0;

	val = rb_store_data(&rb, src_buf, sizeof(src_buf));
	d_print("store %ld\n\r", val);
	val = rb_get_data_size(&rb);
	d_print("size %ld\n\r", val);
	val = rb_get_data_fsize(&rb);
	d_print("fsize %ld\n\r", val);
	val = rb_get_data(&rb, dst_buf, ARRAY_SIZE(dst_buf));
	d_print("get %ld\n\r", val);
	val = rb_get_data_size(&rb);
	d_print("size %ld\n\r", val);
	val = rb_get_data_fsize(&rb);
	d_print("fsize %ld\n\r", val);

	val = rb_store_data(&rb, src_buf, sizeof(src_buf));
	d_print("store %ld\n\r", val);
	val = rb_get_data_size(&rb);
	d_print("size %ld\n\r", val);
	val = rb_get_data_fsize(&rb);
	d_print("fsize %ld\n\r", val);

	val = rb_throw_last_data(&rb, ARRAY_SIZE(dst_buf));
	d_print("throw %ld\n\r", val);
	val = rb_get_data_size(&rb);
	d_print("size %ld\n\r", val);
	val = rb_get_data_fsize(&rb);
	d_print("fsize %ld\n\r", val);

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
    rb_test();

    adc0_init();

    scheduler_init();

    //nmi_trigger();

    struct freq_meter_t fm9 = {.channel = fm_channel_9};
    fm_init(&fm9);


//    uint32_t local_counter = 0;
    uint32_t i = 0;
    while (1) {

//    	adc0_convert();
//        fm_clock_start_count(&fm9);
//    	mdelay(500U);
//        print("Counter chan 9 value: %d\n\r", fm_clock_stop_count(&fm9));
//    	print("\n\rADC0 values:\n\r");
//    	size_t channel = 0;
//    	for (; channel < 32; ++channel) {
//    		uint32_t value = adc0_get_value(channel);
//    		switch(channel){
//    		case 3:
//    			value = (value * 805) / 500;
//    			break;
//    		case 4:
//    			value = (value * 805) / 400;
//    			break;
//    		case 5:
//    			value = (value * 805 * 11) / 1000;
//    			break;
//    		}
//			print("ADC0 channel %d value %d\n\r", channel, value);
//    		print("TEST %d\t0123456789ABCDEF\r\n", channel);
//    		mdelay(1);
//    	}
    	print("1TEST %ld\t0123456789ABCDEF\r\n", i++);
    	print("2TEST %ld\t0123456789ABCDEF\r\n", i++);
    	print("3TEST %ld\t0123456789ABCDEF\r\n", i++);
    	mdelay(500);
//    	print("\033[15A");
//    	if (i >= 155)
//    		break;
    }

//fail:
	while (1) {
		fail_led_toggle();
		mdelay(500);
	}


    return 0;
}

