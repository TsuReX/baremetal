/*
 * control.h
 *
 *  Created on: Apr 7, 2020
 *      Author: vasily
 */

#ifndef BOARDS_CHECKBOARD_LPC54618J512_INCLUDE_CONTROL_H_
#define BOARDS_CHECKBOARD_LPC54618J512_INCLUDE_CONTROL_H_

#include <drivers.h>

/*
	PVPP_EF			01000100	0x44
	PVDDQ_EF		01001100	0x4C
	PVTT_EF			01010100	0x54
	PVPP_CD			01011100	0x5C
	PVDDQ_CD		01100100	0x64
	PVTT_CD			01101100	0x6C
	PVPP_GH			01110100	0x74
	PVDDQ_GH		01111100	0x7C
	PVTT_GH			10000010	0x82
	PVTT_AB			10001010	0x8A
	PVPP_AB			10010010	0x92
	PVDDQ_AB		10011010	0x9A
	P0V75_AUX		10100010	0xA2
	P1V5_AUX		10101010	0xAA
	P1V26_AUX		10110010	0xB2
	PVCCIO			10111010	0xBA
	PVCCIN_CPU0		11000001	0xC1
	PVCCIN_CPU1		11001001	0xC9
	P1V05_PCH		11010001	0xD1
	P1V05_STBY_PCH	11011001	0xD9
	P1V5_PCH		11100001	0xE1
	P3V3_PCH		11101001	0xE9
	P3V3_STBY_DSW	11110001	0xF1
	P3V3_AUX		11111001	0xF9
*/
enum circuit_t {

	PVPP_EF = 0x44,
	PVDDQ_EF = 0x4C,
	PVTT_EF = 0x54,
	PVPP_CD = 0x5C,
	PVDDQ_CD = 0x64,
	PVTT_CD = 0x6C,
	PVPP_GH = 0x74,
	PVDDQ_GH = 0x7C,
	PVTT_GH = 0x82,
	PVTT_AB = 0x8A,
	PVPP_AB = 0x92,
	PVDDQ_AB = 0x9A,
	P0V75_AUX = 0xA2,
	P1V5_AUX = 0xAA,
	P1V26_AUX = 0xB2,
	PVCCIO = 0xBA,
	PVCCIN_CPU0 = 0xC1,
	PVCCIN_CPU1 = 0xC9,
	P1V05_PCH = 0xD1,
	P1V05_STBY_PCH = 0xD9,
	P1V5_PCH = 0xE1,
	P3V3_PCH = 0xE9,
	P3V3_STBY_DSW = 0xF1,
	P3V3_AUX = 0xF9,

};

enum measurement_t {
	RESISTANCE = 0,
	VOLTAGE = 1
};

static inline void ok_led_on(void)
{
	GPIO->B[O_LED_TST_OK_PORT][O_LED_TST_OK_PIN] = 1;
}

static inline void ok_led_off(void)
{
	GPIO->B[O_LED_TST_OK_PORT][O_LED_TST_OK_PIN] = 0;
}

static inline void ok_led_toggle(void)
{
	GPIO->NOT[O_LED_TST_OK_PORT] = O_LED_TST_OK_PIN;
}

static inline void fail_led_on(void)
{
	GPIO->B[O_LED_TST_FAIL_PORT][O_LED_TST_FAIL_PIN] = 1;
}

static inline void fail_led_off(void)
{
	GPIO->B[O_LED_TST_FAIL_PORT][O_LED_TST_FAIL_PIN] = 0;
}

static inline void fail_led_toggle(void)
{
	GPIO->NOT[O_LED_TST_FAIL_PORT] = O_LED_TST_FAIL_PIN;
}

static inline void bp_on(void)
{
	GPIO->B[O_PWR_ON_BP_N_PORT][O_PWR_ON_BP_N_PIN] = 0;
}

static inline void bp_off(void)
{
	GPIO->B[O_PWR_ON_BP_N_PORT][O_PWR_ON_BP_N_PIN] = 1;
}

static inline uint32_t bp_pwr_gd(void)
{
	return GPIO->B[I_PWR_GD_BP_N_PORT][I_PWR_GD_BP_N_PIN];
}

static inline uint32_t cpu0_phases(void)
{
	uint32_t PORT = I_CPU0_PH1_PORT;

	return	(GPIO->B[PORT][I_CPU0_PH6_PIN] << (5)) |
			(GPIO->B[PORT][I_CPU0_PH5_PIN] << (4)) |
			(GPIO->B[PORT][I_CPU0_PH4_PIN] << (3)) |
			(GPIO->B[PORT][I_CPU0_PH3_PIN] << (2)) |
			(GPIO->B[PORT][I_CPU0_PH2_PIN] << (1)) |
			(GPIO->B[PORT][I_CPU0_PH1_PIN]);
}

static inline uint32_t cpu1_phases(void)
{
	uint32_t PORT = I_CPU1_PH1_PORT;

	return	(GPIO->B[PORT][I_CPU1_PH6_PIN] << (5)) |
			(GPIO->B[PORT][I_CPU1_PH5_PIN] << (4)) |
			(GPIO->B[PORT][I_CPU1_PH4_PIN] << (3)) |
			(GPIO->B[PORT][I_CPU1_PH3_PIN] << (2)) |
			(GPIO->B[PORT][I_CPU1_PH2_PIN] << (1)) |
			(GPIO->B[PORT][I_CPU1_PH1_PIN]);
}

void mux_addr_setup(uint32_t config)
{
	GPIO->B[O_MUX_ADDR_0_PORT][O_MUX_ADDR_0_PIN] = (config >> 0) & 0x1;
	GPIO->B[O_MUX_ADDR_1_PORT][O_MUX_ADDR_1_PIN] = (config >> 1) & 0x1;
	GPIO->B[O_MUX_ADDR_2_PORT][O_MUX_ADDR_2_PIN] = (config >> 2) & 0x1;
	GPIO->B[O_MUX_ADDR_3_PORT][O_MUX_ADDR_3_PIN] = (config >> 3) & 0x1;
	GPIO->B[O_MUX_ADDR_4_PORT][O_MUX_ADDR_4_PIN] = (config >> 4) & 0x1;

}

void mux_1_2_3_setup(uint32_t config)
{
	GPIO->B[O_EN_MUX1_PORT][O_EN_MUX1_PIN] = (config >> 0) & 0x1;
	GPIO->B[O_EN_MUX2_PORT][O_EN_MUX2_PIN] = (config >> 1) & 0x1;
	GPIO->B[O_EN_MUX3_PORT][O_EN_MUX3_PIN] = (config >> 2) & 0x1;
}

uint32_t volt_ohm_measure(enum circuit_t circuit, enum measurement_t measurement)
{
	GPIO->B[O_VOLT_OHM_N_PORT][O_VOLT_OHM_N_PIN] = measurement;

	uint32_t mux_addr_config = circuit >> 3;
	uint32_t mux_1_2_3_config = circuit & 0x7;

	mux_addr_setup(mux_addr_config);

	mux_1_2_3_setup(mux_1_2_3_config);

	return 0;
}

#endif /* BOARDS_CHECKBOARD_LPC54618J512_INCLUDE_CONTROL_H_ */
