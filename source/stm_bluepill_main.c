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

void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  LL_TIM_InitTypeDef TIM_InitStruct = {0};
  LL_TIM_OC_InitTypeDef TIM_OC_InitStruct = {0};

  LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* Peripheral clock enable */
  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM2);

  LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOA);
  /**TIM2 GPIO Configuration
  PA0-WKUP   ------> TIM2_CH1
  PA1   ------> TIM2_CH2
  */
  GPIO_InitStruct.Pin = LL_GPIO_PIN_0|LL_GPIO_PIN_1;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_FLOATING;
  LL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  TIM_InitStruct.Prescaler = 0;
  TIM_InitStruct.CounterMode = LL_TIM_COUNTERMODE_UP;
  TIM_InitStruct.Autoreload = 65535;
  TIM_InitStruct.ClockDivision = LL_TIM_CLOCKDIVISION_DIV1;
  LL_TIM_Init(TIM2, &TIM_InitStruct);
  LL_TIM_DisableARRPreload(TIM2);
  LL_TIM_OC_EnablePreload(TIM2, LL_TIM_CHANNEL_CH3);
  TIM_OC_InitStruct.OCMode = LL_TIM_OCMODE_PWM1;
  TIM_OC_InitStruct.OCState = LL_TIM_OCSTATE_DISABLE;
  TIM_OC_InitStruct.OCNState = LL_TIM_OCSTATE_DISABLE;
  TIM_OC_InitStruct.CompareValue = 0;
  TIM_OC_InitStruct.OCPolarity = LL_TIM_OCPOLARITY_HIGH;
  LL_TIM_OC_Init(TIM2, LL_TIM_CHANNEL_CH3, &TIM_OC_InitStruct);
  LL_TIM_OC_DisableFast(TIM2, LL_TIM_CHANNEL_CH3);
  LL_TIM_OC_EnablePreload(TIM2, LL_TIM_CHANNEL_CH4);
  LL_TIM_OC_Init(TIM2, LL_TIM_CHANNEL_CH4, &TIM_OC_InitStruct);
  LL_TIM_OC_DisableFast(TIM2, LL_TIM_CHANNEL_CH4);
  LL_TIM_SetEncoderMode(TIM2, LL_TIM_ENCODERMODE_X2_TI1);
  LL_TIM_IC_SetActiveInput(TIM2, LL_TIM_CHANNEL_CH1, LL_TIM_ACTIVEINPUT_DIRECTTI);
  LL_TIM_IC_SetPrescaler(TIM2, LL_TIM_CHANNEL_CH1, LL_TIM_ICPSC_DIV1);
  LL_TIM_IC_SetFilter(TIM2, LL_TIM_CHANNEL_CH1, LL_TIM_IC_FILTER_FDIV1);
  LL_TIM_IC_SetPolarity(TIM2, LL_TIM_CHANNEL_CH1, LL_TIM_IC_POLARITY_RISING);
  LL_TIM_IC_SetActiveInput(TIM2, LL_TIM_CHANNEL_CH2, LL_TIM_ACTIVEINPUT_DIRECTTI);
  LL_TIM_IC_SetPrescaler(TIM2, LL_TIM_CHANNEL_CH2, LL_TIM_ICPSC_DIV1);
  LL_TIM_IC_SetFilter(TIM2, LL_TIM_CHANNEL_CH2, LL_TIM_IC_FILTER_FDIV1);
  LL_TIM_IC_SetPolarity(TIM2, LL_TIM_CHANNEL_CH2, LL_TIM_IC_POLARITY_RISING);
  LL_TIM_SetTriggerOutput(TIM2, LL_TIM_TRGO_RESET);
  LL_TIM_DisableMasterSlaveMode(TIM2);
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */
  LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOA);
    /**TIM2 GPIO Configuration
    PA2     ------> TIM2_CH3
    PA3     ------> TIM2_CH4
    */
  GPIO_InitStruct.Pin = LL_GPIO_PIN_2|LL_GPIO_PIN_3;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  LL_GPIO_Init(GPIOA, &GPIO_InitStruct);

}

/* TIM3 init function */
void MX_TIM3_Init(void)
{

  /* USER CODE BEGIN TIM3_Init 0 */

  /* USER CODE END TIM3_Init 0 */

  LL_TIM_InitTypeDef TIM_InitStruct = {0};

  LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* Peripheral clock enable */
  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM3);

  LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOA);
  /**TIM3 GPIO Configuration
  PA6   ------> TIM3_CH1
  PA7   ------> TIM3_CH2
  */
  GPIO_InitStruct.Pin = LL_GPIO_PIN_6|LL_GPIO_PIN_7;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_FLOATING;
  LL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* USER CODE BEGIN TIM3_Init 1 */

  /* USER CODE END TIM3_Init 1 */
  LL_TIM_SetEncoderMode(TIM3, LL_TIM_ENCODERMODE_X2_TI1);
  LL_TIM_IC_SetActiveInput(TIM3, LL_TIM_CHANNEL_CH1, LL_TIM_ACTIVEINPUT_DIRECTTI);
  LL_TIM_IC_SetPrescaler(TIM3, LL_TIM_CHANNEL_CH1, LL_TIM_ICPSC_DIV1);
  LL_TIM_IC_SetFilter(TIM3, LL_TIM_CHANNEL_CH1, LL_TIM_IC_FILTER_FDIV1);
  LL_TIM_IC_SetPolarity(TIM3, LL_TIM_CHANNEL_CH1, LL_TIM_IC_POLARITY_RISING);
  LL_TIM_IC_SetActiveInput(TIM3, LL_TIM_CHANNEL_CH2, LL_TIM_ACTIVEINPUT_DIRECTTI);
  LL_TIM_IC_SetPrescaler(TIM3, LL_TIM_CHANNEL_CH2, LL_TIM_ICPSC_DIV1);
  LL_TIM_IC_SetFilter(TIM3, LL_TIM_CHANNEL_CH2, LL_TIM_IC_FILTER_FDIV1);
  LL_TIM_IC_SetPolarity(TIM3, LL_TIM_CHANNEL_CH2, LL_TIM_IC_POLARITY_RISING);
  TIM_InitStruct.Prescaler = 0;
  TIM_InitStruct.CounterMode = LL_TIM_COUNTERMODE_UP;
  TIM_InitStruct.Autoreload = 65535;
  TIM_InitStruct.ClockDivision = LL_TIM_CLOCKDIVISION_DIV1;
  LL_TIM_Init(TIM3, &TIM_InitStruct);
  LL_TIM_DisableARRPreload(TIM3);
  LL_TIM_SetTriggerOutput(TIM3, LL_TIM_TRGO_RESET);
  LL_TIM_DisableMasterSlaveMode(TIM3);
  /* USER CODE BEGIN TIM3_Init 2 */

  /* USER CODE END TIM3_Init 2 */

}

void init() {
    // так можно проставить начальное значение счетчика:
	__HAL_TIM_SET_COUNTER(&htim1, 32760);

	// не забываем включить таймер!
	HAL_TIM_Encoder_Start(&htim1, TIM_CHANNEL_ALL);
}

void loop() {
    int currCounter = __HAL_TIM_GET_COUNTER(&htim1);
    currCounter = 32767 - ((currCounter-1) & 0xFFFF) / 2;
    if(currCounter != prevCounter) {
        char buff[16];
        snprintf(buff, sizeof(buff), "%06d", currCounter);

        // выводим куда-то currCounter
        // ... пропущено ...

        prevCounter = currCounter;
    }
}

void MX_ADC1_Init(void)
{

	 /* USER CODE BEGIN ADC1_Init 0 */

	  /* USER CODE END ADC1_Init 0 */

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

	  /* USER CODE BEGIN ADC1_Init 1 */

	  /* USER CODE END ADC1_Init 1 */
	  /** Common config
	  */
	  ADC_InitStruct.DataAlignment = LL_ADC_DATA_ALIGN_RIGHT;
	  ADC_InitStruct.SequencersScanMode = LL_ADC_SEQ_SCAN_DISABLE;
	  LL_ADC_Init(ADC1, &ADC_InitStruct);
	  ADC_CommonInitStruct.Multimode = LL_ADC_MULTI_INDEPENDENT;
	  LL_ADC_CommonInit(__LL_ADC_COMMON_INSTANCE(ADC1), &ADC_CommonInitStruct);
	  ADC_REG_InitStruct.TriggerSource = LL_ADC_REG_TRIG_SOFTWARE;
	  ADC_REG_InitStruct.SequencerLength = LL_ADC_REG_SEQ_SCAN_DISABLE;
	  ADC_REG_InitStruct.SequencerDiscont = LL_ADC_REG_SEQ_DISCONT_DISABLE;
	  ADC_REG_InitStruct.ContinuousMode = LL_ADC_REG_CONV_CONTINUOUS;
	  ADC_REG_InitStruct.DMATransfer = LL_ADC_REG_DMA_TRANSFER_NONE;
	  LL_ADC_REG_Init(ADC1, &ADC_REG_InitStruct);
	  /** Configure Regular Channel
	  */
	  LL_ADC_REG_SetSequencerRanks(ADC1, LL_ADC_REG_RANK_1, LL_ADC_CHANNEL_4);
	  LL_ADC_SetChannelSamplingTime(ADC1, LL_ADC_CHANNEL_4, LL_ADC_SAMPLINGTIME_1CYCLE_5);
	  /* USER CODE BEGIN ADC1_Init 2 */

	  /* USER CODE END ADC1_Init 2 */

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
#define FUNC 1
int main(void)
{
	soc_init();

	board_init();

	console_init();

	log_level_set(DEBUG);

	spi_init();

#if (FUNC == 0)
	sfdp_check();
#elif (FUNC == 1)
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
#endif

	return 0;
}
