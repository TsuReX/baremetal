/**
 * @file	src/i2c.c
 *
 * @brief	Определения функций работы с шиной I2C.
 *
 * @author	Vasily Yurchenko <vasily.v.yurchenko@yandex.ru>
 */

#include "main.h"
#include "i2c.h"

/** Timing register value is computed with the STM32CubeMX Tool */
#define I2C_TIMING	0x2000090E

void i2c1_config(void)
{
	LL_I2C_InitTypeDef I2C_InitStruct;

	LL_GPIO_InitTypeDef GPIO_InitStruct;

	LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOB);
	/* Peripheral clock enable */
	LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_I2C1);

	/** I2C1 GPIO Configuration
	 PB6   ------> I2C1_SCL
	 PB7   ------> I2C1_SDA
	 */
	GPIO_InitStruct.Pin = LL_GPIO_PIN_6;
	GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
	GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
	GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
	GPIO_InitStruct.Pull = LL_GPIO_PULL_UP;
	GPIO_InitStruct.Alternate = LL_GPIO_AF_4;
	LL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = LL_GPIO_PIN_7;
	GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
	GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
	GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
	GPIO_InitStruct.Pull = LL_GPIO_PULL_UP;
	GPIO_InitStruct.Alternate = LL_GPIO_AF_4;
	LL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	/* USART1 interrupt Init */
	NVIC_SetPriority(I2C1_EV_IRQn, 0);
	NVIC_EnableIRQ(I2C1_EV_IRQn);

	I2C_InitStruct.PeripheralMode	= LL_I2C_MODE_SMBUS_HOST;
	I2C_InitStruct.Timing			= I2C_TIMING;
	I2C_InitStruct.AnalogFilter		= LL_I2C_ANALOGFILTER_ENABLE;
	I2C_InitStruct.DigitalFilter	= 0x00;
	I2C_InitStruct.TypeAcknowledge	= LL_I2C_ACK;
	I2C_InitStruct.OwnAddress1		= 0x00;
	LL_I2C_Init(I2C1, &I2C_InitStruct);

	LL_I2C_Enable(I2C1);
}
