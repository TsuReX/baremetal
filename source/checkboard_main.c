#include <stdint.h>
#include <stddef.h>

#include <fsl_common.h>
#include <fsl_iocon.h>
#include <fsl_power.h>
#include <fsl_gpio.h>

void BOARD_InitPins(void)
{

    CLOCK_EnableClock(kCLOCK_Iocon);

    const uint32_t port3_pin0_config = (IOCON_FUNC0 | IOCON_MODE_INACT | IOCON_DIGITAL_EN | IOCON_INPFILT_OFF);
    IOCON_PinMuxSet(IOCON, 3U, 0U, port3_pin0_config);

    const uint32_t port3_pin2_config = (IOCON_FUNC0 | IOCON_MODE_INACT | IOCON_DIGITAL_EN | IOCON_INPFILT_OFF);
    IOCON_PinMuxSet(IOCON, 3U, 2U, port3_pin2_config);

    const uint32_t port5_pin0_config = (IOCON_FUNC0 | IOCON_MODE_INACT | IOCON_DIGITAL_EN | IOCON_INPFILT_OFF);
    IOCON_PinMuxSet(IOCON, 5U,0U, port5_pin0_config);

    const uint32_t port5_pin1_config = (IOCON_FUNC0 | IOCON_MODE_INACT | IOCON_DIGITAL_EN | IOCON_INPFILT_OFF);
    IOCON_PinMuxSet(IOCON, 5U, 1U, port5_pin1_config);
}

/*******************************************************************************
 * Definitions for BOARD_BootClockPLL180M configuration
 ******************************************************************************/
#define BOARD_BOOTCLOCKPLL180M_CORE_CLOCK 180000000U /*!< Core clock frequency:180000000Hz */

uint32_t SystemCoreClock = DEFAULT_SYSTEM_CLOCK;

/*******************************************************************************
 ******************** Configuration BOARD_BootClockPLL180M *********************
 ******************************************************************************/
/* clang-format off */
/* TEXT BELOW IS USED AS SETTING FOR TOOLS *************************************
!!Configuration
name: BOARD_BootClockPLL180M
called_from_default_init: true
outputs:
- {id: FRO12M_clock.outFreq, value: 12 MHz}
- {id: FROHF_clock.outFreq, value: 96 MHz}
- {id: MAIN_clock.outFreq, value: 180 MHz}
- {id: SYSPLL_clock.outFreq, value: 180 MHz}
- {id: System_clock.outFreq, value: 180 MHz}
- {id: USB0_clock.outFreq, value: 96 MHz}
settings:
- {id: SYSCON.MAINCLKSELB.sel, value: SYSCON.PLL_BYPASS}
- {id: SYSCON.M_MULT.scale, value: '30', locked: true}
- {id: SYSCON.N_DIV.scale, value: '1', locked: true}
- {id: SYSCON.PDEC.scale, value: '2', locked: true}
- {id: SYSCON.USB0CLKSEL.sel, value: SYSCON.fro_hf}
- {id: SYSCON_PDRUNCFG0_PDEN_SYS_PLL_CFG, value: Power_up}
sources:
- {id: SYSCON._clk_in.outFreq, value: 12 MHz, enabled: true}
- {id: SYSCON.fro_hf.outFreq, value: 96 MHz}
 * BE CAREFUL MODIFYING THIS COMMENT - IT IS YAML SETTINGS FOR TOOLS **********/
/* clang-format on */

/*******************************************************************************
 * Variables for BOARD_BootClockPLL180M configuration
 ******************************************************************************/
/*******************************************************************************
 * Code for BOARD_BootClockPLL180M configuration
 ******************************************************************************/
void BOARD_BootClockPLL180M(void)
{
    /*!< Set up the clock sources */
    /*!< Set up FRO */
    POWER_DisablePD(kPDRUNCFG_PD_FRO_EN);  /*!< Ensure FRO is on  */
    CLOCK_AttachClk(kFRO12M_to_MAIN_CLK);  /*!< Switch to FRO 12MHz first to ensure we can change voltage without
                                              accidentally  being below the voltage for current speed */
    POWER_DisablePD(kPDRUNCFG_PD_SYS_OSC); /*!< Enable System Oscillator Power */
    SYSCON->SYSOSCCTRL = ((SYSCON->SYSOSCCTRL & ~SYSCON_SYSOSCCTRL_FREQRANGE_MASK) |
                          SYSCON_SYSOSCCTRL_FREQRANGE(0U)); /*!< Set system oscillator range */
    POWER_SetVoltageForFreq(
        180000000U); /*!< Set voltage for the one of the fastest clock outputs: System clock output */
    CLOCK_SetFLASHAccessCyclesForFreq(180000000U); /*!< Set FLASH wait states for core */

    /*!< Set up SYS PLL */
    const pll_setup_t pllSetup = {
        .pllctrl = SYSCON_SYSPLLCTRL_SELI(32U) | SYSCON_SYSPLLCTRL_SELP(16U) | SYSCON_SYSPLLCTRL_SELR(0U),
        .pllmdec = (SYSCON_SYSPLLMDEC_MDEC(8191U)),
        .pllndec = (SYSCON_SYSPLLNDEC_NDEC(770U)),
        .pllpdec = (SYSCON_SYSPLLPDEC_PDEC(98U)),
        .pllRate = 180000000U,
        .flags   = PLL_SETUPFLAG_WAITLOCK | PLL_SETUPFLAG_POWERUP};
    CLOCK_AttachClk(kFRO12M_to_SYS_PLL); /*!< Set sys pll clock source*/
    CLOCK_SetPLLFreq(&pllSetup);         /*!< Configure PLL to the desired value */
    /*!< Need to make sure ROM and OTP has power(PDRUNCFG0[17,29]= 0U)
         before calling this API since this API is implemented in ROM code */
    CLOCK_SetupFROClocking(96000000U); /*!< Set up high frequency FRO output to selected frequency */

    /*!< Set up dividers */
    CLOCK_SetClkDiv(kCLOCK_DivAhbClk, 1U, false);  /*!< Reset divider counter and set divider to value 1 */
    CLOCK_SetClkDiv(kCLOCK_DivUsb0Clk, 0U, true);  /*!< Reset USB0CLKDIV divider counter and halt it */
    CLOCK_SetClkDiv(kCLOCK_DivUsb0Clk, 1U, false); /*!< Set USB0CLKDIV divider to value 1 */

    /*!< Set up clock selectors - Attach clocks to the peripheries */
    CLOCK_AttachClk(kSYS_PLL_to_MAIN_CLK); /*!< Switch MAIN_CLK to SYS_PLL */
    CLOCK_AttachClk(kFRO_HF_to_USB0_CLK);  /*!< Switch USB0_CLK to FRO_HF */
    SYSCON->MAINCLKSELA =
        ((SYSCON->MAINCLKSELA & ~SYSCON_MAINCLKSELA_SEL_MASK) |
         SYSCON_MAINCLKSELA_SEL(0U)); /*!< Switch MAINCLKSELA to FRO12M even it is not used for MAINCLKSELB */
    /* Set SystemCoreClock variable. */
    SystemCoreClock = BOARD_BOOTCLOCKPLL180M_CORE_CLOCK;
}

/*******************************************************************************
 * Variables
 ******************************************************************************/
volatile uint32_t g_systickCounter;

void SysTick_Handler(void)
{
    if (g_systickCounter != 0U) {
        g_systickCounter--;
    }
}

void SysTick_DelayTicks(uint32_t n)
{
    g_systickCounter = n;
    while (g_systickCounter != 0U) {
    }
}

static inline void power_on_hl1(void)
{
    GPIO_PinWrite(GPIO, 5, 0, 1);
}

static inline void power_off_hl1(void)
{
    GPIO_PinWrite(GPIO, 5, 0, 0);
}

static inline void power_on_hl2(void)
{
    GPIO_PinWrite(GPIO, 5, 2, 1);
}

static inline void power_off_hl2(void)
{
    GPIO_PinWrite(GPIO, 5, 2, 0);
}

static inline void power_on_bp(void)
{
    GPIO_PinWrite(GPIO, 3, 0, 0);
}

static inline void power_off_bp(void)
{
    GPIO_PinWrite(GPIO, 3, 0, 1);
}

static inline void power_on_spl(void)
{
    GPIO_PinWrite(GPIO, 3, 2, 0);
}

static inline void power_off_spl(void)
{
    GPIO_PinWrite(GPIO, 3, 2, 1);
}

int main(void)
{
    /* attach 12 MHz clock to FLEXCOMM0 (debug console) */
    CLOCK_AttachClk(kFRO12M_to_FLEXCOMM0);

    CLOCK_EnableClock(kCLOCK_Gpio3);
    CLOCK_EnableClock(kCLOCK_Gpio5);

    BOARD_InitPins();
    BOARD_BootClockPLL180M();

    /* Print a note to terminal. */

    /* Init output LED GPIO. */
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

	if (SysTick_Config(SystemCoreClock / 1000U)) {
		while (1) {
			;
		}
	}

	power_on_hl1();
    SysTick_DelayTicks(2000U);

    power_on_hl2();
    SysTick_DelayTicks(2000U);

    power_on_bp();
    SysTick_DelayTicks(10000U);

    power_off_bp();
    /* Port masking */
//    GPIO_PortMaskedSet(GPIO, APP_BOARD_TEST_LED_PORT, 0x0000FFFF);
//    GPIO_PortMaskedWrite(GPIO, APP_BOARD_TEST_LED_PORT, 0xFFFFFFFF);
//    port_state = GPIO_PortRead(GPIO, APP_BOARD_TEST_LED_PORT);
//    PRINTF("\r\n Standard port read: %x\r\n", port_state);
//    port_state = GPIO_PortMaskedRead(GPIO, APP_BOARD_TEST_LED_PORT);
//    PRINTF("\r\n Masked port read: %x\r\n", port_state);

    /* Set systick reload value to generate 1ms interrupt */


    while (1) {
//        port_state = GPIO_PortRead(GPIO, APP_SW_PORT);
//        if (!(port_state & (1 << APP_SW_PIN))) {
            GPIO_PortToggle(GPIO, 5, 3);
//        }
        SysTick_DelayTicks(1000U);
    }
}

