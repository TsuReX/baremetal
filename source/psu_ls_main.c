/**
 * @file	src/psu_main.c
 *
 * @brief	Объявление точки вхождения в C-код.
 *
 * @author	Vasily Yurchenko <vasily.v.yurchenko@yandex.ru>
 */

#include <string.h>
#include "console.h"
#include "config.h"
#include "drivers.h"
#include "time.h"
#include "debug.h"

#define ADC_VREF_MILIVOLTS			(3300)
#define AMPL_VOLTS					(310)
#define DIVIDED_AMPL_MILIVOLTS		(2370)

#define VK_10			(((ADC_VREF_MILIVOLTS * 1000) / 4096) * ((AMPL_VOLTS * 1000) / DIVIDED_AMPL_MILIVOLTS))
#define VK_16			(((ADC_VREF_MILIVOLTS >> 2) * ((AMPL_VOLTS << 10) / DIVIDED_AMPL_MILIVOLTS)))
#define CONVERT_10(X)	(((uint32_t)X * VK_10) / 1000000)
#define CONVERT_16(X)	(((uint32_t)X * VK_16) >> 20)

struct command{
	uint8_t en;
	uint8_t data[7];
};

struct command command;

struct measurement {
	uint32_t vac;
	uint32_t vpfc;
};

struct measurement measurement;

void orange_led_on(void)
{
	LL_GPIO_SetOutputPin(GPIOB, LL_GPIO_PIN_8);
}
void orange_led_off(void)
{
	LL_GPIO_ResetOutputPin(GPIOB, LL_GPIO_PIN_8);
}
void green_led_on(void)
{
	LL_GPIO_SetOutputPin(GPIOB, LL_GPIO_PIN_9);
}
void green_led_off(void)
{
	LL_GPIO_ResetOutputPin(GPIOB, LL_GPIO_PIN_9);
}

void adc_init()
{
	LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOA);

	/** VAC */
	LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_0, LL_GPIO_MODE_ANALOG);

	/** VPFC */
	LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_1, LL_GPIO_MODE_ANALOG);

	LL_RCC_HSI14_Enable();
	mdelay(1);
	LL_APB1_GRP2_EnableClock(LL_APB1_GRP2_PERIPH_ADC1);
	mdelay(1);
	LL_ADC_SetClock(ADC1, LL_ADC_CLOCK_ASYNC);

	LL_ADC_Disable(ADC1);
	LL_ADC_StartCalibration(ADC1);
	while (LL_ADC_IsCalibrationOnGoing(ADC1));
	LL_ADC_ClearFlag_ADRDY(ADC1);
	LL_ADC_Enable(ADC1);
	LL_ADC_SetResolution(ADC1, LL_ADC_RESOLUTION_12B);

#if 1
	LL_ADC_REG_SetTriggerSource(ADC1, LL_ADC_REG_TRIG_SOFTWARE);
#else
	LL_ADC_REG_SetTriggerSource(ADC1, LL_ADC_REG_TRIG_EXT_TIM1_TRGO);
	LL_ADC_REG_GetTriggerEdge(ADC1, LL_ADC_REG_TRIG_EXT_RISING);
#endif

	LL_ADC_SetSamplingTimeCommonChannels(ADC1, LL_ADC_SAMPLINGTIME_1CYCLE_5);
	LL_ADC_REG_SetSequencerChannels(ADC1, LL_ADC_CHANNEL_0 | LL_ADC_CHANNEL_1);
	LL_ADC_REG_SetSequencerScanDirection(ADC1, LL_ADC_REG_SEQ_SCAN_DIR_FORWARD);
	LL_ADC_REG_SetContinuousMode(ADC1, LL_ADC_REG_CONV_SINGLE);
	LL_ADC_REG_SetSequencerDiscont(ADC1, LL_ADC_REG_SEQ_DISCONT_DISABLE);
	LL_ADC_REG_SetOverrun(ADC1, LL_ADC_REG_OVR_DATA_OVERWRITTEN);
	LL_ADC_SetLowPowerMode(ADC1, LL_ADC_LP_AUTOWAIT);
	LL_ADC_Enable(ADC1);
}

void comm_init(const void *dst_buffer, size_t dst_buffer_size)
{
	LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOA);

	/* USART1_TX */
//	LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_9, LL_GPIO_MODE_ALTERNATE);
//	LL_GPIO_SetAFPin_8_15(GPIOA, LL_GPIO_PIN_9, LL_GPIO_AF_1);
//	LL_GPIO_SetPinSpeed(GPIOA, LL_GPIO_PIN_9, LL_GPIO_SPEED_FREQ_HIGH);
//	LL_GPIO_SetPinOutputType(GPIOA, LL_GPIO_PIN_9, LL_GPIO_OUTPUT_PUSHPULL);
//	LL_GPIO_SetPinPull(GPIOA, LL_GPIO_PIN_9, LL_GPIO_PULL_UP);

	/* USART1_RX */
	LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_10, LL_GPIO_MODE_ALTERNATE);
	LL_GPIO_SetAFPin_8_15(GPIOA, LL_GPIO_PIN_10, LL_GPIO_AF_1);
	LL_GPIO_SetPinSpeed(GPIOA, LL_GPIO_PIN_10, LL_GPIO_SPEED_FREQ_HIGH);
//	LL_GPIO_SetPinOutputType(GPIOA, LL_GPIO_PIN_10, LL_GPIO_OUTPUT_PUSHPULL);
	LL_GPIO_SetPinPull(GPIOA, LL_GPIO_PIN_10, LL_GPIO_PULL_UP);

	/************************************************************************************************/
	LL_APB1_GRP2_EnableClock(LL_APB1_GRP2_PERIPH_USART1);

	LL_USART_ConfigCharacter(USART1, LL_USART_DATAWIDTH_8B, LL_USART_PARITY_NONE, LL_USART_STOPBITS_1);
	LL_USART_SetHWFlowCtrl(USART1, LL_USART_HWCONTROL_NONE);
	LL_USART_SetOverSampling(USART1, LL_USART_OVERSAMPLING_16);

	LL_USART_SetBaudRate(USART1, HCLK_FREQ >> 1, LL_USART_OVERSAMPLING_16, 1500000);

	LL_USART_EnableDMAReq_RX(USART1);

	LL_USART_EnableIT_RXNE(USART1);

	NVIC_SetPriority(USART1_IRQn, 0);
	NVIC_EnableIRQ(USART1_IRQn);

	LL_USART_Enable(USART1);
	/************************************************************************************************/

	LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_DMA1);
	/* Настройка канала приема. */
	LL_DMA_ConfigTransfer(DMA1, LL_DMA_CHANNEL_3,
								LL_DMA_DIRECTION_PERIPH_TO_MEMORY |
								LL_DMA_PRIORITY_HIGH              |
								LL_DMA_MODE_CIRCULAR              |
								LL_DMA_PERIPH_NOINCREMENT         |
								LL_DMA_MEMORY_INCREMENT           |
								LL_DMA_PDATAALIGN_BYTE            |
								LL_DMA_MDATAALIGN_BYTE);

	LL_DMA_ConfigAddresses(DMA1, LL_DMA_CHANNEL_3,
								(uint32_t)&(USART1->RDR),
								(uint32_t)dst_buffer,
								LL_DMA_DIRECTION_PERIPH_TO_MEMORY);

	LL_DMA_SetDataLength(DMA1, LL_DMA_CHANNEL_3, dst_buffer_size);

//	LL_SYSCFG_SetRemapDMA_USART(LL_SYSCFG_USART1RX_RMP_DMA1CH5);

	/** Генерация прерывания по приему всего буфера. */
	LL_DMA_EnableIT_TC(DMA1, LL_DMA_CHANNEL_3);

	NVIC_SetPriority(DMA1_Channel2_3_IRQn, 1);
	NVIC_EnableIRQ(DMA1_Channel2_3_IRQn);
}

void comm_start(void)
{
	LL_DMA_EnableChannel(DMA1, LL_DMA_CHANNEL_3);
	LL_USART_EnableDirectionRx(USART1);
}

void comm_stop(void)
{
	LL_DMA_DisableChannel(DMA1, LL_DMA_CHANNEL_3);
	LL_USART_DisableDirectionRx(USART1);
}

void usart1_irq_handler(void)
{
//	LL_GPIO_TogglePin(GPIOB, LL_GPIO_PIN_1);
//	LL_USART_ReceiveData8(USART1);
}

void dma1_channel2_3_irq_handler(void)
{
	/* USART1_RX */
	if (LL_DMA_IsActiveFlag_GI3(DMA1) == 1) {

		/* TODO: Place code here */

		LL_DMA_ClearFlag_HT3(DMA1);
		LL_DMA_ClearFlag_TC3(DMA1);
		LL_DMA_ClearFlag_TE3(DMA1);
		LL_DMA_ClearFlag_GI3(DMA1);
	}
}

void adc_start_convertion(void)
{
	LL_ADC_ClearFlag_ADRDY(ADC1);

	uint16_t enc_vac = 0;
	uint16_t enc_vpfc = 0;
	uint32_t vac = 0;
	uint32_t vpfc = 0;
	uint32_t max_vac = 0;
	uint32_t max_vpfc = 0;
	size_t sample_num = 0;
	const size_t sub_sample_count = 128;
	size_t sub_sample_num = 0;
	const size_t sample_count = 16;
	uint32_t vacs[sample_count];
	uint32_t vpfcs[sample_count];

	memset(vacs, 0, sizeof(vacs));
	memset(vpfcs, 0, sizeof(vpfcs));

	while (1) {
		LL_ADC_REG_StartConversion(ADC1);

		while (!LL_ADC_IsActiveFlag_EOC(ADC1));
		enc_vac = LL_ADC_REG_ReadConversionData12(ADC1);

		while (!LL_ADC_IsActiveFlag_EOC(ADC1));
		enc_vpfc = LL_ADC_REG_ReadConversionData12(ADC1);

		LL_ADC_ClearFlag_EOS(ADC1);

		vac = CONVERT_16(enc_vac);
		vpfc = CONVERT_16(enc_vpfc);

		udelay(100);

		if (sub_sample_num++ == sub_sample_count) {
			vacs[sample_num] = max_vac;
			vpfcs[sample_num] = max_vpfc;
			max_vac = 0;
			max_vpfc = 0;
			sub_sample_num = 0;
			sample_num++;
			sample_num %= sample_count;

			uint32_t sum_vac = 0;
			uint32_t sum_vpfc = 0;
			uint32_t avg_vac = 0;
			uint32_t avg_vpfc = 0;

			size_t i = 0;
			for (; i < sample_count; i++) {
				sum_vac += vacs[i];
				sum_vpfc += vpfcs[i];
			}

			avg_vac = sum_vac >> 4;
			avg_vpfc = sum_vpfc >> 4;

//			printk(INFO, "ENCVAC: %d, ENCVPFC: %d\r\n", enc_vac, enc_vpfc);

			console_write((uint8_t *)&avg_vac, sizeof(avg_vac), 1000);
			console_write((uint8_t *)&avg_vpfc, sizeof(avg_vpfc), 1000);

		} else {
			if (vac > max_vac)
				max_vac = vac;
			if (vpfc > max_vpfc)
				max_vpfc = vpfc;
		}
	}
}

/**
 * @brief	C-code entry point.
 *
 * @retval	0 in case of success, otherwise error code
 */
int main(void)
{
	/** Configure internal subsystems of SoC. */
	soc_init();

	/** Configure board's peripherals. */
	board_init();

	console_init();

	comm_init(&measurement, sizeof(struct measurement));

	comm_start();

	/* TODO: Implement LL_GPIO_PIN_11 high to low transition interrupt. */
	while (1) {
		while (1) {
			mdelay(100);
			if (LL_GPIO_IsInputPinSet(GPIOA, LL_GPIO_PIN_11) == 0) {
				command.en = 1;
				console_write((uint8_t *)&command, sizeof(struct command), 1000);
				break;
			}
		}
		while (1) {
			mdelay(100);
			if (LL_GPIO_IsInputPinSet(GPIOA, LL_GPIO_PIN_11) == 1) {
				command.en = 0;
				console_write((uint8_t *)&command, sizeof(struct command), 1000);
				break;
			}
		}
	}
}
