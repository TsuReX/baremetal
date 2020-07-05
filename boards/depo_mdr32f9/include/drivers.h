#ifndef __DRIVERS_H
#define __DRIVERS_H

#include <MDR32Fx.h>
#include <MDR32F9Qx_adc.h>
#include <MDR32F9Qx_audio.h>
#include <MDR32F9Qx_comp.h>
#include <MDR32F9Qx_ebc.h>
#include <MDR32F9Qx_i2c.h>
#include <MDR32F9Qx_led.h>
#include <MDR32F9Qx_port.h>
#include <MDR32F9Qx_ssp.h>
#include <MDR32F9Qx_usb.h>
#include <MDR32F9Qx_arinc429r.h>
#include <MDR32F9Qx_bkp.h>
#include <MDR32F9Qx_dac.h>
#include <MDR32F9Qx_eeprom.h>
#include <MDR32F9Qx_iwdg.h>
#include <MDR32F9Qx_lib.h>
#include <MDR32F9Qx_power.h>
#include <MDR32F9Qx_timer.h>
#include <MDR32F9Qx_wwdg.h>
#include <MDR32F9Qx_arinc429t.h>
#include <MDR32F9Qx_can.h>
#include <MDR32F9Qx_dma.h>
//#include <MDR32F9Qx_eth.h>
#include <MDR32F9Qx_keypad.h>
//#include <MDR32F9Qx_mil_std_1553.h>
#include <MDR32F9Qx_rst_clk.h>
#include <MDR32F9Qx_uart.h>

/** Delay 500 milliseconds. */
#define DELAY_500_MS	500
/** Delay 50 milliseconds. */
#define DELAY_50_MS	50

static inline uint16_t swap_bytes_in_word(uint16_t word)
{
	uint8_t tmp = ((uint8_t *)&word)[0];
	((uint8_t *)&word)[0] = ((uint8_t *)&word)[1];
	((uint8_t *)&word)[1] = tmp;
	return word;
}

#endif /* __DRIVERS_H */

