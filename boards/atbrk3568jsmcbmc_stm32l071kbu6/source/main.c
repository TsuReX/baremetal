#include "time.h"
#include "drivers.h"
#include "console.h"

#include "init.h"
#include "config.h"
#include "debug.h"

void i2c1_init(void)
{
	/* USER CODE BEGIN I2C1_Init 0 */

	/* USER CODE END I2C1_Init 0 */

	LL_I2C_InitTypeDef I2C_InitStruct = {0};

	LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

	LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOA);
	/**I2C1 GPIO Configuration
	PA9   ------> I2C1_SCL
	PA10   ------> I2C1_SDA
	*/
	GPIO_InitStruct.Pin = LL_GPIO_PIN_9;
	GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
	GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_OPENDRAIN;
	GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
	GPIO_InitStruct.Alternate = LL_GPIO_AF_6;
	LL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = LL_GPIO_PIN_10;
	GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
	GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_OPENDRAIN;
	GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
	GPIO_InitStruct.Alternate = LL_GPIO_AF_6;
	LL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	/* Peripheral clock enable */
	LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_I2C1);

	/* USER CODE BEGIN I2C1_Init 1 */

	/* USER CODE END I2C1_Init 1 */

	/** I2C Initialization
	*/
	LL_I2C_EnableAutoEndMode(I2C1);
	LL_I2C_DisableOwnAddress2(I2C1);
	LL_I2C_DisableGeneralCall(I2C1);
	LL_I2C_EnableClockStretching(I2C1);
	I2C_InitStruct.PeripheralMode = LL_I2C_MODE_I2C;
	I2C_InitStruct.Timing = 0x007074B3;
	I2C_InitStruct.AnalogFilter = LL_I2C_ANALOGFILTER_ENABLE;
	I2C_InitStruct.DigitalFilter = 8;
	I2C_InitStruct.OwnAddress1 = 34;
	I2C_InitStruct.TypeAcknowledge = LL_I2C_ACK;
	I2C_InitStruct.OwnAddrSize = LL_I2C_OWNADDRESS1_7BIT;
	LL_I2C_Init(I2C1, &I2C_InitStruct);
	LL_I2C_SetOwnAddress2(I2C1, 0, LL_I2C_OWNADDRESS2_NOMASK);
	/* USER CODE BEGIN I2C1_Init 2 */

	/* USER CODE END I2C1_Init 2 */
}

static void carrier_pwr_on_set()
{
	/* X1_CARRIER_PWR_ON */
	LL_GPIO_SetOutputPin(GPIOB, LL_GPIO_PIN_1);
}

static void carrier_pwr_on_reset()
{
	/* X1_CARRIER_PWR_ON */
	LL_GPIO_ResetOutputPin(GPIOB, LL_GPIO_PIN_1);
}

static void pmic_pwron_set(void)
{
	/* BTN_PWRON => ! PWRON_ */
	LL_GPIO_ResetOutputPin(GPIOB, LL_GPIO_PIN_6);
}

static void pmic_pwron_reset(void)
{
	/* BTN_PWRON => ! PWRON_ */
	LL_GPIO_SetOutputPin(GPIOB, LL_GPIO_PIN_6);
}

void pmic_resetb_set(void)
{
	/* PMIC_RST => ! PMIC_RESETn */
	LL_GPIO_ResetOutputPin(GPIOA, LL_GPIO_PIN_3);
}

void pmic_resetb_reset(void)
{
	/* PMIC_RST => ! PMIC_RESETn */
	LL_GPIO_SetOutputPin(GPIOA, LL_GPIO_PIN_3);
}

static void carrier_stby_n_reset()
{
	/* X1_CARRIER_STBY_ */
	LL_GPIO_ResetOutputPin(GPIOB, LL_GPIO_PIN_4);
}

static void carrier_stby_n_set()
{
	/* X1_CARRIER_STBY_ */
	LL_GPIO_SetOutputPin(GPIOB, LL_GPIO_PIN_4);
}

static void reset_out_n_reset()
{
	/* X1_RESET_OUT_ */
	LL_GPIO_ResetOutputPin(GPIOA, LL_GPIO_PIN_8);
}

static void reset_out_n_set()
{
	/* X1_RESET_OUT_ */
	LL_GPIO_SetOutputPin(GPIOA, LL_GPIO_PIN_8);
}

void emmc_reset_enable()
{
	/* eMMC_Disable => !eMMC_RST_ */
	LL_GPIO_SetOutputPin(GPIOA, LL_GPIO_PIN_0);
}

void emmc_reset_disable()
{
	/* eMMC_Disable => !eMMC_RST_ */
	LL_GPIO_ResetOutputPin(GPIOA, LL_GPIO_PIN_0);
}

void usd_disable(void)
{
	/* SD_Disable => !SDMMC0_CLK */
	LL_GPIO_SetOutputPin(GPIOA, LL_GPIO_PIN_2);
}

void usd_enable(void)
{
	/* SD_Disable => !SDMMC0_CLK */
	LL_GPIO_ResetOutputPin(GPIOA, LL_GPIO_PIN_2);
}

int main(void)
{

    soc_init();

    board_init();

//    i2c1_init();

//    console_init();
//    log_level_set(DEBUG);
//    printk(DEBUG, "ATB RK4568J SMC BMC\r\n");

    // RESET_OUT# = 0
    reset_out_n_reset();
    // CARRIER_STBY# = 0
    carrier_stby_n_reset();
    // CARRIER_PWR_ON = 0
    carrier_pwr_on_reset();

    // Check states of power sources
    // TODO
    mdelay(100);

    // RESET_OUT# = 1
    reset_out_n_set();
    // CARRIER_PWR_ON = 1
    carrier_pwr_on_set();
    // CARRIER_STBY# = 1
    carrier_stby_n_set();

    emmc_reset_enable();
	usd_disable();

    /*0*/
    pmic_resetb_reset();
    mdelay(100);

    /* 1 */
    pmic_pwron_set();
    mdelay(500);
    /* 0 */
    pmic_pwron_reset();
    mdelay(500);
    /* 1 */
    pmic_pwron_set();
    mdelay(100);

    emmc_reset_disable();
	usd_enable();
    /*1*/
    pmic_resetb_set();
    mdelay(10);



    while(1) {
    	mdelay(500);
    }
    return 0;
}
