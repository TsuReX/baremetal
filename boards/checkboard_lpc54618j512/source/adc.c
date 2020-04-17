
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
    adc0_config.clockDividerNumber = 180;
    adc0_config.resolution = kADC_Resolution12bit;
    adc0_config.enableBypassCalibration = false;
    adc0_config.sampleTimeNumber = 0U;
    ADC_Init(ADC0, &adc0_config);

    adc_conv_seq_config_t adc0_convseq;
    adc0_convseq.channelMask = 0xFFF;
	adc0_convseq.triggerMask      = 0U;
	adc0_convseq.triggerPolarity  = kADC_TriggerPolarityPositiveEdge;
	adc0_convseq.enableSingleStep = false;
	adc0_convseq.enableSyncBypass = false;
	adc0_convseq.interruptMode    = kADC_InterruptForEachSequence;

	ADC_SetConvSeqAConfig(ADC0, &adc0_convseq);
	ADC_EnableConvSeqA(ADC0, true);
//	ADC_DoSoftwareTriggerConvSeqA(ADC0);

	print("Configuration Done.\r\n");
}

void adc0_convert()
{
	size_t channel = 0;

	for (; channel < ADC0_CHANNEL_COUNT; ++channel)
		(void)(ADC0->DAT[channel]);

	ADC_DoSoftwareTriggerConvSeqA(ADC0);
}

uint32_t adc0_get_value(uint32_t channel)
{
	if (channel >= ADC0_CHANNEL_COUNT)
		return 0xFFFFFFFF;

	return ((ADC0->DAT[channel] & ADC_DAT_RESULT_MASK) >> ADC_DAT_RESULT_SHIFT);
}
