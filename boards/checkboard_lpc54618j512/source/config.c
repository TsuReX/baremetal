/**
 * @file	src/config.c
 *
 * @brief	Определения функций инициализации SoC и периферии.
 *
 * @author	Vasily Yurchenko <vasily.v.yurchenko@yandex.ru>
 */

#include "drivers.h"
#include "config.h"

#define MAIN_CLK_180MHZ 180000000

/**
 * @brief	Настраивает внутреннюю флеш память для корректного взаимодействия с ядром,
 * 			работающем на частоте 48 МГц.
 */
static void flash_config(void)
{
	SYSCON->FLASHCFG = SYSCON->FLASHCFG | ((uint32_t)kCLOCK_Flash9Cycle << SYSCON_FLASHCFG_FLASHTIM_SHIFT);
}

static void pll_config(const pll_setup_t *pSetup)
{
    if ((SYSCON->SYSPLLCLKSEL & SYSCON_SYSPLLCLKSEL_SEL_MASK) == 0x01U)
    {
        /* Turn on the ext clock if system pll input select clk_in */
        SYSCON->PDRUNCFGCLR[0] |= SYSCON_PDRUNCFG_PDEN_VD2_ANA_MASK;
        SYSCON->PDRUNCFGCLR[1] |= SYSCON_PDRUNCFG_PDEN_SYSOSC_MASK;
    }
    /* Enable power VD3 for PLLs */
    POWER_SetPLL();
    /* Power off PLL during setup changes */
    POWER_EnablePD(kPDRUNCFG_PD_SYS_PLL0);

    /* Write PLL setup data */
    SYSCON->SYSPLLCTRL = pSetup->pllctrl;
    SYSCON->SYSPLLNDEC = pSetup->pllndec;
    SYSCON->SYSPLLNDEC = pSetup->pllndec | (1UL << SYSCON_SYSPLLNDEC_NREQ_SHIFT); /* latch */
    SYSCON->SYSPLLPDEC = pSetup->pllpdec;
    SYSCON->SYSPLLPDEC = pSetup->pllpdec | (1UL << SYSCON_SYSPLLPDEC_PREQ_SHIFT); /* latch */
    SYSCON->SYSPLLMDEC = pSetup->pllmdec;
    SYSCON->SYSPLLMDEC = pSetup->pllmdec | (1UL << SYSCON_SYSPLLMDEC_MREQ_SHIFT); /* latch */

    /* Flags for lock or power on */
    if ((pSetup->flags & (PLL_SETUPFLAG_POWERUP | PLL_SETUPFLAG_WAITLOCK)) != 0U)
    {
        /* If turning the PLL back on, perform the following sequence to accelerate PLL lock */
        uint32_t maxCCO    = (1UL << 18U) | 0x5dd2U; /* CCO = 1.6Ghz + MDEC enabled*/
        uint32_t curSSCTRL = SYSCON->SYSPLLMDEC & ~(1UL << 17U);

        /* Initialize  and power up PLL */
        SYSCON->SYSPLLMDEC = maxCCO;
        POWER_DisablePD(kPDRUNCFG_PD_SYS_PLL0);

        /* Set mreq to activate */
        SYSCON->SYSPLLMDEC = maxCCO | (1UL << 17U);

        /* Delay for 72 uSec @ 12Mhz */
        uint32_t count  = 72 * 12; //72 * 12000000 / 1000000;
        __ASM volatile("    MOV    R0, %0" : : "r"(count));
        __ASM volatile(
            "loop:                          \n"
    #if defined(__GNUC__) && !defined(__ARMCC_VERSION)
            "    SUB    R0, R0, #1          \n"
    #else
            "    SUBS   R0, R0, #1          \n"
    #endif
            "    CMP    R0, #0              \n"

            "    BNE    loop                \n");

        /* clear mreq to prepare for restoring mreq */
        SYSCON->SYSPLLMDEC = curSSCTRL;

        /* set original value back and activate */
        SYSCON->SYSPLLMDEC = curSSCTRL | (1UL << 17U);

        /* Enable peripheral states by setting low */
        POWER_DisablePD(kPDRUNCFG_PD_SYS_PLL0);
    }
    if ((pSetup->flags & PLL_SETUPFLAG_WAITLOCK) != 0U)
    {
        while (((SYSCON->SYSPLLSTAT & SYSCON_SYSPLLSTAT_LOCK_MASK) != 0U) == false)
        {
        }
    }

    /* Update current programmed PLL rate var */
//    s_Pll_Freq = pSetup->pllRate;

}

static int32_t fro_config(uint32_t iFreq)
{
    uint32_t usb_adj;
    if ((iFreq != 12000000U) && (iFreq != 48000000U) && (iFreq != 96000000U))
    {
        return kStatus_Fail;
    }
    /* Power up the FRO and set this as the base clock */
    POWER_DisablePD(kPDRUNCFG_PD_FRO_EN);
    /* back up the value of whether USB adj is selected, in which case we will have a value of 1 else 0 */
    usb_adj = ((SYSCON->FROCTRL) & SYSCON_FROCTRL_USBCLKADJ_MASK) >> SYSCON_FROCTRL_USBCLKADJ_SHIFT;

    if (iFreq > 12000000U)
    {
        /* Call ROM API to set FRO */
        set_fro_frequency(iFreq);
        if (iFreq == 96000000U)
        {
            SYSCON->FROCTRL |= (SYSCON_FROCTRL_SEL(1) | SYSCON_FROCTRL_WRTRIM(1) | SYSCON_FROCTRL_USBCLKADJ(usb_adj) |
                                SYSCON_FROCTRL_HSPDCLK(1));
        }
        else
        {
            SYSCON->FROCTRL |= (SYSCON_FROCTRL_SEL(0) | SYSCON_FROCTRL_WRTRIM(1) | SYSCON_FROCTRL_USBCLKADJ(usb_adj) |
                                SYSCON_FROCTRL_HSPDCLK(1));
        }
    }
    else
    {
        SYSCON->FROCTRL &= ~SYSCON_FROCTRL_HSPDCLK(1);
    }

    return kStatus_Success;
}

/**
 * @brief	Производит настройку источников тактирования
 *
 */
static void rcc_config(void)
{
	POWER_DisablePD(kPDRUNCFG_PD_FRO_EN);  /*!< Ensure FRO is on  */

	SYSCON->MAINCLKSELA = SYSCON_MAINCLKSELA_SEL(0U);

	POWER_DisablePD(kPDRUNCFG_PD_SYS_OSC); /*!< Enable System Oscillator Power */

	SYSCON->SYSOSCCTRL = SYSCON_SYSOSCCTRL_FREQRANGE(0U); /*!< Set system oscillator range */

	POWER_SetVoltageForFreq(180000000U); /*!< Set voltage for the one of the fastest clock outputs: System clock output */

	SYSCON->SYSPLLCLKSEL = SYSCON_SYSPLLCLKSEL_SEL(0x0);

	const pll_setup_t pllSetup = {
		.pllctrl = SYSCON_SYSPLLCTRL_SELI(32U) | SYSCON_SYSPLLCTRL_SELP(16U) | SYSCON_SYSPLLCTRL_SELR(0U),
		.pllmdec = (SYSCON_SYSPLLMDEC_MDEC(8191U)),
		.pllndec = (SYSCON_SYSPLLNDEC_NDEC(770U)),
		.pllpdec = (SYSCON_SYSPLLPDEC_PDEC(98U)),
		.pllRate = 180000000U,
		.flags   = PLL_SETUPFLAG_WAITLOCK | PLL_SETUPFLAG_POWERUP};

	pll_config(&pllSetup);         /*!< Configure PLL to the desired value */

	fro_config(96000000U); /*!< Set up high frequency FRO output to selected frequency */

	SYSCON->SCTCLKSEL = SYSCON_SCTCLKSEL_SEL(0x2);

    SYSCON->SCTCLKDIV = SYSCON_SCTCLKDIV_DIV(0x0);

    SYSCON->AHBCLKDIV = SYSCON_AHBCLKDIV_DIV(0x0);

    SYSCON->MAINCLKSELB = SYSCON_MAINCLKSELB_SEL(0x2);

	SYSCON->MAINCLKSELA = SYSCON_MAINCLKSELA_SEL(0x0); /*!< Switch MAINCLKSELA to FRO12M even it is not used for MAINCLKSELB */

}

/**
 * @brief	Наcтраивает системный таймер ядра (SysTick)
 *
 * @param	main_clk	частота шины ядра и системной шины в герцах
 */
static void systick_config(uint32_t main_clk)
{
	/* Производится настройка системного таймера ядра для определения интервала времени равного 1 миллисекунде.  */
	SysTick_Config(main_clk / 1000);
	SysTick->CTRL  &= ~SysTick_CTRL_TICKINT_Msk;
}

/**
 * @brief	Настройка устройств платформы(платы)
 */
void board_config(void)
{
	/* attach 12 MHz clock to FLEXCOMM0 (debug console) */
	SYSCON->FCLKSEL[0] = SYSCON_AHBCLKCTRL_FLEXCOMM0(0x0);

    CLOCK_EnableClock(kCLOCK_Iocon);
	CLOCK_EnableClock(kCLOCK_Gpio1);
	CLOCK_EnableClock(kCLOCK_Gpio3);
	CLOCK_EnableClock(kCLOCK_Gpio5);
    CLOCK_EnableClock(kCLOCK_Adc0); /* SYSCON->AHBCLKCTRL[0] |= SYSCON_AHBCLKCTRL_ADC0_MASK; */
    CLOCK_EnableClock(kCLOCK_Sct0);

    const uint32_t port3_pin0_config = (IOCON_FUNC0 | IOCON_MODE_INACT | IOCON_DIGITAL_EN | IOCON_INPFILT_OFF);
    IOCON_PinMuxSet(IOCON, 3U, 0U, port3_pin0_config);

    const uint32_t port3_pin2_config = (IOCON_FUNC0 | IOCON_MODE_INACT | IOCON_DIGITAL_EN | IOCON_INPFILT_OFF);
    IOCON_PinMuxSet(IOCON, 3U, 2U, port3_pin2_config);

    const uint32_t port5_pin0_config = (IOCON_FUNC0 | IOCON_MODE_INACT | IOCON_DIGITAL_EN | IOCON_INPFILT_OFF);
    IOCON_PinMuxSet(IOCON, 5U,0U, port5_pin0_config);

    const uint32_t port5_pin1_config = (IOCON_FUNC0 | IOCON_MODE_INACT | IOCON_DIGITAL_EN | IOCON_INPFILT_OFF);
    IOCON_PinMuxSet(IOCON, 5U, 1U, port5_pin1_config);

    GPIO_PortInit(GPIO, 3);
    GPIO_PortInit(GPIO, 5);

    gpio_pin_config_t hl1 = { kGPIO_DigitalOutput, 0};
    GPIO_PinInit(GPIO, 5, 0, &hl1);

    gpio_pin_config_t hl2 = { kGPIO_DigitalOutput, 0};
    GPIO_PinInit(GPIO, 5, 1, &hl2);

    gpio_pin_config_t pwr_no_bp_n = { kGPIO_DigitalOutput, 1};
    GPIO_PinInit(GPIO, 3, 0, &pwr_no_bp_n);

    gpio_pin_config_t pwr_on_spl_n = { kGPIO_DigitalOutput, 1};
    GPIO_PinInit(GPIO, 3, 2, &pwr_on_spl_n);
}

/**
 * @brief	Настройка внутренних подсистем системы на кристалле.
 */
void soc_config(void)
{
	/* Настройка внутренней флеш памяти. */
	flash_config();
	/* Настройка подсистемы тактирования. */
	rcc_config();
	/* Настраивает системный таймер ядра. */
	systick_config(MAIN_CLK_180MHZ);
}

