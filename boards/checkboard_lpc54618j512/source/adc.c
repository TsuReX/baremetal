
#include "drivers.h"
#include "adc.h"
#include "console.h"

#define PIO015_DIGIMODE_ANALOG 0x00u
#define PIO016_DIGIMODE_ANALOG 0x00u
#define PIO031_DIGIMODE_ANALOG 0x00u

#define PIO015_FUNC_ALT0 0x00u
#define PIO016_FUNC_ALT0 0x00u
#define PIO031_FUNC_ALT0 0x00u

static void adc0_gpio_init(void)
{
    CLOCK_EnableClock(kCLOCK_Iocon);

    IOCON->PIO[0][15] = ((IOCON->PIO[0][15] &
							(~(IOCON_PIO_FUNC_MASK | IOCON_PIO_MODE_MASK | IOCON_PIO_DIGIMODE_MASK)))
							| IOCON_PIO_FUNC(PIO015_FUNC_ALT0)
							| IOCON_PIO_DIGIMODE(PIO015_DIGIMODE_ANALOG));

    IOCON->PIO[0][16] = ((IOCON->PIO[0][16] &
							(~(IOCON_PIO_FUNC_MASK | IOCON_PIO_MODE_MASK | IOCON_PIO_DIGIMODE_MASK)))
							| IOCON_PIO_FUNC(PIO016_FUNC_ALT0)
							| IOCON_PIO_DIGIMODE(PIO016_DIGIMODE_ANALOG));

    IOCON->PIO[0][31] = ((IOCON->PIO[0][31] &
							(~(IOCON_PIO_FUNC_MASK | IOCON_PIO_MODE_MASK | IOCON_PIO_DIGIMODE_MASK)))
							| IOCON_PIO_FUNC(PIO031_FUNC_ALT0)
							| IOCON_PIO_DIGIMODE(PIO031_DIGIMODE_ANALOG));
}

static void adc0_power_init(void)
{
    /* SYSCON power. */
    POWER_DisablePD(kPDRUNCFG_PD_VDDA);    /* Power on VDDA. */
    POWER_DisablePD(kPDRUNCFG_PD_ADC0);    /* Power on the ADC converter. */
    POWER_DisablePD(kPDRUNCFG_PD_VD2_ANA); /* Power on the analog power supply. */
    POWER_DisablePD(kPDRUNCFG_PD_VREFP);   /* Power on the reference voltage source. */
    POWER_DisablePD(kPDRUNCFG_PD_TS);      /* Power on the temperature sensor. */

    CLOCK_EnableClock(kCLOCK_Adc0); /* SYSCON->AHBCLKCTRL[0] |= SYSCON_AHBCLKCTRL_ADC0_MASK; */
}

void adc0_init(void)
{
	adc0_gpio_init();
	adc0_power_init();

	ADC0->CTRL |= ADC_CTRL_BYPASSCAL_MASK;
	uint32_t frequency = CLOCK_GetFreq(kCLOCK_BusClk);
	if (true == ADC_DoOffsetCalibration(ADC0, frequency)) {
		print("ADC Calibration Done.\r\n");
	} else {
		print("ADC Calibration Failed.\r\n");
	}

    adc_config_t adc0_config;
    adc0_config.clockMode = kADC_ClockSynchronousMode;
    adc0_config.clockDividerNumber = 2;
    adc0_config.resolution = kADC_Resolution12bit;
    adc0_config.enableBypassCalibration = false;
    adc0_config.sampleTimeNumber = 0U;
    ADC_Init(ADC0, &adc0_config);

    adc_conv_seq_config_t adc0_convseq;
    adc0_convseq.channelMask = 0x0FFF;
	adc0_convseq.triggerMask      = 0U;
	adc0_convseq.triggerPolarity  = kADC_TriggerPolarityPositiveEdge;
	adc0_convseq.enableSingleStep = false;
	adc0_convseq.enableSyncBypass = false;
	adc0_convseq.interruptMode    = kADC_InterruptForEachSequence;

	ADC_SetConvSeqAConfig(ADC0, &adc0_convseq);
	ADC_EnableConvSeqA(ADC0, true);
	ADC_DoSoftwareTriggerConvSeqA(ADC0);

//	adc_result_info_t adcResultInfoStruct;
//	ADC_GetChannelConversionResult(ADC0, 3, &adcResultInfoStruct);
//	ADC_GetChannelConversionResult(ADC0, 4, &adcResultInfoStruct);
//	ADC_GetChannelConversionResult(ADC0, 5, &adcResultInfoStruct);
//	ADC_GetConvSeqAGlobalConversionResult(ADC0, &adcResultInfoStruct);



	print("Configuration Done.\r\n");
}

uint32_t adc0_3_getval(void)
{
	adc_result_info_t adcResultInfoStruct;
	ADC_DoSoftwareTriggerConvSeqA(ADC0);

	/* Wait for the converter to be done. */
	while (!ADC_GetChannelConversionResult(ADC0, 3, &adcResultInfoStruct)) {
	}
	return adcResultInfoStruct.result;
}

uint32_t adc0_4_getval(void)
{
	adc_result_info_t adcResultInfoStruct;
	ADC_DoSoftwareTriggerConvSeqA(ADC0);

	/* Wait for the converter to be done. */
	while (!ADC_GetChannelConversionResult(ADC0, 4, &adcResultInfoStruct)) {
	}
	return adcResultInfoStruct.result;
}

uint32_t adc0_5_getval(void)
{
	adc_result_info_t adcResultInfoStruct;
	ADC_DoSoftwareTriggerConvSeqA(ADC0);

	/* Wait for the converter to be done. */
	while (!ADC_GetChannelConversionResult(ADC0, 5, &adcResultInfoStruct)) {
	}
	return adcResultInfoStruct.result;
}
