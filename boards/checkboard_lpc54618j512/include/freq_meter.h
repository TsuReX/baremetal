

#ifndef BOARDS_CHECKBOARD_LPC54618J512_INCLUDE_FREQ_METER_H_
#define BOARDS_CHECKBOARD_LPC54618J512_INCLUDE_FREQ_METER_H_

#include <stdint.h>

struct fm_port_conf_t {
	uint8_t	gpio_port;
	uint8_t gpio_pin;
	uint8_t sct_in;
};

enum freq_meter_channel_t {
	fm_channel_0 = 0,
	fm_channel_1,
	fm_channel_2,
	fm_channel_3,
	fm_channel_4,
	fm_channel_5,
	fm_channel_6,
	fm_channel_7,
	fm_channel_8,
	fm_channel_9
};

struct freq_meter_t {
	enum freq_meter_channel_t  channel;
	uint32_t last_count_val;
};

void fm_init(struct freq_meter_t *fm);
void fm_free(struct freq_meter_t *fm);

void fm_clock_start_count(struct freq_meter_t *fm);
uint32_t fm_clock_stop_count(struct freq_meter_t *fm);


#endif /* BOARDS_CHECKBOARD_LPC54618J512_INCLUDE_FREQ_METER_H_ */
