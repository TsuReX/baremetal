
#include "drivers.h"
#include "adc.h"
#include "time.h"
#include "console.h"

#define PIO015_DIGIMODE_ANALOG 0x00u
#define PIO016_DIGIMODE_ANALOG 0x00u
#define PIO031_DIGIMODE_ANALOG 0x00u

#define PIO015_FUNC_ALT0 0x00u
#define PIO016_FUNC_ALT0 0x00u
#define PIO031_FUNC_ALT0 0x00u

static void adc0_gpio_init(void)
{
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

}

static int32_t adc0_calibrate()
{
    uint32_t tmp32   = ADC0->CTRL;
    /* The maximum ADC clock frequency during calibration is 30 MHz. */

    int32_t ret_val = 0;

    /* Enable the converter. */
    /* This bit should be set after at least 10 us after the ADC is powered on. */
    mdelay(1);
    /* This bit can only be set 1 by software. It is cleared automatically whenever the ADC is powered down. */
    ADC0->STARTUP = ADC_STARTUP_ADC_ENA_MASK;

    if (0UL == (ADC0->STARTUP & ADC_STARTUP_ADC_ENA_MASK)) {
        ret_val = 1; /* ADC is not powered up. */
    }

	/* The divider should round up to ensure the frequency be lower than the maximum frequency. */
	uint8_t divider = 180;
	/* Divide the system clock to yield an ADC clock of about 1 MHz. */
	ADC0->CTRL &= ~ADC_CTRL_CLKDIV_MASK;
	ADC0->CTRL |= ADC_CTRL_CLKDIV(divider - 1UL);

    /* Launch the calibration cycle or "dummy" conversions. */
    if (ADC_CALIB_CALREQD_MASK == (ADC0->CALIB & ADC_CALIB_CALREQD_MASK)) {
        /* Calibration is required, do it now. */
        ADC0->CALIB = ADC_CALIB_CALIB_MASK;

        /* A calibration cycle requires approximately 81 ADC clocks to complete. */
        mdelay(1);
        if (ADC_CALIB_CALIB_MASK == (ADC0->CALIB & ADC_CALIB_CALIB_MASK)) {
            ADC0->CTRL = tmp32;
            ret_val = 2; /* Calibration timeout. */
        }
    } else {
        /* If a calibration is not performed, launch the conversion cycle.  */
        ADC0->STARTUP |= ADC_STARTUP_ADC_INIT_MASK;

        /* A “dummy” conversion cycle requires approximately 6 ADC clocks */
        mdelay(1);
        if (ADC_STARTUP_ADC_INIT_MASK == (ADC0->STARTUP & ADC_STARTUP_ADC_INIT_MASK)) {
            ADC0->CTRL = tmp32;
            ret_val = 3; /* Initialization timeout. */
        }
    }

    ADC0->CTRL = tmp32;

    return ret_val;
}
void adc0_init(void)
{
	adc0_gpio_init();

	adc0_power_init();

	ADC0->CTRL |= ADC_CTRL_BYPASSCAL_MASK;

//	if (true == ADC_DoOffsetCalibration(ADC0, 180000000)) {
	if (true == adc0_calibrate()) {
		print("ADC0 calibrated.\r\n");
	} else {
		print("ADC0 calibration failed.\r\n");
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

	print("ADC0 initialized.\r\n");
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
