#include "time.h"
#include "drivers.h"
#include "console.h"
#include "init.h"
#include "config.h"
#include "debug.h"
#include "usb_device.h"
#include "usbd_cdc_if.h"
#include "i2c_trans.h"

//#define F407VET6 1
//#define F407ZGT6 1

extern PCD_HandleTypeDef peripheral_controller_driver;

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

void i2c2_init(void)
{
	LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_I2C2);
	LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOF);

//	PF1   ------> I2C2_SCL
	LL_GPIO_SetPinMode(GPIOF, LL_GPIO_PIN_1, LL_GPIO_MODE_ALTERNATE);
	LL_GPIO_SetAFPin_0_7(GPIOF, LL_GPIO_PIN_1, LL_GPIO_AF_4);
	LL_GPIO_SetPinSpeed(GPIOF, LL_GPIO_PIN_1, LL_GPIO_SPEED_FREQ_HIGH);
	LL_GPIO_SetPinOutputType(GPIOF, LL_GPIO_PIN_1, LL_GPIO_OUTPUT_OPENDRAIN);
	LL_GPIO_SetPinPull(GPIOF, LL_GPIO_PIN_1, LL_GPIO_PULL_UP);

//	PF0   ------> I2C2_SDA
	LL_GPIO_SetPinMode(GPIOF, LL_GPIO_PIN_0, LL_GPIO_MODE_ALTERNATE);
	LL_GPIO_SetAFPin_0_7(GPIOF, LL_GPIO_PIN_0, LL_GPIO_AF_4);
	LL_GPIO_SetPinSpeed(GPIOF, LL_GPIO_PIN_0, LL_GPIO_SPEED_FREQ_HIGH);
	LL_GPIO_SetPinOutputType(GPIOF, LL_GPIO_PIN_0, LL_GPIO_OUTPUT_OPENDRAIN);
	LL_GPIO_SetPinPull(GPIOF, LL_GPIO_PIN_0, LL_GPIO_PULL_UP);


	LL_I2C_EnableReset(I2C2);
	mdelay(10);
	LL_I2C_DisableReset(I2C2);

	LL_I2C_InitTypeDef I2C_InitStruct = {0};
	LL_I2C_DisableOwnAddress2(I2C2);
	LL_I2C_DisableGeneralCall(I2C2);
	LL_I2C_EnableClockStretching(I2C2);
	I2C_InitStruct.PeripheralMode = LL_I2C_MODE_I2C;
	I2C_InitStruct.ClockSpeed = 100000;
	I2C_InitStruct.DutyCycle = LL_I2C_DUTYCYCLE_2;
	I2C_InitStruct.OwnAddress1 = 0;
	I2C_InitStruct.TypeAcknowledge = LL_I2C_ACK;
	I2C_InitStruct.OwnAddrSize = LL_I2C_OWNADDRESS1_7BIT;
	LL_I2C_Init(I2C2, &I2C_InitStruct);
	LL_I2C_SetOwnAddress2(I2C2, 0);

	LL_I2C_Enable(I2C2);
}

void i2c1_init(void)
{
	LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_I2C1);
	LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOB);

//	PB6   ------> I2C1_SCL
	LL_GPIO_SetPinMode(GPIOB, LL_GPIO_PIN_6, LL_GPIO_MODE_ALTERNATE);
	LL_GPIO_SetAFPin_0_7(GPIOB, LL_GPIO_PIN_6, LL_GPIO_AF_4);
	LL_GPIO_SetPinSpeed(GPIOB, LL_GPIO_PIN_6, LL_GPIO_SPEED_FREQ_HIGH);
	LL_GPIO_SetPinOutputType(GPIOB, LL_GPIO_PIN_6, LL_GPIO_OUTPUT_OPENDRAIN);
	LL_GPIO_SetPinPull(GPIOB, LL_GPIO_PIN_6, LL_GPIO_PULL_UP);

//	PB7   ------> I2C1_SDA
	LL_GPIO_SetPinMode(GPIOB, LL_GPIO_PIN_7, LL_GPIO_MODE_ALTERNATE);
	LL_GPIO_SetAFPin_0_7(GPIOB, LL_GPIO_PIN_7, LL_GPIO_AF_4);
	LL_GPIO_SetPinSpeed(GPIOB, LL_GPIO_PIN_7, LL_GPIO_SPEED_FREQ_HIGH);
	LL_GPIO_SetPinOutputType(GPIOB, LL_GPIO_PIN_7, LL_GPIO_OUTPUT_OPENDRAIN);
	LL_GPIO_SetPinPull(GPIOB, LL_GPIO_PIN_7, LL_GPIO_PULL_UP);

	LL_I2C_EnableReset(I2C1);
	mdelay(10);
	LL_I2C_DisableReset(I2C1);

	LL_I2C_InitTypeDef I2C_InitStruct = {0};
	LL_I2C_DisableOwnAddress2(I2C1);
	LL_I2C_DisableGeneralCall(I2C1);
	LL_I2C_EnableClockStretching(I2C1);
	I2C_InitStruct.PeripheralMode = LL_I2C_MODE_I2C;
	I2C_InitStruct.ClockSpeed = 100000;
	I2C_InitStruct.DutyCycle = LL_I2C_DUTYCYCLE_2;
	I2C_InitStruct.OwnAddress1 = 0;
	I2C_InitStruct.TypeAcknowledge = LL_I2C_ACK;
	I2C_InitStruct.OwnAddrSize = LL_I2C_OWNADDRESS1_7BIT;
	LL_I2C_Init(I2C1, &I2C_InitStruct);
	LL_I2C_SetOwnAddress2(I2C1, 0);
	LL_I2C_Enable(I2C1);
}

void d85_enable()
{
	uint8_t chip_addr = D85_ADDR;
	uint8_t reg = 0;
	i2c_write(I2C1, chip_addr << 1, 0x03, &reg, sizeof(reg));
	reg = 0xFF;
	i2c_write(I2C1, chip_addr << 1, 0x01, &reg, sizeof(reg));
	reg = 0x00;
	i2c_read(I2C1, chip_addr << 1, 0x01, &reg, sizeof(reg));
	printk(DEBUG, "D85 value: 0x%X\r\n", reg);
}

void d86_enable()
{
	uint8_t chip_addr = D86_ADDR;
	uint8_t reg = 0;
	i2c_write(I2C1, chip_addr << 1, 0x06, &reg, sizeof(reg));
	i2c_write(I2C1, chip_addr << 1, 0x07, &reg, sizeof(reg));
	mdelay(1000);
	reg = 0x00;
	i2c_write(I2C1, chip_addr << 1, 0x02, &reg, sizeof(reg));
	i2c_write(I2C1, chip_addr << 1, 0x03, &reg, sizeof(reg));
	mdelay(1000);
	reg = 0xFF;
	i2c_write(I2C1, chip_addr << 1, 0x02, &reg, sizeof(reg));
	i2c_write(I2C1, chip_addr << 1, 0x03, &reg, sizeof(reg));

	i2c_read(I2C1, chip_addr << 1, 0x02, &reg, sizeof(reg));
	printk(DEBUG, "D86 value: 0x%X\r\n", reg);
	i2c_read(I2C1, chip_addr << 1, 0x03, &reg, sizeof(reg));
	printk(DEBUG, "D86 value: 0x%X\r\n", reg);
}

void d87_enable()
{
	uint8_t chip_addr = D87_ADDR;
	uint8_t reg = 0;
	i2c_write(I2C1, chip_addr << 1, 0x06, &reg, sizeof(reg));
	i2c_write(I2C1, chip_addr << 1, 0x07, &reg, sizeof(reg));
	mdelay(1000);
	reg = 0x00;
	i2c_write(I2C1, chip_addr << 1, 0x02, &reg, sizeof(reg));
	i2c_write(I2C1, chip_addr << 1, 0x03, &reg, sizeof(reg));
	mdelay(1000);
	reg = 0xFF;
	i2c_write(I2C1, chip_addr << 1, 0x02, &reg, sizeof(reg));
	i2c_write(I2C1, chip_addr << 1, 0x03, &reg, sizeof(reg));

	i2c_read(I2C1, chip_addr << 1, 0x02, &reg, sizeof(reg));
	printk(DEBUG, "D87 value: 0x%X\r\n", reg);
	i2c_read(I2C1, chip_addr << 1, 0x03, &reg, sizeof(reg));
	printk(DEBUG, "D87 value: 0x%X\r\n", reg);
}

void d88_enable()
{
	uint8_t chip_addr = D88_ADDR;
	uint8_t reg = 0;
	i2c_write(I2C2, chip_addr << 1, 0x03, &reg, sizeof(reg));
	reg = 0xFF;
	i2c_write(I2C2, chip_addr << 1, 0x01, &reg, sizeof(reg));
	reg = 0x00;
	i2c_read(I2C2, chip_addr << 1, 0x01, &reg, sizeof(reg));
	printk(DEBUG, "D88 value: 0x%X\r\n", reg);
}

#if defined(F407VET6)
int main(void)
{
	soc_init();

	board_init();

	HAL_Init();

	usb_init();

	mdelay(2500);

	console_init();

	log_level_set(DEBUG);

	i2c1_init();

	printk(DEBUG, "EVB\r\n");

	uint8_t chip_addr = 0x00;
	for (;chip_addr < 0x7F; ++chip_addr) {
		uint8_t reg = 0;
		if (i2c_read(I2C1, chip_addr << 1, 0x01, &reg, sizeof(reg)) == 0)
		printk(DEBUG, "0x%02X value: 0x%X\r\n", chip_addr, reg);
	}

	uint32_t i = 0;
	while(1) {
//		uint8_t str[] = "CDC Transmit\r\n";
//		CDC_Transmit_FS(str, strlen((char*)str));

		printk(DEBUG, "\rcycle 0x%lX\t\t", i++);

		LL_GPIO_TogglePin(GPIOA, LL_GPIO_PIN_6);
		LL_GPIO_TogglePin(GPIOA, LL_GPIO_PIN_7);

		mdelay(500);
	}

	return 0;
}
#elif defined(F407ZGT6)
int main(void)
{
	soc_init();

	board_init();

//	GPIO_InitTypeDef GPIO_InitStruct = {0};
//	LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOA);
//	/**USB_OTG_FS GPIO Configuration
//	PA11     ------> USB_OTG_FS_DM
//	PA12     ------> USB_OTG_FS_DP
//	*/
//	GPIO_InitStruct.Pin = GPIO_PIN_11|GPIO_PIN_12;
//	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
//	GPIO_InitStruct.Pull = GPIO_NOPULL;
//	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
//	GPIO_InitStruct.Alternate = GPIO_AF10_OTG_FS;
//	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
//
//	LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_OTGFS);
//	LL_APB1_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_SYSCFG);
//
//	USB_DevConnect(USB_OTG_FS);

	HAL_Init();

	usb_init();

	mdelay(1000);
//	while(1);
	console_init();

	log_level_set(DEBUG);

	printk(DEBUG, "DBS\r\n");

//	mdelay(6000);

	LL_GPIO_ResetOutputPin(PS_ON_BMC_PORT, PS_ON_BMC_PIN);

	mdelay(100);

	i2c1_init();

	i2c2_init();

	uint8_t chip_addr = 0x00;
	for (;chip_addr < 0x7F; ++chip_addr) {
		uint8_t reg = 0;
		i2c_read(I2C1, chip_addr << 1, 0x01, &reg, sizeof(reg));
		printk(DEBUG, "I2C1 0x%02X value: 0x%X\r\n", chip_addr, reg);
	}

	chip_addr = 0x00;
	for (;chip_addr < 0x7F; ++chip_addr) {
		uint8_t reg = 0;
		i2c_read(I2C2, chip_addr << 1, 0x01, &reg, sizeof(reg));
		printk(DEBUG, "I2C2 0x%02X value: 0x%X\r\n", chip_addr, reg);
	}

//	while(1);

	d85_enable();
	d86_enable();
	d87_enable();
	d88_enable();

	LL_GPIO_SetOutputPin(VDD_PCIE_VPH_CPU0_EN_PORT, VDD_PCIE_VPH_CPU0_EN_PIN);

	LL_GPIO_SetOutputPin(VDD_PCIE_VP_CPU0_EN_PORT, VDD_PCIE_VP_CPU0_EN_PIN);

	LL_GPIO_SetOutputPin(VDD_PCIE_VPH_CPU1_EN_PORT, VDD_PCIE_VPH_CPU1_EN_PIN);

	LL_GPIO_SetOutputPin(VDD_PCIE_VP_CPU1_EN_PORT, VDD_PCIE_VP_CPU1_EN_PIN);

//	LL_GPIO_SetOutputPin(PS_ON_BMC_PORT, PS_ON_BMC_PIN);

	uint8_t str[] = "CDC Transmit\r\n";

	while(1) {
		mdelay(500);
		CDC_Transmit_FS(str, strlen((char*)str));
	}

	return 0;
}
#endif
