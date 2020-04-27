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
	/* Подключение тактирования для шины FLEXCOMM0 (используется для USART0). */
	SYSCON->FCLKSEL[0] = SYSCON_FCLKSEL_SEL(0x0);

	/* Подключение тактирования для системы управления входами/выходами GPIO. */
    SYSCON->AHBCLKCTRLSET[0] = SYSCON_AHBCLKCTRL_IOCON(1);

	/* GPIO 0 ***********************************/
    /* Настройка входов/выходов GPIO0. */
    IOCON->PIO[A_3V3_BP_ADC_PORT][A_3V3_BP_ADC_PIN] = IOCON_PIO_FUNC(0x0) | IOCON_PIO_DIGIMODE(0x0);
    IOCON->PIO[A_5V_BP_ADC_PORT][A_5V_BP_ADC_PIN] = IOCON_PIO_FUNC(0x0) | IOCON_PIO_DIGIMODE(0x0);
    IOCON->PIO[A_12V_BP_ADC_PORT][A_12V_BP_ADC_PIN] = IOCON_PIO_FUNC(0x0) | IOCON_PIO_DIGIMODE(0x0);

    /* Подключение тактирования для GPIO0. */
    SYSCON->AHBCLKCTRLSET[0] = SYSCON_AHBCLKCTRL_GPIO0(1);

    /* Установить GPIO0 в состояние сброса. */
	SYSCON->PRESETCTRLSET[0] = SYSCON_PRESETCTRL_GPIO0_RST(1);
    while (0u == (SYSCON->PRESETCTRL[0] & SYSCON_PRESETCTRL_GPIO0_RST(1))) {
    }

    /* Вывести GPIO0 из состояния сброса. */
    SYSCON->PRESETCTRLCLR[0] = SYSCON_PRESETCTRL_GPIO0_RST(1);
    while (SYSCON_PRESETCTRL_GPIO0_RST(1) == (SYSCON->PRESETCTRL[0] & SYSCON_PRESETCTRL_GPIO0_RST(1))) {
    }

    /* GPIO 1 ***********************************/
    /* Настройка входов/выходов GPIO1. */
    IOCON->PIO[D_RX_UC_TX_USB_PORT][D_RX_UC_TX_USB_PIN] = IOCON_PIO_FUNC(0x1) | IOCON_PIO_DIGIMODE(0x1) | IOCON_PIO_FILTEROFF(1);
    IOCON->PIO[D_TX_UC_RX_USB_PORT][D_TX_UC_RX_USB_PIN] = IOCON_PIO_FUNC(0x1) | IOCON_PIO_DIGIMODE(0x1) | IOCON_PIO_FILTEROFF(1);

    IOCON->PIO[I_CPU1_PH1_PORT][I_CPU1_PH1_PIN] = IOCON_PIO_FUNC(0x0) | IOCON_PIO_DIGIMODE(0x1) | IOCON_PIO_FILTEROFF(1);
    IOCON->PIO[I_CPU1_PH2_PORT][I_CPU1_PH2_PIN] = IOCON_PIO_FUNC(0x0) | IOCON_PIO_DIGIMODE(0x1) | IOCON_PIO_FILTEROFF(1);
    IOCON->PIO[I_CPU1_PH3_PORT][I_CPU1_PH3_PIN] = IOCON_PIO_FUNC(0x0) | IOCON_PIO_DIGIMODE(0x1) | IOCON_PIO_FILTEROFF(1);
    IOCON->PIO[I_CPU1_PH4_PORT][I_CPU1_PH4_PIN] = IOCON_PIO_FUNC(0x0) | IOCON_PIO_DIGIMODE(0x1) | IOCON_PIO_FILTEROFF(1);
    IOCON->PIO[I_CPU1_PH5_PORT][I_CPU1_PH5_PIN] = IOCON_PIO_FUNC(0x0) | IOCON_PIO_DIGIMODE(0x1) | IOCON_PIO_FILTEROFF(1);
    IOCON->PIO[I_CPU1_PH6_PORT][I_CPU1_PH6_PIN] = IOCON_PIO_FUNC(0x0) | IOCON_PIO_DIGIMODE(0x1) | IOCON_PIO_FILTEROFF(1);

    /* Подключение тактирования для GPIO1. */
    SYSCON->AHBCLKCTRLSET[0] = SYSCON_AHBCLKCTRL_GPIO1(1);

    /* Установить GPIO1 в состояние сброса. */
	SYSCON->PRESETCTRLSET[0] = SYSCON_PRESETCTRL_GPIO1_RST(1);
    while (0u == (SYSCON->PRESETCTRL[0] & SYSCON_PRESETCTRL_GPIO1_RST(1))) {
    }

    /* Вывести GPIO1 из состояния сброса. */
    SYSCON->PRESETCTRLCLR[0] = SYSCON_PRESETCTRL_GPIO1_RST(1);
    while (SYSCON_PRESETCTRL_GPIO1_RST(1) == (SYSCON->PRESETCTRL[0] & SYSCON_PRESETCTRL_GPIO1_RST(1))) {
    }

    /* GPIO 2 ***********************************/
    /* Настройка входов/выходов GPIO2. */
    IOCON->PIO[A_MUX4_OHM_ADC_PORT][A_MUX4_OHM_ADC_PIN] = IOCON_PIO_FUNC(0x0) | IOCON_PIO_DIGIMODE(0x0);
    IOCON->PIO[A_MUX4_VOLT_ADC_PORT][A_MUX4_VOLT_ADC_PIN] = IOCON_PIO_FUNC(0x0) | IOCON_PIO_DIGIMODE(0x0);

    IOCON->PIO[O_VOLT_OHM_N_PORT][O_VOLT_OHM_N_PIN] = IOCON_PIO_FUNC(0x0) | IOCON_PIO_DIGIMODE(0x1) | IOCON_PIO_FILTEROFF(1);

    IOCON->PIO[O_MUX_ADDR_0_PORT][O_MUX_ADDR_0_PIN] = IOCON_PIO_FUNC(0x0) | IOCON_PIO_DIGIMODE(0x1);
    IOCON->PIO[O_MUX_ADDR_1_PORT][O_MUX_ADDR_1_PIN] = IOCON_PIO_FUNC(0x0) | IOCON_PIO_DIGIMODE(0x1);
    IOCON->PIO[O_MUX_ADDR_2_PORT][O_MUX_ADDR_2_PIN] = IOCON_PIO_FUNC(0x0) | IOCON_PIO_DIGIMODE(0x1);
    IOCON->PIO[O_MUX_ADDR_3_PORT][O_MUX_ADDR_3_PIN] = IOCON_PIO_FUNC(0x0) | IOCON_PIO_DIGIMODE(0x1);
    IOCON->PIO[O_MUX_ADDR_4_PORT][O_MUX_ADDR_4_PIN] = IOCON_PIO_FUNC(0x0) | IOCON_PIO_DIGIMODE(0x1);

    IOCON->PIO[O_EN_MUX1_PORT][O_EN_MUX1_PIN] = IOCON_PIO_FUNC(0x0) | IOCON_PIO_DIGIMODE(0x1);
    IOCON->PIO[O_EN_MUX2_PORT][O_EN_MUX2_PIN] = IOCON_PIO_FUNC(0x0) | IOCON_PIO_DIGIMODE(0x1);
    IOCON->PIO[O_EN_MUX3_PORT][O_EN_MUX3_PIN] = IOCON_PIO_FUNC(0x0) | IOCON_PIO_DIGIMODE(0x1);

    IOCON->PIO[I_CPU0_PH1_PORT][I_CPU0_PH1_PIN] = IOCON_PIO_FUNC(0x0) | IOCON_PIO_DIGIMODE(0x1) | IOCON_PIO_FILTEROFF(1);
    IOCON->PIO[I_CPU0_PH2_PORT][I_CPU0_PH2_PIN] = IOCON_PIO_FUNC(0x0) | IOCON_PIO_DIGIMODE(0x1) | IOCON_PIO_FILTEROFF(1);
    IOCON->PIO[I_CPU0_PH3_PORT][I_CPU0_PH3_PIN] = IOCON_PIO_FUNC(0x0) | IOCON_PIO_DIGIMODE(0x1) | IOCON_PIO_FILTEROFF(1);
    IOCON->PIO[I_CPU0_PH4_PORT][I_CPU0_PH4_PIN] = IOCON_PIO_FUNC(0x0) | IOCON_PIO_DIGIMODE(0x1) | IOCON_PIO_FILTEROFF(1);
    IOCON->PIO[I_CPU0_PH5_PORT][I_CPU0_PH5_PIN] = IOCON_PIO_FUNC(0x0) | IOCON_PIO_DIGIMODE(0x1) | IOCON_PIO_FILTEROFF(1);
    IOCON->PIO[I_CPU0_PH6_PORT][I_CPU0_PH6_PIN] = IOCON_PIO_FUNC(0x0) | IOCON_PIO_DIGIMODE(0x1) | IOCON_PIO_FILTEROFF(1);

    /* Подключение тактирования для GPIO2. */
    SYSCON->AHBCLKCTRLSET[0] = SYSCON_AHBCLKCTRL_GPIO2(1);

    /* Установить GPIO2 в состояние сброса. */
	SYSCON->PRESETCTRLSET[0] = SYSCON_PRESETCTRL_GPIO2_RST(1);
    while (0u == (SYSCON->PRESETCTRL[0] & SYSCON_PRESETCTRL_GPIO2_RST(1))) {
    }

    /* Вывести GPIO2 из состояния сброса. */
    SYSCON->PRESETCTRLCLR[0] = SYSCON_PRESETCTRL_GPIO2_RST(1);
    while (SYSCON_PRESETCTRL_GPIO2_RST(1) == (SYSCON->PRESETCTRL[0] & SYSCON_PRESETCTRL_GPIO2_RST(1))) {
    }

    /* Установить уровни на выходах GPIO2. */
    GPIO->DIR[2] = 0x0; /* Настроить все контакты порта, как входы. */

    GPIO->DIR[O_EN_MUX1_PORT] |= 0X1 << O_EN_MUX1_PIN; /* Настроить контакт, как выход. */
    GPIO->CLR[O_EN_MUX1_PORT] = 0X1 << O_EN_MUX1_PIN; /* Установить 0. */
    GPIO->DIR[O_EN_MUX2_PORT] |= 0X1 << O_EN_MUX2_PIN;
    GPIO->CLR[O_EN_MUX2_PORT] = 0X1 << O_EN_MUX2_PIN;
    GPIO->DIR[O_EN_MUX3_PORT] |= 0X1 << O_EN_MUX3_PIN;
    GPIO->CLR[O_EN_MUX3_PORT] = 0X1 << O_EN_MUX3_PIN;

    GPIO->DIR[O_MUX_ADDR_0_PORT] |= 0X1 << O_MUX_ADDR_0_PIN;
    GPIO->CLR[O_MUX_ADDR_0_PORT] = 0X1 << O_MUX_ADDR_0_PIN;
    GPIO->DIR[O_MUX_ADDR_1_PORT] |= 0X1 << O_MUX_ADDR_1_PIN;
    GPIO->CLR[O_MUX_ADDR_1_PORT] = 0X1 << O_MUX_ADDR_1_PIN;
    GPIO->DIR[O_MUX_ADDR_2_PORT] |= 0X1 << O_MUX_ADDR_2_PIN;
    GPIO->CLR[O_MUX_ADDR_2_PORT] = 0X1 << O_MUX_ADDR_2_PIN;
    GPIO->DIR[O_MUX_ADDR_3_PORT] |= 0X1 << O_MUX_ADDR_3_PIN;
    GPIO->CLR[O_MUX_ADDR_3_PORT] = 0X1 << O_MUX_ADDR_3_PIN;
    GPIO->DIR[O_MUX_ADDR_4_PORT] |= 0X1 << O_MUX_ADDR_4_PIN;
    GPIO->CLR[O_MUX_ADDR_4_PORT] = 0X1 << O_MUX_ADDR_4_PIN;

    /* GPIO 3 ***********************************/
    /* Настройка входов/выходов GPIO3. */
    IOCON->PIO[O_PWR_ON_BP_N_PORT][O_PWR_ON_BP_N_PIN] = IOCON_PIO_FUNC(0x0) | IOCON_PIO_DIGIMODE(0x1);
    IOCON->PIO[I_PWR_GD_BP_N_PORT][I_PWR_GD_BP_N_PIN] = IOCON_PIO_FUNC(0x0) | IOCON_PIO_DIGIMODE(0x1) | IOCON_PIO_FILTEROFF(1);
    IOCON->PIO[I_PWR_ON_SPL_N_PORT][I_PWR_ON_BP_N_PIN] = IOCON_PIO_FUNC(0x0) | IOCON_PIO_DIGIMODE(0x1) | IOCON_PIO_FILTEROFF(1);
    IOCON->PIO[O_SYSPL_STBY_ON_PORT][O_SYSPL_STBY_ON_PIN] = IOCON_PIO_FUNC(0x0) | IOCON_PIO_DIGIMODE(0x1);
    IOCON->PIO[O_SYSPL_ON_PORT][O_SYSPL_ON_PIN] = IOCON_PIO_FUNC(0x0) | IOCON_PIO_DIGIMODE(0x1);
    IOCON->PIO[O_SYSPL_CPU0_ON_PORT][O_SYSPL_CPU0_ON_PIN] = IOCON_PIO_FUNC(0x0) | IOCON_PIO_DIGIMODE(0x1);
    IOCON->PIO[O_SYSPL_CPU1_ON_PORT][O_SYSPL_CPU1_ON_PIN] = IOCON_PIO_FUNC(0x0) | IOCON_PIO_DIGIMODE(0x1);
    IOCON->PIO[O_MUX5_ADDR_A0_PORT][O_MUX5_ADDR_A0_PIN] = IOCON_PIO_FUNC(0x0) | IOCON_PIO_DIGIMODE(0x1);
    IOCON->PIO[O_MUX5_ADDR_A1_PORT][O_MUX5_ADDR_A1_PIN] = IOCON_PIO_FUNC(0x0) | IOCON_PIO_DIGIMODE(0x1);
    IOCON->PIO[A_MUX5_OHM_ADC_PORT][A_MUX5_OHM_ADC_PIN] = IOCON_PIO_FUNC(0x0) | IOCON_PIO_DIGIMODE(0x0);
    IOCON->PIO[A_MUX6_OHM_ADC_PORT][A_MUX6_OHM_ADC_PIN] = IOCON_PIO_FUNC(0x0) | IOCON_PIO_DIGIMODE(0x0);
    IOCON->PIO[O_MUX_ADDR_0_PORT][O_MUX_ADDR_0_PIN] = IOCON_PIO_FUNC(0x0) | IOCON_PIO_DIGIMODE(0x1);
    IOCON->PIO[O_MUX_ADDR_1_PORT][O_MUX_ADDR_1_PIN] = IOCON_PIO_FUNC(0x0) | IOCON_PIO_DIGIMODE(0x1);
    IOCON->PIO[O_MUX_ADDR_2_PORT][O_MUX_ADDR_2_PIN] = IOCON_PIO_FUNC(0x0) | IOCON_PIO_DIGIMODE(0x1);
    IOCON->PIO[O_EN_MUX6_PORT][O_EN_MUX6_PIN] = IOCON_PIO_FUNC(0x0) | IOCON_PIO_DIGIMODE(0x1) | IOCON_PIO_FILTEROFF(1);

    /* Подключение тактирования для GPIO3. */
    SYSCON->AHBCLKCTRLSET[0] = SYSCON_AHBCLKCTRL_GPIO3(1);

    /* Установить GPIO3 в состояние сброса. */
	SYSCON->PRESETCTRLSET[0] = SYSCON_PRESETCTRL_GPIO3_RST(1);
    while (0u == (SYSCON->PRESETCTRL[0] & SYSCON_PRESETCTRL_GPIO3_RST(1))) {
    }

    /* Вывести GPIO3 из состояния сброса. */
    SYSCON->PRESETCTRLCLR[0] = SYSCON_PRESETCTRL_GPIO3_RST(1);
    while (SYSCON_PRESETCTRL_GPIO3_RST(1) == (SYSCON->PRESETCTRL[0] & SYSCON_PRESETCTRL_GPIO3_RST(1))) {
    }

    /* Установить уровни на выходах GPIO3. */
    GPIO->DIR[3] = 0x0; /* Настроить все контакты порта, как входы. */

    GPIO->DIR[O_PWR_ON_BP_N_PORT] |= 0X1 << O_PWR_ON_BP_N_PIN;
    GPIO->SET[O_PWR_ON_BP_N_PORT] = 0X1 << O_PWR_ON_BP_N_PIN; /* Установить 1. */

    GPIO->DIR[O_SYSPL_STBY_ON_PORT] |= 0X1 << O_SYSPL_STBY_ON_PIN;
    GPIO->CLR[O_SYSPL_STBY_ON_PORT] = 0X1 << O_SYSPL_STBY_ON_PIN; /* Установить 0. */

    GPIO->DIR[O_SYSPL_ON_PORT] |= 0X1 << O_SYSPL_ON_PIN;
    GPIO->CLR[O_SYSPL_ON_PORT] = 0X1 << O_SYSPL_ON_PIN;

    GPIO->DIR[O_SYSPL_CPU0_ON_PORT] |= 0X1 << O_SYSPL_CPU0_ON_PIN;
    GPIO->CLR[O_SYSPL_CPU0_ON_PORT] = 0X1 << O_SYSPL_CPU0_ON_PIN;

    GPIO->DIR[O_SYSPL_CPU1_ON_PORT] |= 0X1 << O_SYSPL_CPU1_ON_PIN;
    GPIO->CLR[O_SYSPL_CPU1_ON_PORT] = 0X1 << O_SYSPL_CPU1_ON_PIN;

    GPIO->DIR[O_MUX5_ADDR_A0_PORT] |= 0X1 << O_MUX5_ADDR_A0_PIN;
    GPIO->CLR[O_MUX5_ADDR_A0_PORT] = 0X1 << O_MUX5_ADDR_A0_PIN;

    GPIO->DIR[O_MUX5_ADDR_A1_PORT] |= 0X1 << O_MUX5_ADDR_A1_PIN;
    GPIO->CLR[O_MUX5_ADDR_A1_PORT] = 0X1 << O_MUX5_ADDR_A1_PIN;

    GPIO->DIR[A_MUX5_OHM_ADC_PORT] |= 0X1 << A_MUX5_OHM_ADC_PIN;
    GPIO->CLR[A_MUX5_OHM_ADC_PORT] = 0X1 << A_MUX5_OHM_ADC_PIN;

    GPIO->DIR[A_MUX6_OHM_ADC_PORT] |= 0X1 << A_MUX6_OHM_ADC_PIN;
    GPIO->CLR[A_MUX6_OHM_ADC_PORT] = 0X1 << A_MUX6_OHM_ADC_PIN;

    GPIO->DIR[O_MUX6_ADDR_0_PORT] |= 0X1 << O_MUX_ADDR_0_PIN;
    GPIO->CLR[O_MUX6_ADDR_0_PORT] = 0X1 << O_MUX_ADDR_0_PIN;

    GPIO->DIR[O_MUX6_ADDR_1_PORT] |= 0X1 << O_MUX_ADDR_1_PIN;
    GPIO->CLR[O_MUX6_ADDR_1_PORT] = 0X1 << O_MUX_ADDR_1_PIN;

    GPIO->DIR[O_MUX6_ADDR_2_PORT] |= 0X1 << O_MUX_ADDR_2_PIN;
    GPIO->CLR[O_MUX6_ADDR_2_PORT] = 0X1 << O_MUX_ADDR_2_PIN;

    GPIO->DIR[O_EN_MUX6_PORT] |= 0X1 << O_EN_MUX6_PIN;
    GPIO->CLR[O_EN_MUX6_PORT] = 0X1 << O_EN_MUX6_PIN;

    /* GPIO 4 ***********************************/
    /* Настройка входов/выходов GPIO4. */
    IOCON->PIO[D_RTC_CLK_KT_42_PORT][D_RTC_CLK_KT_42_PIN] = IOCON_PIO_FUNC(0x5) | IOCON_PIO_DIGIMODE(0x1) | IOCON_PIO_FILTEROFF(1);

    IOCON->PIO[O_MEASURE_FREQ_RTC_EN_N_PORT][O_MEASURE_FREQ_RTC_EN_N_PIN] = IOCON_PIO_FUNC(0x0) | IOCON_PIO_DIGIMODE(0x1);
    IOCON->PIO[O_MEASURE_FREQ_EN_N_PORT][O_MEASURE_FREQ_EN_N_PIN] = IOCON_PIO_FUNC(0x0) | IOCON_PIO_DIGIMODE(0x1);

    IOCON->PIO[D_CLK_KT_43_PORT][D_CLK_KT_43_PIN] = IOCON_PIO_FUNC(0x5) | IOCON_PIO_DIGIMODE(0x1) | IOCON_PIO_FILTEROFF(1);
    IOCON->PIO[D_CLK_KT_44_PORT][D_CLK_KT_44_PIN] = IOCON_PIO_FUNC(0x5) | IOCON_PIO_DIGIMODE(0x1) | IOCON_PIO_FILTEROFF(1);
    IOCON->PIO[D_CLK_KT_45_PORT][D_CLK_KT_45_PIN] = IOCON_PIO_FUNC(0x5) | IOCON_PIO_DIGIMODE(0x1) | IOCON_PIO_FILTEROFF(1);
    IOCON->PIO[D_CLK_KT_46_PORT][D_CLK_KT_46_PIN] = IOCON_PIO_FUNC(0x5) | IOCON_PIO_DIGIMODE(0x1) | IOCON_PIO_FILTEROFF(1);
    IOCON->PIO[D_CLK_KT_47_PORT][D_CLK_KT_47_PIN] = IOCON_PIO_FUNC(0x5) | IOCON_PIO_DIGIMODE(0x1) | IOCON_PIO_FILTEROFF(1);
    IOCON->PIO[D_CLK_KT_48_PORT][D_CLK_KT_48_PIN] = IOCON_PIO_FUNC(0x5) | IOCON_PIO_DIGIMODE(0x1) | IOCON_PIO_FILTEROFF(1);
    IOCON->PIO[D_CLK_KT_49_PORT][D_CLK_KT_49_PIN] = IOCON_PIO_FUNC(0x5) | IOCON_PIO_DIGIMODE(0x1) | IOCON_PIO_FILTEROFF(1);
    IOCON->PIO[D_CLK_KT_50_PORT][D_CLK_KT_50_PIN] = IOCON_PIO_FUNC(0x5) | IOCON_PIO_DIGIMODE(0x1) | IOCON_PIO_FILTEROFF(1);

    IOCON->PIO[O_CHECK_RST_SPL_N_PORT][O_CHECK_RST_SPL_N_PIN] = IOCON_PIO_FUNC(0x0) | IOCON_PIO_DIGIMODE(0x1) | IOCON_PIO_FILTEROFF(1);
    IOCON->PIO[I_RST_BMC_LVC3_N_PORT][I_RST_BMC_LVC3_N_PIN] = IOCON_PIO_FUNC(0x0) | IOCON_PIO_DIGIMODE(0x1) | IOCON_PIO_FILTEROFF(1);
    IOCON->PIO[I_RST_PCH_RSTBTN_N_PORT][I_RST_PCH_RSTBTN_N_PIN] = IOCON_PIO_FUNC(0x0) | IOCON_PIO_DIGIMODE(0x1) | IOCON_PIO_FILTEROFF(1);
    IOCON->PIO[I_RST_RTCRST_N_PORT][I_RST_RTCRST_N_PIN] = IOCON_PIO_FUNC(0x0) | IOCON_PIO_DIGIMODE(0x1) | IOCON_PIO_FILTEROFF(1);
    IOCON->PIO[I_RST_SRTCRST_N_PORT][I_RST_SRTCRST_N_PIN] = IOCON_PIO_FUNC(0x0) | IOCON_PIO_DIGIMODE(0x1) | IOCON_PIO_FILTEROFF(1);
    IOCON->PIO[I_RST_RSMRST_PCH_N_PORT][I_RST_RSMRST_PCH_N_PIN] = IOCON_PIO_FUNC(0x0) | IOCON_PIO_DIGIMODE(0x1) | IOCON_PIO_FILTEROFF(1);
    IOCON->PIO[I_RST_PLTRST_N_PORT][I_RST_PLTRST_N_PIN] = IOCON_PIO_FUNC(0x0) | IOCON_PIO_DIGIMODE(0x1) | IOCON_PIO_FILTEROFF(1);

    IOCON->PIO[O_PWR_SW_N_PORT][O_PWR_SW_N_PIN] = IOCON_PIO_FUNC(0x0) | IOCON_PIO_DIGIMODE(0x1);
    IOCON->PIO[O_RST_SW_N_PORT][O_RST_SW_N_PIN] = IOCON_PIO_FUNC(0x0) | IOCON_PIO_DIGIMODE(0x1);

    /* Подключение тактирования для GPIO4. */
    SYSCON->AHBCLKCTRLSET[2] = SYSCON_AHBCLKCTRL_GPIO4(1);

    /* Установить GPIO4 в состояние сброса. */
	SYSCON->PRESETCTRLSET[2] = SYSCON_PRESETCTRL_GPIO4_RST(1);
    while (0u == (SYSCON->PRESETCTRL[2] & SYSCON_PRESETCTRL_GPIO4_RST(1))) {
    }

    /* Вывести GPIO4 из состояния сброса. */
    SYSCON->PRESETCTRLCLR[2] = SYSCON_PRESETCTRL_GPIO4_RST(1);
    while (SYSCON_PRESETCTRL_GPIO4_RST(1) == (SYSCON->PRESETCTRL[2] & SYSCON_PRESETCTRL_GPIO4_RST(1))) {
    }

    /* Установить уровни на выходах GPIO4. */
    GPIO->DIR[4] = 0x0; /* Настроить все контакты порта, как входы. */

	GPIO->DIR[O_MEASURE_FREQ_RTC_EN_N_PORT] |= 0X1 << O_MEASURE_FREQ_RTC_EN_N_PIN;
	GPIO->SET[O_MEASURE_FREQ_RTC_EN_N_PORT] = 0X1 << O_MEASURE_FREQ_RTC_EN_N_PIN; /* Установить 1. */

	GPIO->DIR[O_MEASURE_FREQ_EN_N_PORT] |= 0X1 << O_MEASURE_FREQ_EN_N_PIN;
	GPIO->SET[O_MEASURE_FREQ_EN_N_PORT] = 0X1 << O_MEASURE_FREQ_EN_N_PIN;

	GPIO->DIR[O_CHECK_RST_SPL_N_PORT] |= 0X1 << O_CHECK_RST_SPL_N_PIN;
	GPIO->SET[O_CHECK_RST_SPL_N_PORT] = 0X1 << O_CHECK_RST_SPL_N_PIN;

	GPIO->DIR[O_RST_SW_N_PORT] |= 0X1 << O_PWR_SW_N_PIN;
	GPIO->SET[O_RST_SW_N_PORT] = 0X1 << O_PWR_SW_N_PIN;

	GPIO->DIR[O_RST_SW_N_PORT] |= 0X1 << O_RST_SW_N_PIN;
	GPIO->SET[O_RST_SW_N_PORT] = 0X1 << O_RST_SW_N_PIN;

    /* GPIO 5 ***********************************/
    /* Настройка входов/выходов GPIO5. */
	IOCON->PIO[O_LED_TST_OK_PORT][O_LED_TST_OK_PIN] = IOCON_PIO_FUNC(0x0) | IOCON_PIO_DIGIMODE(0x1);
	IOCON->PIO[O_LED_TST_FAIL_PORT][O_LED_TST_FAIL_PIN] = IOCON_PIO_FUNC(0x0) | IOCON_PIO_DIGIMODE(0x1);
	IOCON->PIO[I_SW_TST_N_PORT][I_SW_TST_N_PIN] = IOCON_PIO_FUNC(0x0) | IOCON_PIO_DIGIMODE(0x1) | IOCON_PIO_FILTEROFF(1);

    /* Подключение тактирования для GPIO5. */
	SYSCON->AHBCLKCTRLSET[2] = SYSCON_AHBCLKCTRL_GPIO5(1);

    /* Установить GPIO5 в состояние сброса. */
	SYSCON->PRESETCTRLSET[2] = SYSCON_PRESETCTRL_GPIO5_RST(1);
    while (0u == (SYSCON->PRESETCTRL[2] & SYSCON_PRESETCTRL_GPIO5_RST(1))) {
    }

    /* Вывести GPIO5 из состояния сброса. */
    SYSCON->PRESETCTRLCLR[2] = SYSCON_PRESETCTRL_GPIO5_RST(1);
    while (SYSCON_PRESETCTRL_GPIO5_RST(1) == (SYSCON->PRESETCTRL[2] & SYSCON_PRESETCTRL_GPIO5_RST(1))) {
    }

    /* Установить активные уровни на выходах GPIO5. */
    GPIO->DIR[5] = 0x0; /* Настроить все контакты порта, как входы. */

	GPIO->DIR[O_LED_TST_OK_PORT] |= 0X1 << O_LED_TST_OK_PIN;
	GPIO->CLR[O_LED_TST_OK_PORT] = 0X1 << O_LED_TST_OK_PIN; /* Установить 0. */

	GPIO->DIR[O_LED_TST_FAIL_PORT] |= 0X1 << O_LED_TST_FAIL_PIN;
	GPIO->CLR[O_LED_TST_FAIL_PORT] = 0X1 << O_LED_TST_FAIL_PIN;


	/* ADC 0 ***********************************/
	/* Подключение тактирования для ADC 0. */
    SYSCON->AHBCLKCTRLSET[0] = SYSCON_AHBCLKCTRL_ADC0(1);

    /* SCTimer 0 ***********************************/
    /* Подключение тактирования для SCTimer 0. */
    SYSCON->AHBCLKCTRLSET[1] = SYSCON_AHBCLKCTRL_SCT0(1);

    /* SYSCON power. */
    /* !!!!!!!!!!!!!!!!!!!. */
    POWER_DisablePD(kPDRUNCFG_PD_VDDA);    /* Power on VDDA. */
    POWER_DisablePD(kPDRUNCFG_PD_ADC0);    /* Power on the ADC converter. */
    POWER_DisablePD(kPDRUNCFG_PD_VD2_ANA); /* Power on the analog power supply. */
    POWER_DisablePD(kPDRUNCFG_PD_VREFP);   /* Power on the reference voltage source. */

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

