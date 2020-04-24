#ifndef __DRIVERS_H
#define __DRIVERS_H

#include <fsl_common.h>
#include <fsl_gpio.h>
#include <fsl_iocon.h>
#include <fsl_power.h>
#include <fsl_rit.h>
#include "fsl_usart.h"
#include "fsl_usart_dma.h"
#include "fsl_dma.h"
#include "fsl_adc.h"
#include "fsl_sctimer.h"

#define A_3V3_BP_ADC_PORT			0
#define A_3V3_BP_ADC_PIN			14

#define A_5V_BP_ADC_PORT			0
#define A_5V_BP_ADC_PIN				15

#define A_12V_BP_ADC_PORT			0
#define A_12V_BP_ADC_PIN			31

#define A_P3V3_SYS_PL_OHM_ADC_PORT	1
#define A_P3V3_SYS_PL_OHM_ADC_PIN	0

#define D_RX_UC_TX_USB_PORT			1
#define D_RX_UC_TX_USB_PIN			5

#define D_TX_UC_RX_USB_PORT			1
#define D_TX_UC_RX_USB_PIN			6

#define I_CPU1_PH1_PORT				1
#define I_CPU1_PH1_PIN				14

#define I_CPU1_PH2_PORT				1
#define I_CPU1_PH2_PIN				15

#define I_CPU1_PH3_PORT				1
#define I_CPU1_PH3_PIN				16

#define I_CPU1_PH4_PORT				1
#define I_CPU1_PH4_PIN				17

#define I_CPU1_PH5_PORT				1
#define I_CPU1_PH5_PIN				18

#define I_CPU1_PH6_PORT				1
#define I_CPU1_PH6_PIN				19

#define A_MUX4_OHM_ADC_PORT			2
#define A_MUX4_OHM_ADC_PIN			0

#define A_MUX4_VOLT_ADC_PORT		2
#define A_MUX4_VOLT_ADC_PIN			1

#define O_VOLT_OHM_N_PORT			2
#define O_VOLT_OHM_N_PIN			3

#define O_MUX_ADDR_0_PORT			2
#define O_MUX_ADDR_0_PIN			11

#define O_MUX_ADDR_1_PORT			2
#define O_MUX_ADDR_1_PIN			12

#define O_MUX_ADDR_2_PORT			2
#define O_MUX_ADDR_2_PIN			13

#define O_MUX_ADDR_3_PORT			2
#define O_MUX_ADDR_3_PIN			14

#define O_MUX_ADDR_4_PORT			2
#define O_MUX_ADDR_4_PIN			15

#define O_EN_MUX1_PORT				2
#define O_EN_MUX1_PIN				18

#define O_EN_MUX2_PORT				2
#define O_EN_MUX2_PIN				19

#define O_EN_MUX3_PORT				2
#define O_EN_MUX3_PIN				20

#define I_CPU0_PH1_PORT				2
#define I_CPU0_PH1_PIN				22

#define I_CPU0_PH2_PORT				2
#define I_CPU0_PH2_PIN				23

#define I_CPU0_PH3_PORT				2
#define I_CPU0_PH3_PIN				24

#define I_CPU0_PH4_PORT				2
#define I_CPU0_PH4_PIN				25

#define I_CPU0_PH5_PORT				2
#define I_CPU0_PH5_PIN				26

#define I_CPU0_PH6_PORT				2
#define I_CPU0_PH6_PIN				27

#define O_PWR_ON_BP_N_PORT			3
#define O_PWR_ON_BP_N_PIN			0

#define I_PWR_GD_BP_N_PORT			3
#define I_PWR_GD_BP_N_PIN			1

#define I_PWR_ON_SPL_N_PORT			3
#define I_PWR_ON_BP_N_PIN			2

#define O_SYSPL_STBY_ON_PORT		3
#define O_SYSPL_STBY_ON_PIN			4

#define O_SYSPL_ON_PORT				3
#define O_SYSPL_ON_PIN				5

#define O_SYSPL_CPU0_ON_PORT		3
#define O_SYSPL_CPU0_ON_PIN			9

#define O_SYSPL_CPU1_ON_PORT		3
#define O_SYSPL_CPU1_ON_PIN			10

#define O_MUX5_ADDR_A0_PORT			3
#define O_MUX5_ADDR_A0_PIN			19

#define O_MUX5_ADDR_A1_PORT			3
#define O_MUX5_ADDR_A1_PIN			20

#define A_MUX5_OHM_ADC_PORT			3
#define A_MUX5_OHM_ADC_PIN			21

#define A_MUX6_OHM_ADC_PORT			3
#define A_MUX6_OHM_ADC_PIN			22

#define O_MUX6_ADDR_0_PORT			3
#define O_MUX6_ADDR_0_PIN			25

#define O_MUX6_ADDR_1_PORT			3
#define O_MUX6_ADDR_1_PIN			26

#define O_MUX6_ADDR_2_PORT			3
#define O_MUX6_ADDR_2_PIN			27

#define O_EN_MUX6_PORT				3
#define O_EN_MUX6_PIN				29

#define D_RTC_CLK_KT_42_PORT		4
#define D_RTC_CLK_KT_42_PIN			0

#define O_MEASURE_FREQ_RTC_EN_N_PORT	4
#define O_MEASURE_FREQ_RTC_EN_N_PIN		1

#define O_MEASURE_FREQ_EN_N_PORT	4
#define O_MEASURE_FREQ_EN_N_PIN		5

#define D_CLK_KT_43_PORT			4
#define D_CLK_KT_43_PIN				7

#define D_CLK_KT_44_PORT			4
#define D_CLK_KT_44_PIN				8

#define D_CLK_KT_45_PORT			4
#define D_CLK_KT_45_PIN				9

#define D_CLK_KT_46_PORT			4
#define D_CLK_KT_46_PIN				10

#define D_CLK_KT_47_PORT			4
#define D_CLK_KT_47_PIN				11

#define D_CLK_KT_48_PORT			4
#define D_CLK_KT_48_PIN				12

#define D_CLK_KT_49_PORT			4
#define D_CLK_KT_49_PIN				13

#define D_CLK_KT_50_PORT			4
#define D_CLK_KT_50_PIN				14

#define O_CHECK_RST_SPL_N_PORT		4
#define O_CHECK_RST_SPL_N_PIN		20

#define I_RST_BMC_LVC3_N_PORT		4
#define I_RST_BMC_LVC3_N_PIN		24

#define I_RST_PCH_RSTBTN_N_PORT		4
#define I_RST_PCH_RSTBTN_N_PIN		25

#define I_RST_RTCRST_N_PORT			4
#define I_RST_RTCRST_N_PIN			26

#define I_RST_SRTCRST_N_PORT		4
#define I_RST_SRTCRST_N_PIN			27

#define I_RST_RSMRST_PCH_N_PORT		4
#define I_RST_RSMRST_PCH_N_PIN		28

#define I_RST_PLTRST_N_PORT			4
#define I_RST_PLTRST_N_PIN			29

#define O_PWR_SW_N_PORT				4
#define O_PWR_SW_N_PIN				30

#define O_RST_SW_N_PORT				4
#define O_RST_SW_N_PIN				31

#define O_LED_TST_OK_PORT			5
#define O_LED_TST_OK_PIN			0

#define O_LED_TST_FAIL_PORT			5
#define O_LED_TST_FAIL_PIN			1

#define I_SW_TST_N_PORT				5
#define I_SW_TST_N_PIN				2

#endif /* __DRIVERS_H */

