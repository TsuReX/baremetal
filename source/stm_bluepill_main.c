#include <string.h>
#include "time.h"
#include "drivers.h"
#include "console.h"
#include "spi.h"
#include "init.h"
#include "config.h"
#include "debug.h"
#include "spi_flash.h"

#define DECMODE		0x0009
#define INTENSITY	0x000A
#define SCANLIM		0x000B
#define SHUTDOWN	0x000C

uint16_t adc_buf[2] = {0, 0};

void tim2_init(void)
{
	LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM2);

	LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOA);

	/**TIM2 GPIO Configuration
	PA2     ------> TIM2_CH3
	PA3     ------> TIM2_CH4
	*/
	LL_GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.Pin = LL_GPIO_PIN_2|LL_GPIO_PIN_3;
	GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
	GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
	GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
	LL_GPIO_Init(GPIOA, &GPIO_InitStruct);


	/* General timer configuration */
	LL_TIM_InitTypeDef TIM_InitStruct = {0};
	TIM_InitStruct.Prescaler = 48;
	TIM_InitStruct.CounterMode = LL_TIM_COUNTERMODE_UP;
	TIM_InitStruct.Autoreload = 1000;
	TIM_InitStruct.ClockDivision = LL_TIM_CLOCKDIVISION_DIV1;
	LL_TIM_Init(TIM2, &TIM_InitStruct);
	LL_TIM_DisableARRPreload(TIM2);

	/* PWN specific timer configuration */
	LL_TIM_OC_InitTypeDef TIM_OC_InitStruct = {0};
	TIM_OC_InitStruct.OCMode = LL_TIM_OCMODE_PWM1;
	TIM_OC_InitStruct.OCState = LL_TIM_OCSTATE_DISABLE;
	TIM_OC_InitStruct.OCNState = LL_TIM_OCSTATE_DISABLE;
	TIM_OC_InitStruct.OCPolarity = LL_TIM_OCPOLARITY_HIGH;

	LL_TIM_OC_EnablePreload(TIM2, LL_TIM_CHANNEL_CH3);
	TIM_OC_InitStruct.CompareValue = 200;
	LL_TIM_OC_Init(TIM2, LL_TIM_CHANNEL_CH3, &TIM_OC_InitStruct);
	LL_TIM_OC_DisableFast(TIM2, LL_TIM_CHANNEL_CH3);

	LL_TIM_OC_EnablePreload(TIM2, LL_TIM_CHANNEL_CH4);
	TIM_OC_InitStruct.CompareValue = 800;
	LL_TIM_OC_Init(TIM2, LL_TIM_CHANNEL_CH4, &TIM_OC_InitStruct);
	LL_TIM_OC_DisableFast(TIM2, LL_TIM_CHANNEL_CH4);

	/* Encoder specific timer configuration */
//	LL_TIM_SetEncoderMode(TIM2, LL_TIM_ENCODERMODE_X2_TI1);
//
//	LL_TIM_IC_SetActiveInput(TIM2, LL_TIM_CHANNEL_CH1, LL_TIM_ACTIVEINPUT_DIRECTTI);
//	LL_TIM_IC_SetPrescaler(TIM2, LL_TIM_CHANNEL_CH1, LL_TIM_ICPSC_DIV1);
//	LL_TIM_IC_SetFilter(TIM2, LL_TIM_CHANNEL_CH1, LL_TIM_IC_FILTER_FDIV1);
//	LL_TIM_IC_SetPolarity(TIM2, LL_TIM_CHANNEL_CH1, LL_TIM_IC_POLARITY_RISING);
//
//	LL_TIM_IC_SetActiveInput(TIM2, LL_TIM_CHANNEL_CH2, LL_TIM_ACTIVEINPUT_DIRECTTI);
//	LL_TIM_IC_SetPrescaler(TIM2, LL_TIM_CHANNEL_CH2, LL_TIM_ICPSC_DIV1);
//	LL_TIM_IC_SetFilter(TIM2, LL_TIM_CHANNEL_CH2, LL_TIM_IC_FILTER_FDIV1);
//	LL_TIM_IC_SetPolarity(TIM2, LL_TIM_CHANNEL_CH2, LL_TIM_IC_POLARITY_RISING);

	LL_TIM_SetTriggerOutput(TIM2, LL_TIM_TRGO_RESET);
	LL_TIM_DisableMasterSlaveMode(TIM2);
}

void tim3_init(void)
{
	LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM3);

	LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOA);
	/**TIM3 GPIO Configuration
	PA6   ------> TIM3_CH1
	PA7   ------> TIM3_CH2
	*/
	LL_GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.Pin = LL_GPIO_PIN_6|LL_GPIO_PIN_7;
	GPIO_InitStruct.Mode = LL_GPIO_MODE_FLOATING;
	LL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	LL_TIM_SetEncoderMode(TIM3, LL_TIM_ENCODERMODE_X2_TI1);

	LL_TIM_IC_SetActiveInput(TIM3, LL_TIM_CHANNEL_CH1, LL_TIM_ACTIVEINPUT_DIRECTTI);
	LL_TIM_IC_SetPrescaler(TIM3, LL_TIM_CHANNEL_CH1, LL_TIM_ICPSC_DIV8);
	LL_TIM_IC_SetFilter(TIM3, LL_TIM_CHANNEL_CH1, LL_TIM_IC_FILTER_FDIV32_N8);
	LL_TIM_IC_SetPolarity(TIM3, LL_TIM_CHANNEL_CH1, LL_TIM_IC_POLARITY_RISING);

	LL_TIM_IC_SetActiveInput(TIM3, LL_TIM_CHANNEL_CH2, LL_TIM_ACTIVEINPUT_DIRECTTI);
	LL_TIM_IC_SetPrescaler(TIM3, LL_TIM_CHANNEL_CH2, LL_TIM_ICPSC_DIV8);
	LL_TIM_IC_SetFilter(TIM3, LL_TIM_CHANNEL_CH2, LL_TIM_IC_FILTER_FDIV32_N8);
	LL_TIM_IC_SetPolarity(TIM3, LL_TIM_CHANNEL_CH2, LL_TIM_IC_POLARITY_RISING);

	LL_TIM_InitTypeDef TIM_InitStruct = {0};
	TIM_InitStruct.Prescaler = 0;
	TIM_InitStruct.CounterMode = LL_TIM_COUNTERMODE_UP;
	TIM_InitStruct.Autoreload = 100;
	TIM_InitStruct.ClockDivision = LL_TIM_CLOCKDIVISION_DIV4;
	LL_TIM_Init(TIM3, &TIM_InitStruct);
	LL_TIM_DisableARRPreload(TIM3);
	LL_TIM_SetTriggerOutput(TIM3, LL_TIM_TRGO_RESET);
	LL_TIM_DisableMasterSlaveMode(TIM3);
}

void tim2_start() {
	LL_TIM_CC_EnableChannel(TIM2, LL_TIM_CHANNEL_CH3 | LL_TIM_CHANNEL_CH4);

	LL_TIM_EnableCounter(TIM2);
}

void tim3_start() {
	LL_TIM_SetCounter(TIM3, 50);
	LL_TIM_CC_EnableChannel(TIM3, LL_TIM_CHANNEL_CH1 | LL_TIM_CHANNEL_CH2);
	LL_TIM_EnableCounter(TIM3);
}

void encoder3_value_print() {
	printk(DEBUG,"Encoder 3: 0x%08lX\r\n", LL_TIM_GetCounter(TIM3));
}

void adc1_init(void)
{
	LL_ADC_InitTypeDef ADC_InitStruct = {0};
	LL_ADC_CommonInitTypeDef ADC_CommonInitStruct = {0};
	LL_ADC_REG_InitTypeDef ADC_REG_InitStruct = {0};

	LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

	/* Peripheral clock enable */
	LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_ADC1);

	LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOA);
	/**ADC1 GPIO Configuration
	PA4   ------> ADC1_IN4
	PA5   ------> ADC1_IN5
	*/
	GPIO_InitStruct.Pin = LL_GPIO_PIN_4|LL_GPIO_PIN_5;
	GPIO_InitStruct.Mode = LL_GPIO_MODE_ANALOG;
	LL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	ADC_InitStruct.DataAlignment = LL_ADC_DATA_ALIGN_RIGHT;
	ADC_InitStruct.SequencersScanMode = LL_ADC_SEQ_SCAN_ENABLE;
	LL_ADC_Init(ADC1, &ADC_InitStruct);
	ADC_CommonInitStruct.Multimode = LL_ADC_MULTI_INDEPENDENT;
	LL_ADC_CommonInit(__LL_ADC_COMMON_INSTANCE(ADC1), &ADC_CommonInitStruct);
	ADC_REG_InitStruct.TriggerSource = LL_ADC_REG_TRIG_SOFTWARE;
	ADC_REG_InitStruct.SequencerLength = LL_ADC_REG_SEQ_SCAN_ENABLE_2RANKS;
	ADC_REG_InitStruct.SequencerDiscont = LL_ADC_REG_SEQ_DISCONT_DISABLE;
	ADC_REG_InitStruct.ContinuousMode = LL_ADC_REG_CONV_SINGLE;
	ADC_REG_InitStruct.DMATransfer = LL_ADC_REG_DMA_TRANSFER_UNLIMITED;
	LL_ADC_REG_Init(ADC1, &ADC_REG_InitStruct);

	LL_ADC_REG_SetSequencerRanks(ADC1, LL_ADC_REG_RANK_1, LL_ADC_CHANNEL_4);
	LL_ADC_SetChannelSamplingTime(ADC1, LL_ADC_CHANNEL_4, LL_ADC_SAMPLINGTIME_1CYCLE_5);

	LL_ADC_REG_SetSequencerRanks(ADC1, LL_ADC_REG_RANK_2, LL_ADC_CHANNEL_5);
	LL_ADC_SetChannelSamplingTime(ADC1, LL_ADC_CHANNEL_5, LL_ADC_SAMPLINGTIME_1CYCLE_5);

	LL_ADC_Enable(ADC1);
	LL_ADC_StartCalibration(ADC1);
	while (LL_ADC_IsCalibrationOnGoing(ADC1));

	LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_DMA1);

	LL_DMA_SetDataTransferDirection(DMA1, LL_DMA_CHANNEL_1, LL_DMA_DIRECTION_PERIPH_TO_MEMORY);
	LL_DMA_SetChannelPriorityLevel(DMA1, LL_DMA_CHANNEL_1, LL_DMA_PRIORITY_HIGH);
	LL_DMA_SetMode(DMA1, LL_DMA_CHANNEL_1, LL_DMA_MODE_CIRCULAR);
	LL_DMA_SetPeriphIncMode(DMA1, LL_DMA_CHANNEL_1, LL_DMA_PERIPH_NOINCREMENT);
	LL_DMA_SetMemoryIncMode(DMA1, LL_DMA_CHANNEL_1, LL_DMA_MEMORY_INCREMENT);
	LL_DMA_SetPeriphSize(DMA1, LL_DMA_CHANNEL_1, LL_DMA_PDATAALIGN_HALFWORD);
	LL_DMA_SetMemorySize(DMA1, LL_DMA_CHANNEL_1, LL_DMA_MDATAALIGN_HALFWORD);

	LL_DMA_SetMemoryAddress(DMA1, LL_DMA_CHANNEL_1, (uint32_t)&adc_buf);
	LL_DMA_SetPeriphAddress(DMA1, LL_DMA_CHANNEL_1, (uint32_t)&(ADC1->DR));

	LL_DMA_SetDataLength(DMA1, LL_DMA_CHANNEL_1, 2);
	LL_DMA_EnableChannel(DMA1, LL_DMA_CHANNEL_1);
}

void adc1_value_print()
{
#define uref_value		3300000 /* micro volts */
#define uvolts_per_bit	(uref_value >> 12)

	LL_ADC_REG_StartConversionSWStart(ADC1);

	while (!LL_DMA_IsActiveFlag_TC1(DMA1));
	LL_DMA_ClearFlag_TC1(DMA1);

	printk(DEBUG, "ADC 1 0: %d\r\n", adc_buf[0] * uvolts_per_bit / 1000);

	printk(DEBUG, "ADC 1 1: %d\r\n", adc_buf[1] * uvolts_per_bit / 1000);
}

void max7219_digit_value_set(size_t dig_num, uint32_t value)
{
	dig_num = (dig_num & 0x7) + 1; /* (0-7)->(1-8) */
	value &= 0xF;

	uint16_t cmd = (value << 8) | dig_num;
	spi_chip_activate(0);
	spi_dma_data_send((uint8_t*)&cmd, sizeof(cmd));
	spi_chip_deactivate(0);

}

void max7219_init(void)
{
	uint16_t	cmd = 0x0;

	cmd = (0x0F << 8) | INTENSITY;
	spi_chip_activate(FLASH_CHANNEL);
	spi_dma_data_send((uint8_t*)&cmd, sizeof(cmd));
	spi_chip_deactivate(FLASH_CHANNEL);

	cmd = (0xFF << 8) | DECMODE;
	spi_chip_activate(FLASH_CHANNEL);
	spi_dma_data_send((uint8_t*)&cmd, sizeof(cmd));
	spi_chip_deactivate(FLASH_CHANNEL);

	cmd = (0x0F << 8) | SCANLIM;
	spi_chip_activate(FLASH_CHANNEL);
	spi_dma_data_send((uint8_t*)&cmd, sizeof(cmd));
	spi_chip_deactivate(FLASH_CHANNEL);

	cmd = (0x01 << 8) | SHUTDOWN;
	spi_chip_activate(FLASH_CHANNEL);
	spi_dma_data_send((uint8_t*)&cmd, sizeof(cmd));
	spi_chip_deactivate(FLASH_CHANNEL);

}

void sfdp_check(void)
{
	uint8_t sfdp[256];
	size_t sfdp_size = 16;
	memset(sfdp, 0xFF, sizeof(sfdp));
	spi_chip_activate(FLASH_CHANNEL);
	spi_flash_sfdp_read(sfdp, sfdp_size);
	spi_chip_deactivate(FLASH_CHANNEL);

	spi_flash_test();

	uint32_t sfdp_presence = 0;
	if (strncmp("SFDP", (const char*)sfdp, 4) == 0)
		sfdp_presence = 1;

	size_t i = 0;
	for (;i < sfdp_size; i++) {
		printk(INFO, "0x%02X\r\n", sfdp[i]);
	}

	if (sfdp_presence == 0) {
		while(1) {
			mdelay(500);
			LL_GPIO_TogglePin(GPIOC, LL_GPIO_PIN_13);
		}
	} else {

		LL_GPIO_ResetOutputPin(GPIOC, LL_GPIO_PIN_13);
		__WFI();
	}
}
#define FUNC 2

int main(void)
{
	soc_init();

	board_init();

	console_init();

	log_level_set(DEBUG);

#if (FUNC == 0)

	spi_init();
	sfdp_check();

#elif (FUNC == 1)

	spi_init();
	mdelay(1000);
	max7219_init();
	size_t dig_num = 0;
	size_t value = 0;
	while (1) {
		for (dig_num = 0; dig_num < 8; ++dig_num) {
			max7219_digit_value_set(dig_num, dig_num + value);
		}
		value++;
		mdelay(200);
	}

#elif (FUNC == 2)

	tim2_init();

	tim2_start();

	tim3_init();

	tim3_start();

	adc1_init();

//	adc1_start();
	uint32_t i = 0;
	while (1) {

		printk(DEBUG, "Blue pill %ld\r\n", i++);

		LL_GPIO_TogglePin(GPIOC, LL_GPIO_PIN_13);

		encoder3_value_print();

		adc1_value_print();

		mdelay(100);
	}

#endif

	return 0;
}
