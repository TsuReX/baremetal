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
void gyro_init()
{
	uint8_t chip_addr = GYRO_ADDR;
	uint8_t reg = 0xFF;

	printk(DEBUG, "Gyroscope init\r\n");

	// DR = 01 (200 Hz ODR); BW = 10 (50 Hz bandwidth); PD = 1 (normal mode); Zen = Yen = Xen = 1 (all axes enabled)

	i2c_read(I2C1, chip_addr << 1, 0x20, &reg, 1);
	printk(DEBUG, "0x%04X CTRL_REG1: 0x%X\r\n", chip_addr, reg);
	reg = 0x6F;
	i2c_write(I2C1, chip_addr << 1, 0x20, &reg, 1);
	i2c_read(I2C1, chip_addr << 1, 0x20, &reg, 1);
	printk(DEBUG, "0x%04X CTRL_REG1: 0x%X\r\n", chip_addr, reg);

	reg = 0x00;
	i2c_write(I2C1, chip_addr << 1, 0x23, &reg, 1);
	reg = 0x00;
	i2c_write(I2C1, chip_addr << 1, 0x39, &reg, 1);
}

void gyro_read()
{
	uint8_t chip_addr = GYRO_ADDR;
	uint8_t reg = 0xFF;

	printk(DEBUG, "Gyroscope\r\n");

	i2c_read(I2C1, chip_addr << 1, 0x0F, &reg, 1);
	printk(DEBUG, "0x%04X WHO_AM_I: 0x%X\r\n", chip_addr, reg);

	i2c_read(I2C1, chip_addr << 1, 0x27, &reg, 1);
	printk(DEBUG, "0x%04X STATUS_REG value: 0x%X\r\n", chip_addr, reg);

	i2c_read(I2C1, chip_addr << 1, 0x28, &reg, 1);
	printk(DEBUG, "0x%04X 1 value: 0x%X\r\n", chip_addr, reg);

	i2c_read(I2C1, chip_addr << 1, 0x29, &reg, 1);
	printk(DEBUG, "0x%04X 2 value: 0x%X\r\n", chip_addr, reg);

	i2c_read(I2C1, chip_addr << 1, 0x2A, &reg, 1);
	printk(DEBUG, "0x%04X 3 value: 0x%X\r\n", chip_addr, reg);

	i2c_read(I2C1, chip_addr << 1, 0x2B, &reg, 1);
	printk(DEBUG, "0x%04X 4 value: 0x%X\r\n", chip_addr, reg);

	i2c_read(I2C1, chip_addr << 1, 0x2C, &reg, 1);
	printk(DEBUG, "0x%04X 5 value: 0x%X\r\n", chip_addr, reg);

	i2c_read(I2C1, chip_addr << 1, 0x2D, &reg, 1);
	printk(DEBUG, "0x%04X 6 value: 0x%X\r\n", chip_addr, reg);
}

void accel_init()
{
	uint8_t chip_addr = ACCL_ADDR;
	uint8_t reg = 0xFF;
	int32_t ret_val = 0;

	printk(DEBUG, "Accelerometer init\r\n");

// Accelerometer
// 0x00 = 0b00000000
// AFS = 0 (+/- 2 g full scale)
	reg = 0x00;
	ret_val = i2c_write(I2C1, chip_addr << 1, 0x21, &reg, 1);
	if (ret_val < 0)
		printk(DEBUG, "0x%02X error: %ld\r\n", chip_addr, ret_val);
// 0x57 = 0b01010111
// AODR = 0101 (50 Hz ODR); AZEN = AYEN = AXEN = 1 (all axes enabled)
	reg = 0x56;
	ret_val = i2c_write(I2C1, chip_addr << 1, 0x20, &reg, 1);
	if (ret_val < 0)
		printk(DEBUG, "0x%02X error: %ld\r\n", chip_addr, ret_val);
}

void magnet_init()
{
	uint8_t chip_addr = MGNT_ADDR;
	uint8_t reg = 0xFF;

	printk(DEBUG, "Magnetometer init\r\n");

// Magnetometer
// 0x64 = 0b01100100
// M_RES = 11 (high resolution mode); M_ODR = 001 (6.25 Hz ODR)

// 0x20 = 0b00100000
// MFS = 01 (+/- 4 gauss full scale)

// 0x00 = 0b00000000
// MLP = 0 (low power mode off); MD = 00 (continuous-conversion mode)

	reg = 0x64;
	i2c_write(I2C1, chip_addr << 1, 0x24, &reg, 1);
	reg = 0x20;
	i2c_write(I2C1, chip_addr << 1, 0x25, &reg, 1);
	reg = 0x00;
	i2c_write(I2C1, chip_addr << 1, 0x26, &reg, 1);

}

void accel_read()
{
	uint8_t chip_addr = ACCL_ADDR;
	uint8_t reg = 0xFF;

	printk(DEBUG, "Accelerometer\r\n");

	reg = 0xFF;
	i2c_read(I2C1, chip_addr << 1, 0x20, &reg, 1);
	printk(DEBUG, "0x%04X CTRL_REG_A: 0x%X\r\n", chip_addr, reg);

	i2c_read(I2C1, chip_addr << 1, 0x27, &reg, 1);
	printk(DEBUG, "0x%04X STATUS_REG_A: 0x%X\r\n", chip_addr, reg);

	i2c_read(I2C1, chip_addr << 1, 0x28, &reg, 1);
	printk(DEBUG, "0x%04X OUT_X_L_A: 0x%X\r\n", chip_addr, reg);

	i2c_read(I2C1, chip_addr << 1, 0x29, &reg, 1);
	printk(DEBUG, "0x%04X OUT_X_H_A: 0x%X\r\n", chip_addr, reg);

	i2c_read(I2C1, chip_addr << 1, 0x2A, &reg, 1);
	printk(DEBUG, "0x%04X OUT_Y_L_A: 0x%X\r\n", chip_addr, reg);

	i2c_read(I2C1, chip_addr << 1, 0x2B, &reg, 1);
	printk(DEBUG, "0x%04X OUT_Y_H_A: 0x%X\r\n", chip_addr, reg);

	i2c_read(I2C1, chip_addr << 1, 0x2C, &reg, 1);
	printk(DEBUG, "0x%04X OUT_Z_L_A: 0x%X\r\n", chip_addr, reg);

	i2c_read(I2C1, chip_addr << 1, 0x2D, &reg, 1);
	printk(DEBUG, "0x%04X OUT_Z_H_A: 0x%X\r\n", chip_addr, reg);

}

void magnet_read()
{
	uint8_t chip_addr = MGNT_ADDR;
	uint8_t reg = 0xFF;

	printk(DEBUG, "Magnetometer init\r\n");

	i2c_read(I2C1, chip_addr << 1, 0x3, &reg, 1);
	printk(DEBUG, "0x%04X OUT_X_L_M: 0x%X\r\n", chip_addr, reg);

	i2c_read(I2C1, chip_addr << 1, 0x4, &reg, 1);
	printk(DEBUG, "0x%04X OUT_X_H_M: 0x%X\r\n", chip_addr, reg);

	i2c_read(I2C1, chip_addr << 1, 0x5, &reg, 1);
	printk(DEBUG, "0x%04X OUT_Y_L_M: 0x%X\r\n", chip_addr, reg);

	i2c_read(I2C1, chip_addr << 1, 0x6, &reg, 1);
	printk(DEBUG, "0x%04X OUT_Y_H_M: 0x%X\r\n", chip_addr, reg);

	i2c_read(I2C1, chip_addr << 1, 0x7, &reg, 1);
	printk(DEBUG, "0x%04X OUT_Z_L_M: 0x%X\r\n", chip_addr, reg);

	i2c_read(I2C1, chip_addr << 1, 0x8, &reg, 1);
	printk(DEBUG, "0x%04X OUT_Z_H_M: 0x%X\r\n", chip_addr, reg);
}

#if defined(F407VET6)
int main(void)
{
	soc_init();

	HAL_Init();

	usb_init();

	board_init();

	mdelay(50);

	console_init();

	log_level_set(DEBUG);


	i2c1_init();

	i2c2_init();

	printk(DEBUG, "DBS\r\n");

//	uint8_t chip_addr = 0x00;
//	for (;chip_addr < 0x7F; ++chip_addr) {
//		uint8_t reg = 0;
//		int32_t ret_val = i2c_read(I2C1, chip_addr << 1, 0x01, &reg, sizeof(reg));
//		if (ret_val < 0) {
////			printk(DEBUG, "0x%02X error: %ld\r\n",chip_addr, ret_val);
//			;
//		}
//		else {
//			printk(DEBUG, "0x%02X value: 0x%X\r\n", chip_addr, reg);
//		}
//	}

	accel_init();
	gyro_init();
	magnet_init();

	uint32_t i = 0;
//	uint8_t str[] = "CDC Transmit\r\n";
	while(1) {
		printk(DEBUG, "cycle 0x%lX\r\n", i++);
//		CDC_Transmit_FS(str, strlen((char*)str));
		LL_GPIO_SetOutputPin(GPIOA, LL_GPIO_PIN_6);
		LL_GPIO_ResetOutputPin(GPIOA, LL_GPIO_PIN_7);
		mdelay(500);
		LL_GPIO_ResetOutputPin(GPIOA, LL_GPIO_PIN_6);
		LL_GPIO_SetOutputPin(GPIOA, LL_GPIO_PIN_7);
		mdelay(500);

		accel_read();
		gyro_read();
		magnet_read();
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
