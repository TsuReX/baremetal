#include "time.h"
#include "drivers.h"
#include "console.h"
#include "init.h"
#include "config.h"
#include "debug.h"
#include "usb_device.h"

#define GUARD_COUNTER_INIT	1000
#define I2C_REQUEST_WRITE	0x00
#define I2C_REQUEST_READ	0x01


extern PCD_HandleTypeDef peripheral_controller_driver;

void Error_Handler(void)
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

int32_t i2c_read(uint8_t chip_addr, uint8_t reg_addr, uint8_t *buffer, size_t buffer_size)
{
	uint32_t guard_counter = GUARD_COUNTER_INIT;

//	LL_I2C_DisableBitPOS(I2C1);

	LL_I2C_AcknowledgeNextData(I2C1, LL_I2C_ACK);

	LL_I2C_GenerateStartCondition(I2C1);

	while(!LL_I2C_IsActiveFlag_SB(I2C1)) {
		if (guard_counter-- == 0) {
			printk(DEBUG, "I2C1->SR1: 0x%02lX\r\n", READ_BIT(I2C1->SR1, 0xFF));
			printk(DEBUG, "I2C1->SR2: 0x%02lX\r\n", READ_BIT(I2C1->SR2, 0xFF));
			LL_I2C_GenerateStopCondition(I2C1);
			return -1;
		}
	}

	LL_I2C_TransmitData8(I2C1, chip_addr | I2C_REQUEST_WRITE);
//	return -10;
	guard_counter = GUARD_COUNTER_INIT;

	while(!LL_I2C_IsActiveFlag_ADDR(I2C1)) {
		if (guard_counter-- == 0) {
//			printk(DEBUG, "I2C1->SR1: 0x%02lX\r\n", READ_BIT(I2C1->SR1, 0xFF));
//			printk(DEBUG, "I2C1->SR2: 0x%02lX\r\n", READ_BIT(I2C1->SR2, 0xFF));
			LL_I2C_GenerateStopCondition(I2C1);
			return -2;
		}
	}

	LL_I2C_ClearFlag_ADDR(I2C1);

	LL_I2C_TransmitData8(I2C1, reg_addr);
	while(!LL_I2C_IsActiveFlag_TXE(I2C1)) {
		if (guard_counter-- == 0) {
//			printk(DEBUG, "I2C1->SR1: 0x%02lX\r\n", READ_BIT(I2C1->SR1, 0xFF));
//			printk(DEBUG, "I2C1->SR2: 0x%02lX\r\n", READ_BIT(I2C1->SR2, 0xFF));
			LL_I2C_GenerateStopCondition(I2C1);
			return -3;
		}
	}

/*********************************************/
	LL_I2C_GenerateStartCondition(I2C1);

	while(!LL_I2C_IsActiveFlag_SB(I2C1)) {
		if (guard_counter-- == 0) {
//			printk(DEBUG, "I2C1->SR1: 0x%02lX\r\n", READ_BIT(I2C1->SR1, 0xFF));
//			printk(DEBUG, "I2C1->SR2: 0x%02lX\r\n", READ_BIT(I2C1->SR2, 0xFF));
			LL_I2C_GenerateStopCondition(I2C1);
			return -4;
		}
	}

	LL_I2C_TransmitData8(I2C1, chip_addr | I2C_REQUEST_READ);

	guard_counter = GUARD_COUNTER_INIT;

	while(!LL_I2C_IsActiveFlag_ADDR(I2C1)) {
		if (guard_counter-- == 0) {
//			printk(DEBUG, "I2C1->SR1: 0x%02lX\r\n", READ_BIT(I2C1->SR1, 0xFF));
//			printk(DEBUG, "I2C1->SR2: 0x%02lX\r\n", READ_BIT(I2C1->SR2, 0xFF));
			LL_I2C_GenerateStopCondition(I2C1);
			return -5;
		}
	}

	LL_I2C_ClearFlag_ADDR(I2C1);

/*********************************************/
	guard_counter = GUARD_COUNTER_INIT;

	while(buffer_size > 1) {
		if(LL_I2C_IsActiveFlag_RXNE(I2C1)) {

			(*buffer++) = LL_I2C_ReceiveData8(I2C1);
			buffer_size--;
		}
		if (guard_counter-- == 0) {
//			printk(DEBUG, "I2C1->SR1: 0x%02lX\r\n", READ_BIT(I2C1->SR1, 0xFF));
//			printk(DEBUG, "I2C1->SR2: 0x%02lX\r\n", READ_BIT(I2C1->SR2, 0xFF));
			LL_I2C_GenerateStopCondition(I2C1);
			return -6;
		}
	}

	LL_I2C_AcknowledgeNextData(I2C1, LL_I2C_NACK);
	LL_I2C_GenerateStopCondition(I2C1);

	guard_counter = GUARD_COUNTER_INIT;
	while(buffer_size > 0) {
		if(LL_I2C_IsActiveFlag_RXNE(I2C1)) {

			(*buffer++) = LL_I2C_ReceiveData8(I2C1);
			buffer_size--;
		}
		if (guard_counter-- == 0) {
//			printk(DEBUG, "I2C1->SR1: 0x%02lX\r\n", READ_BIT(I2C1->SR1, 0xFF));
//			printk(DEBUG, "I2C1->SR2: 0x%02lX\r\n", READ_BIT(I2C1->SR2, 0xFF));
			LL_I2C_GenerateStopCondition(I2C1);
			return -6;
		}
	}

	return 0;
}

int32_t i2c_write(uint8_t chip_addr, uint8_t reg_addr, uint8_t *buffer, size_t buffer_size)
{
	uint32_t guard_counter = GUARD_COUNTER_INIT;

	/* (1) Prepare acknowledge for Master data reception ************************/
	LL_I2C_AcknowledgeNextData(I2C1, LL_I2C_ACK);
	printk(DEBUG, "i2c_write 1\r\n");
	/* (2) Initiate a Start condition to the Slave device ***********************/
	/* Master Generate Start condition */
	LL_I2C_GenerateStartCondition(I2C1);
	printk(DEBUG, "i2c_write 2\r\n");
	/* (3) Loop until Start Bit transmitted (SB flag raised) ********************/
	printk(DEBUG, "I2C1->SR1: 0x%02lX\r\n", READ_BIT(I2C1->SR1, 0xFF));
	printk(DEBUG, "I2C1->SR2: 0x%02lX\r\n", READ_BIT(I2C1->SR2, 0xFF));
	/* Loop until SB flag is raised  */
	while(!LL_I2C_IsActiveFlag_SB(I2C1)) {
		if (guard_counter-- == 0)
			return -1;
	}
	printk(DEBUG, "i2c_write 3\r\n");
	/* (4) Send Slave address with a 7-Bit SLAVE_OWN_ADDRESS for a write request */
	LL_I2C_TransmitData8(I2C1, chip_addr | I2C_REQUEST_WRITE);
	printk(DEBUG, "i2c_write 4\r\n");
	/* (5) Loop until Address Acknowledgement received (ADDR flag raised) *******/

	guard_counter = GUARD_COUNTER_INIT;
	/* Loop until ADDR flag is raised  */
	while(!LL_I2C_IsActiveFlag_ADDR(I2C1)) {
		if (guard_counter-- == 0)
			return -2;
	}
	printk(DEBUG, "i2c_write 5\r\n");
	/* (6) Clear ADDR flag and loop until end of transfer (ubNbDataToTransmit == 0) */

	/* Clear ADDR flag value in ISR register */
	LL_I2C_ClearFlag_ADDR(I2C1);
	printk(DEBUG, "i2c_write 6\r\n");
	guard_counter = GUARD_COUNTER_INIT;
	/* Loop until TXE flag is raised  */
	while(buffer_size > 0) {
		/* (6.1) Transmit data (TXE flag raised) **********************************/

		/* Check TXE flag value in ISR register */
		if(LL_I2C_IsActiveFlag_TXE(I2C1)) {
			/* Write data in Transmit Data register.
			TXE flag is cleared by writing data in TXDR register */
			LL_I2C_TransmitData8(I2C1, (*buffer++));

			buffer_size--;
		}
		if (guard_counter-- == 0)
			return -3;
	}
	printk(DEBUG, "i2c_write 7\r\n");
	/* (7) End of tranfer, Data consistency are checking into Slave process *****/
	/* Generate Stop condition */
	LL_I2C_GenerateStopCondition(I2C1);
	printk(DEBUG, "i2c_write 8\r\n");
	return 0;
}
#if 0
int main(void)
{
	soc_init();

	board_init();

	HAL_Init();
	usb_init();

	mdelay(50);

	console_init();

	log_level_set(DEBUG);

	i2c1_init();

	printk(DEBUG, "depo_baikalbmc\r\n");

	uint8_t chip_addr = 0x00;
	for (;chip_addr < 0x7F; ++chip_addr) {
		uint8_t reg = 0;
		int32_t ret_val = i2c_read(chip_addr << 1, 0x01, &reg, sizeof(reg));
		if (ret_val < 0) {
//			printk(DEBUG, "0x%02X error: %ld\r\n",chip_addr, ret_val);
			;
		}
		else {
			printk(DEBUG, "0x%02X value: 0x%X\r\n", chip_addr, reg);
		}
		mdelay(50);
	}

	uint32_t i = 0;
	while(1) {
		printk(DEBUG, "cycle 0x%lX\r\n", i++);
		LL_GPIO_SetOutputPin(GPIOA, LL_GPIO_PIN_6);
		LL_GPIO_ResetOutputPin(GPIOA, LL_GPIO_PIN_7);
		mdelay(500);
		LL_GPIO_ResetOutputPin(GPIOA, LL_GPIO_PIN_6);
		LL_GPIO_SetOutputPin(GPIOA, LL_GPIO_PIN_7);
		mdelay(500);
	}

	return 0;
}
#else
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
