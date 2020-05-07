
#ifndef BOARDS_CHECKBOARD_LPC54618J512_INCLUDE_CONTROL_H_
#define BOARDS_CHECKBOARD_LPC54618J512_INCLUDE_CONTROL_H_

#include <drivers.h>

#define	RST_BMC_LVC3_N_SHIFT		0
#define RST_PCH_RSTBTN_N_SHIFT		1
#define	RST_RTCRST_N_SHIFT			2
#define	RST_SRTCRST_N_SHIFT			3
#define	RST_RSMRST_PCH_N_SHIFT		4
#define	RST_PLTRST_N_SHIFT			5

#define	RST_BMC_LVC3_N(X)		(((X) >> RST_BMC_LVC3_N_SHIFT) 		& 0x1)
#define RST_PCH_RSTBTN_N(X)		(((X) >> RST_PCH_RSTBTN_N_SHIFT)	& 0x1)
#define	RST_RTCRST_N(X)			(((X) >> RST_RTCRST_N_SHIFT)		& 0x1)
#define	RST_SRTCRST_N(X)		(((X) >> RST_SRTCRST_N_SHIFT)		& 0x1)
#define	RST_RSMRST_PCH_N(X)		(((X) >> RST_RSMRST_PCH_N_SHIFT)	& 0x1)
#define	RST_PLTRST_N(X)			(((X) >> RST_PLTRST_N_SHIFT)		& 0x1)

#define GET_ADDR(X)				((X) & 0xFF)
#define GET_MUX(X)				(((X) >> 8) & 0xFF)

#define MUX1	0x04
#define MUX2	0x02
#define MUX3	0x01
#define MUX5	0x08
#define MUX6	0x10

/*	____________________________________________________________________________
	| MUX6 | MUX5 | MUX1 | MUX2 | MUX3 | A7 | A6 | A5 | A4 | A3 | A2 | A1 | A0 |

	MUX5

	P5V_STBY			100000000001	0x0801
	P5V_STBY_PSU		100000000010	0x0802
	P5V_AUX				100000000011	0x0803

	MUX6

	P12V				1000000000000	0x1000
	P12V3A, P12V1		1000000000001	0x1001
	P12V2, P12V3B		1000000000010	0x1002
	P5V					1000000000011	0x1003
	N12V				1000000000100	0x1004
	P12V_P5V_AUX		1000000000101	0x1005
	P12V_SW_PVCCIN_CPU1	1000000000110	0x1006
	P12V_SW_PVCCIN_CPU0	1000000000111	0x1007

	MUX1

	PVPP_EF				10000001000		0x0408
	PVDDQ_EF			10000001001		0x0409
	PVTT_EF				10000001010		0x040A
	PVPP_CD				10000001011		0x040B
	PVDDQ_CD			10000001100		0x040C
	PVTT_CD				10000001101		0x040D
	PVPP_GH				10000001110		0x040E
	PVDDQ_GH			10000001111		0x040F

	MUX2

	PVTT_GH				1000010000		0x0210
	PVTT_AB				1000010001		0x0211
	PVPP_AB				1000010010		0x0212
	PVDDQ_AB			1000010011		0x0213
	P0V75_AUX			1000010100		0x0214
	P1V5_AUX			1000010101		0x0215
	P1V26_AUX			1000010110		0x0216
	PVCCIO				1000010111		0x0217

	MUX3

	PVCCIN_CPU0			100011000		0x0118
	PVCCIN_CPU1			100011001		0x0119
	P1V05_PCH			100011010		0x011A
	P1V05_STBY_PCH		100011011		0x011B
	P1V5_PCH			100011100		0x011C
	P3V3_PCH			100011101		0x011D
	P3V3_STBY_DSW		100011110		0x011E
	P3V3_AUX			100011111		0x011F
*/
enum circuit_t {

	P5V_STBY 			=	0x0801,
	P5V_STBY_PSU		=	0x0802,
	P5V_AUX				=	0x0803,
	P12V				=	0x1000,
	P12V3A				=	0x1001, /*!*/
	P12V1				=	0x1001, /*!*/
	P12V3B				=	0x1002, /*!*/
	P12V2				=	0x1002, /*!*/
	P5V					=	0x1003,
	N12V				=	0x1004,
	P12V_P5V_AUX		=	0x1005,
	P12V_SW_PVCCIN_CPU1	=	0x1006,
	P12V_SW_PVCCIN_CPU0	=	0x1007,
	PVPP_EF				=	0x0408,
	PVDDQ_EF			=	0x0409,
	PVTT_EF				=	0x040A,
	PVPP_CD				=	0x040B,
	PVDDQ_CD			=	0x040C,
	PVTT_CD				=	0x040D,
	PVPP_GH				=	0x040E,
	PVDDQ_GH			=	0x040F,
	PVTT_GH				=	0x0210,
	PVTT_AB				=	0x0211,
	PVPP_AB				=	0x0212,
	PVDDQ_AB			=	0x0213,
	P0V75_AUX			=	0x0214,
	P1V5_AUX			=	0x0215,
	P1V26_AUX			=	0x0216,
	PVCCIO				=	0x0217,
	PVCCIN_CPU0			=	0x0118,
	PVCCIN_CPU1			=	0x0119,
	P1V05_PCH			=	0x011A,
	P1V05_STBY_PCH		=	0x011B,
	P1V5_PCH			=	0x011C,
	P3V3_PCH			=	0x011D,
	P3V3_STBY_DSW		=	0x011E,
	P3V3_AUX			=	0x011F

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

static inline void cpu0_on(void)
{
	GPIO->B[O_SYSPL_CPU0_ON_PORT][O_SYSPL_CPU0_ON_PIN] = 1;
}

static inline void cpu0_off(void)
{
	GPIO->B[O_SYSPL_CPU0_ON_PORT][O_SYSPL_CPU0_ON_PIN] = 0;
}

static inline void cpu1_on(void)
{
	GPIO->B[O_SYSPL_CPU1_ON_PORT][O_SYSPL_CPU1_ON_PIN] = 1;
}

static inline void cpu1_off(void)
{
	GPIO->B[O_SYSPL_CPU1_ON_PORT][O_SYSPL_CPU1_ON_PIN] = 0;
}

static inline void spl_stby_on(void)
{
	GPIO->B[O_SYSPL_STBY_ON_PORT][O_SYSPL_STBY_ON_PIN] = 1;
}

static inline void spl_stby_off(void)
{
	GPIO->B[O_SYSPL_STBY_ON_PORT][O_SYSPL_STBY_ON_PIN] = 1;
}

static inline void spl_on(void)
{
	GPIO->B[O_SYSPL_STBY_ON_PORT][O_SYSPL_STBY_ON_PIN] = 1;
}

static inline void spl_off(void)
{
	GPIO->B[O_SYSPL_ON_PORT][O_SYSPL_ON_PIN] = 0;
}

static inline uint32_t pwr_on_spl(void)
{
	return GPIO->B[I_PWR_ON_SPL_N_PORT][I_PWR_ON_SPL_N_PIN];
}

static inline void freq_on(void)
{
	GPIO->B[O_MEASURE_FREQ_EN_N_PORT][O_MEASURE_FREQ_EN_N_PIN] = 0;
	GPIO->B[O_MEASURE_FREQ_RTC_EN_N_PORT][O_MEASURE_FREQ_RTC_EN_N_PIN] = 0;
}

static inline void freq_off(void)
{
	GPIO->B[O_MEASURE_FREQ_EN_N_PORT][O_MEASURE_FREQ_EN_N_PIN] = 1;
	GPIO->B[O_MEASURE_FREQ_RTC_EN_N_PORT][O_MEASURE_FREQ_RTC_EN_N_PIN] = 1;
}

static inline void rst_check_on(void)
{
	GPIO->B[O_CHECK_RST_SPL_N_PORT][O_CHECK_RST_SPL_N_PIN] = 0;
}

static inline void rst_check_off(void)
{
	GPIO->B[O_CHECK_RST_SPL_N_PORT][O_CHECK_RST_SPL_N_PIN] = 1;
}

static inline uint32_t rst_get(void)
{
	return	(GPIO->B[I_RST_BMC_LVC3_N_PORT][I_RST_BMC_LVC3_N_PIN]		<< RST_BMC_LVC3_N_SHIFT)	|
			(GPIO->B[I_RST_PCH_RSTBTN_N_PORT][I_RST_PCH_RSTBTN_N_PIN]	<< RST_PCH_RSTBTN_N_SHIFT)	|
			(GPIO->B[I_RST_RTCRST_N_PORT][I_RST_RTCRST_N_PIN]			<< RST_RTCRST_N_SHIFT)		|
			(GPIO->B[I_RST_SRTCRST_N_PORT][I_RST_SRTCRST_N_PORT]		<< RST_SRTCRST_N_SHIFT)		|
			(GPIO->B[I_RST_RSMRST_PCH_N_PORT][I_RST_RSMRST_PCH_N_PIN]	<< RST_RSMRST_PCH_N_SHIFT)	|
			(GPIO->B[I_RST_PLTRST_N_PORT][I_RST_PLTRST_N_PIN]			<< RST_PLTRST_N_SHIFT);

}

uint32_t volt_ohm_measure(enum circuit_t circuit, enum measurement_t measurement);

#endif /* BOARDS_CHECKBOARD_LPC54618J512_INCLUDE_CONTROL_H_ */
