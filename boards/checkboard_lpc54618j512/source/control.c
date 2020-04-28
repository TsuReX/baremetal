#include <control.h>
#include <adc.h>
#include <time.h>

static void mux_addr_setup(uint8_t mux, uint8_t address)
{
	GPIO->B[O_MUX_ADDR_0_PORT][O_MUX_ADDR_0_PIN] = 0;
	GPIO->B[O_MUX_ADDR_1_PORT][O_MUX_ADDR_1_PIN] = 0;
	GPIO->B[O_MUX_ADDR_2_PORT][O_MUX_ADDR_2_PIN] = 0;
	GPIO->B[O_MUX_ADDR_3_PORT][O_MUX_ADDR_3_PIN] = 0;
	GPIO->B[O_MUX_ADDR_4_PORT][O_MUX_ADDR_4_PIN] = 0;

	GPIO->B[O_MUX5_ADDR_A0_PORT][O_MUX5_ADDR_A0_PIN] = 0;
	GPIO->B[O_MUX5_ADDR_A1_PORT][O_MUX5_ADDR_A1_PIN] = 0;

	GPIO->B[O_MUX6_ADDR_0_PORT][O_MUX6_ADDR_0_PIN] = 0;
	GPIO->B[O_MUX6_ADDR_1_PORT][O_MUX6_ADDR_1_PIN] = 0;
	GPIO->B[O_MUX6_ADDR_2_PORT][O_MUX6_ADDR_2_PIN] = 0;

	switch (mux) {
		case MUX1:
		case MUX2:
		case MUX3:
				GPIO->B[O_MUX_ADDR_0_PORT][O_MUX_ADDR_0_PIN] = (address >> 0) & 0x1;
				GPIO->B[O_MUX_ADDR_1_PORT][O_MUX_ADDR_1_PIN] = (address >> 1) & 0x1;
				GPIO->B[O_MUX_ADDR_2_PORT][O_MUX_ADDR_2_PIN] = (address >> 2) & 0x1;
				GPIO->B[O_MUX_ADDR_3_PORT][O_MUX_ADDR_3_PIN] = (address >> 3) & 0x1;
				GPIO->B[O_MUX_ADDR_4_PORT][O_MUX_ADDR_4_PIN] = (address >> 4) & 0x1;
				break;

		case MUX5:
				GPIO->B[O_MUX5_ADDR_A0_PORT][O_MUX5_ADDR_A0_PIN] = (address >> 0) & 0x1;
				GPIO->B[O_MUX5_ADDR_A1_PORT][O_MUX5_ADDR_A1_PIN] = (address >> 1) & 0x1;
				break;

		case MUX6:
				GPIO->B[O_MUX6_ADDR_0_PORT][O_MUX6_ADDR_0_PIN] = (address >> 0) & 0x1;
				GPIO->B[O_MUX6_ADDR_1_PORT][O_MUX6_ADDR_1_PIN] = (address >> 1) & 0x1;
				GPIO->B[O_MUX6_ADDR_2_PORT][O_MUX6_ADDR_2_PIN] = (address >> 2) & 0x1;
				break;
	}
}

static void mux_setup(uint32_t mux)
{
	GPIO->B[O_EN_MUX1_PORT][O_EN_MUX1_PIN] = 0;
	GPIO->B[O_EN_MUX2_PORT][O_EN_MUX2_PIN] = 0;
	GPIO->B[O_EN_MUX3_PORT][O_EN_MUX3_PIN] = 0;
	GPIO->B[O_EN_MUX6_PORT][O_EN_MUX6_PIN] = 0;

	switch (mux) {
		case MUX1:
				GPIO->B[O_EN_MUX1_PORT][O_EN_MUX1_PIN] = 1;
				break;

		case MUX2:
				GPIO->B[O_EN_MUX2_PORT][O_EN_MUX2_PIN] = 1;
				break;

		case MUX3:
				GPIO->B[O_EN_MUX3_PORT][O_EN_MUX3_PIN] = 1;
				break;

		case MUX5:
				/* Мультиплекор аксивен всегда. */
				break;

		case MUX6:
				GPIO->B[O_EN_MUX6_PORT][O_EN_MUX6_PIN] = 1;
				break;
	}
}

uint32_t volt_ohm_measure(enum circuit_t circuit, enum measurement_t measurement)
{
	GPIO->B[O_VOLT_OHM_N_PORT][O_VOLT_OHM_N_PIN] = measurement;

	mux_addr_setup(GET_MUX(circuit), GET_ADDR(circuit));

	mux_setup(GET_MUX(circuit));

	uint32_t value = 0;

	adc0_convert();
	mdelay(1);

	switch (GET_MUX(circuit)) {
		case MUX1:
		case MUX2:
		case MUX3:
				if (measurement == RESISTANCE) {
					value = adc0_get_value(CHAN7_MUX4_OHM_ADC);

				} else if (measurement == VOLTAGE) {
					value = adc0_get_value(CHAN8_MUX4_VOLT_ADC);
				}
				break;

		case MUX5:
				value = adc0_get_value(CHAN9_MUX5_OHM_ADC);
				break;

		case MUX6:
				value = adc0_get_value(CHAN10_MUX6_OHM_ADC);
				break;
	}

	return value;
}
