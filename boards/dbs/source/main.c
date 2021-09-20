#include "time.h"
#include "drivers.h"
#include "console.h"
#include "init.h"
#include "config.h"
#include "debug.h"
#include "usb_device.h"
#include "usbd_cdc_if.h"
#include "i2c_trans.h"

#define F407VET6 1

/*
 * AltIMU-10
 * https://www.pololu.com/product/1269
 */
#define GYRO_ADDR 0x6B /* Gyro (L3GD20) */
#define ACCL_ADDR 0x19 /* Accelerometer (LSM303DLHC) */
#define MGNT_ADDR 0x1E /* Magnetometer (LSM303DLHC) */
#define BMTR_ADDR 0x5D /* Barometer (LPS331AP) */

extern PCD_HandleTypeDef peripheral_controller_driver;

void _Error_Handler(void)
{
	printk(DEBUG, "Error_Handler\r\n");
}

/********************************************************/
void HAL_Delay(uint32_t Delay) {
	mdelay(Delay);
}

HAL_StatusTypeDef HAL_InitTick(uint32_t TickPriority) {
	return 0;
}

void HAL_MspInit(void)
{
	__HAL_RCC_SYSCFG_CLK_ENABLE();
	/* SET_BIT(RCC->APB2ENR, RCC_APB2ENR_SYSCFGEN); */

	__HAL_RCC_PWR_CLK_ENABLE();
	/* SET_BIT(RCC->APB1ENR, RCC_APB1ENR_PWREN) */
}
/********************************************************/

void OTG_FS_IRQHandler(void)
{
	HAL_PCD_IRQHandler(&peripheral_controller_driver);
}

void i2c1_init(void)
{
	LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_I2C1);
	LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOB);
//	PB6   ------> I2C1_SCL
//	PB7   ------> I2C1_SDA

#if 0
	LL_GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.Pin = LL_GPIO_PIN_6|LL_GPIO_PIN_7;
	GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
	GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
	GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;

	LL_GPIO_Init(GPIOB, &GPIO_InitStruct);
#else
	/* Configure SCL Pin as : Alternate function, High Speed, Open drain, Pull up */
	LL_GPIO_SetPinMode(GPIOB, LL_GPIO_PIN_6, LL_GPIO_MODE_ALTERNATE);
	LL_GPIO_SetAFPin_0_7(GPIOB, LL_GPIO_PIN_6, LL_GPIO_AF_4);
	LL_GPIO_SetPinSpeed(GPIOB, LL_GPIO_PIN_6, LL_GPIO_SPEED_FREQ_HIGH);
	LL_GPIO_SetPinOutputType(GPIOB, LL_GPIO_PIN_6, LL_GPIO_OUTPUT_OPENDRAIN);
	LL_GPIO_SetPinPull(GPIOB, LL_GPIO_PIN_6, LL_GPIO_PULL_UP);

	/* Configure SDA Pin as : Alternate function, High Speed, Open drain, Pull up */
	LL_GPIO_SetPinMode(GPIOB, LL_GPIO_PIN_7, LL_GPIO_MODE_ALTERNATE);
	LL_GPIO_SetAFPin_0_7(GPIOB, LL_GPIO_PIN_7, LL_GPIO_AF_4);
	LL_GPIO_SetPinSpeed(GPIOB, LL_GPIO_PIN_7, LL_GPIO_SPEED_FREQ_HIGH);
	LL_GPIO_SetPinOutputType(GPIOB, LL_GPIO_PIN_7, LL_GPIO_OUTPUT_OPENDRAIN);
	LL_GPIO_SetPinPull(GPIOB, LL_GPIO_PIN_7, LL_GPIO_PULL_UP);
#endif

#if 1

	LL_I2C_EnableReset(I2C1);
	mdelay(10);
	LL_I2C_DisableReset(I2C1);

	LL_I2C_InitTypeDef I2C_InitStruct = {0};
	LL_I2C_DisableOwnAddress2(I2C1);
	LL_I2C_DisableGeneralCall(I2C1);
	LL_I2C_EnableClockStretching(I2C1);
	I2C_InitStruct.PeripheralMode = LL_I2C_MODE_I2C;
	I2C_InitStruct.ClockSpeed = 400000;
//	I2C_InitStruct.DutyCycle = LL_I2C_DUTYCYCLE_16_9;
	I2C_InitStruct.DutyCycle = LL_I2C_DUTYCYCLE_2;
	I2C_InitStruct.OwnAddress1 = 0;
	I2C_InitStruct.TypeAcknowledge = LL_I2C_ACK;
	I2C_InitStruct.OwnAddrSize = LL_I2C_OWNADDRESS1_7BIT;
	LL_I2C_Init(I2C1, &I2C_InitStruct);
	LL_I2C_SetOwnAddress2(I2C1, 0);
#else
	LL_I2C_Disable(I2C1);
	LL_RCC_ClocksTypeDef rcc_clocks;
	LL_RCC_GetSystemClocksFreq(&rcc_clocks);
	LL_I2C_ConfigSpeed(I2C1, rcc_clocks.PCLK1_Frequency, 400000, LL_I2C_DUTYCYCLE_2);
#endif

	LL_I2C_Enable(I2C1);
}

void i2c2_init(void)
{

	LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_I2C2);
	LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOB);
//	PB6   ------> I2C2_SCL
//	PB7   ------> I2C2_SDA

#if 0
	LL_GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.Pin = LL_GPIO_PIN_6|LL_GPIO_PIN_7;
	GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
	GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
	GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;

	LL_GPIO_Init(GPIOB, &GPIO_InitStruct);
#else
	/* Configure SCL Pin as : Alternate function, High Speed, Open drain, Pull up */
	LL_GPIO_SetPinMode(GPIOB, LL_GPIO_PIN_6, LL_GPIO_MODE_ALTERNATE);
	LL_GPIO_SetAFPin_0_7(GPIOB, LL_GPIO_PIN_6, LL_GPIO_AF_4);
	LL_GPIO_SetPinSpeed(GPIOB, LL_GPIO_PIN_6, LL_GPIO_SPEED_FREQ_HIGH);
	LL_GPIO_SetPinOutputType(GPIOB, LL_GPIO_PIN_6, LL_GPIO_OUTPUT_OPENDRAIN);
	LL_GPIO_SetPinPull(GPIOB, LL_GPIO_PIN_6, LL_GPIO_PULL_UP);

	/* Configure SDA Pin as : Alternate function, High Speed, Open drain, Pull up */
	LL_GPIO_SetPinMode(GPIOB, LL_GPIO_PIN_7, LL_GPIO_MODE_ALTERNATE);
	LL_GPIO_SetAFPin_0_7(GPIOB, LL_GPIO_PIN_7, LL_GPIO_AF_4);
	LL_GPIO_SetPinSpeed(GPIOB, LL_GPIO_PIN_7, LL_GPIO_SPEED_FREQ_HIGH);
	LL_GPIO_SetPinOutputType(GPIOB, LL_GPIO_PIN_7, LL_GPIO_OUTPUT_OPENDRAIN);
	LL_GPIO_SetPinPull(GPIOB, LL_GPIO_PIN_7, LL_GPIO_PULL_UP);
#endif

#if 1

	LL_I2C_EnableReset(I2C2);
	mdelay(10);
	LL_I2C_DisableReset(I2C2);

	LL_I2C_InitTypeDef I2C_InitStruct = {0};
	LL_I2C_DisableOwnAddress2(I2C2);
	LL_I2C_DisableGeneralCall(I2C2);
	LL_I2C_EnableClockStretching(I2C2);
	I2C_InitStruct.PeripheralMode = LL_I2C_MODE_I2C;
	I2C_InitStruct.ClockSpeed = 400000;
//	I2C_InitStruct.DutyCycle = LL_I2C_DUTYCYCLE_16_9;
	I2C_InitStruct.DutyCycle = LL_I2C_DUTYCYCLE_2;
	I2C_InitStruct.OwnAddress1 = 0;
	I2C_InitStruct.TypeAcknowledge = LL_I2C_ACK;
	I2C_InitStruct.OwnAddrSize = LL_I2C_OWNADDRESS1_7BIT;
	LL_I2C_Init(I2C2, &I2C_InitStruct);
	LL_I2C_SetOwnAddress2(I2C2, 0);
#else
	LL_I2C_Disable(I2C2);
	LL_RCC_ClocksTypeDef rcc_clocks;
	LL_RCC_GetSystemClocksFreq(&rcc_clocks);
	LL_I2C_ConfigSpeed(I2C2, rcc_clocks.PCLK1_Frequency, 400000, LL_I2C_DUTYCYCLE_2);
#endif

	LL_I2C_Enable(I2C2);
}

#if defined(F407VET6)
int main(void)
{
	soc_init();

	board_init();

	mdelay(50);

	console_init();

	log_level_set(DEBUG);

	HAL_Init();

	usb_init();

	i2c1_init();

	i2c2_init();

	printk(DEBUG, "depo_baikalbmc\r\n");

	uint8_t chip_addr = 0x00;
	for (;chip_addr < 0x7F; ++chip_addr) {
		uint8_t reg = 0;
		int32_t ret_val = i2c_read(I2C1, chip_addr << 1, 0x01, &reg, sizeof(reg));
		if (ret_val < 0) {
//			printk(DEBUG, "0x%02X error: %ld\r\n",chip_addr, ret_val);
			;
		}
		else {
			printk(DEBUG, "0x%02X value: 0x%X\r\n", chip_addr, reg);
		}
	}

	uint32_t i = 0;
	uint8_t str[] = "CDC Transmit\r\n";
	while(1) {
		printk(DEBUG, "cycle 0x%lX\r\n", i++);
		CDC_Transmit_FS(str, strlen((char*)str));
		LL_GPIO_SetOutputPin(GPIOA, LL_GPIO_PIN_6);
		LL_GPIO_ResetOutputPin(GPIOA, LL_GPIO_PIN_7);
		mdelay(500);
		LL_GPIO_ResetOutputPin(GPIOA, LL_GPIO_PIN_6);
		LL_GPIO_SetOutputPin(GPIOA, LL_GPIO_PIN_7);
		mdelay(500);
	}

	return 0;
}
#elif defined(F407ZGT6)
int main(void)
{
	soc_init();

	board_init();

	HAL_Init();
	usb_init();

	mdelay(5000);

	LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOD);
	LL_GPIO_SetPinMode(GPIOD, LL_GPIO_PIN_13, LL_GPIO_MODE_OUTPUT);
	LL_GPIO_SetOutputPin(GPIOD, LL_GPIO_PIN_13);

	while(1) {
		mdelay(500);
	}

	return 0;
}
#endif
