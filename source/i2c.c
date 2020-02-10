/**
 * @file	src/i2c.c
 *
 * @brief	Определения функций работы с шиной I2C.
 *
 * @author	Vasily Yurchenko <vasily.v.yurchenko@yandex.ru>
 */

#include "i2c.h"
#include "console.h"

#define GUARD_COUNTER_INIT 10000

void i2c_init(void)
{
	/** I2C1 GPIO Configuration
	 PB6   ------> I2C1_SCL
	 PB7   ------> I2C1_SDA
	 */

	LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOB);

	LL_GPIO_SetPinMode(GPIOB, LL_GPIO_PIN_6, LL_GPIO_MODE_ALTERNATE);
	LL_GPIO_SetAFPin_0_7(GPIOB, LL_GPIO_PIN_6, LL_GPIO_AF_4);
	LL_GPIO_SetPinSpeed(GPIOB, LL_GPIO_PIN_6, LL_GPIO_SPEED_FREQ_HIGH);
	LL_GPIO_SetPinOutputType(GPIOB, LL_GPIO_PIN_6, LL_GPIO_OUTPUT_PUSHPULL);
	LL_GPIO_SetPinPull(GPIOB, LL_GPIO_PIN_6, LL_GPIO_PULL_UP);

	LL_GPIO_SetPinMode(GPIOB, LL_GPIO_PIN_7, LL_GPIO_MODE_ALTERNATE);
	LL_GPIO_SetAFPin_0_7(GPIOB, LL_GPIO_PIN_7, LL_GPIO_AF_4);
	LL_GPIO_SetPinSpeed(GPIOB, LL_GPIO_PIN_7, LL_GPIO_SPEED_FREQ_HIGH);
	LL_GPIO_SetPinOutputType(GPIOB, LL_GPIO_PIN_7, LL_GPIO_OUTPUT_PUSHPULL);
	LL_GPIO_SetPinPull(GPIOB, LL_GPIO_PIN_7, LL_GPIO_PULL_UP);

	LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_I2C1);

	NVIC_SetPriority(I2C1_EV_IRQn, 0);
	NVIC_EnableIRQ(I2C1_EV_IRQn);

	NVIC_SetPriority(I2C1_ER_IRQn, 0);
	NVIC_EnableIRQ(I2C1_ER_IRQn);

/*
	I2CCLK = HSI == 8MHz (RCC_CFG3.I2CxSW == 0 default) or SYSCLK (RCC_CFG3.I2CxSW == 1);
	t_I2CCLK = 1 / I2CCLK;
	t_PRESC = (PRESC + 1) * t_I2CCLK;
	t_SCLDEL = (SCLDEL + 1) * t_PRESC;
	t_SDADEL = SDADEL * t_PRESC;
	t_SCLH = (SCLH + 1) * t_PRESC;
	t_SCLL = (SCLL + 1) * t_PRESC;

	I2CCLK == 8 MHz;
	I2C Clock = 100 KHz;
	PRESC = 0x1;
	SCLDEL = 0x4;
	SDADEL = 0x2;
	SCLH = 0x0F;
	SCLL = 0x13;

	I2CCLK == 48 MHz;
	I2C Clock = 100 KHz;
	PRESC = 0xB;
	SCLDEL = 0x4;
	SDADEL = 0x2;
	SCLH = 0x0F;
	SCLL = 0x13;
*/
	LL_I2C_Disable(I2C1);

	uint32_t PRESC = 0x1;
	uint32_t SCLDEL = 0x4;
	uint32_t SDADEL = 0x2;
	uint32_t SCLH = 0x0F;
	uint32_t SCLL = 0x13;

	LL_I2C_SetTiming(I2C1, PRESC << 28 | SCLDEL << 20 | SDADEL << 16 | SCLH << 8 | SCLL);

	LL_I2C_EnableIT_TX(I2C1);
	LL_I2C_EnableIT_RX(I2C1);
	LL_I2C_EnableIT_ADDR(I2C1);

	/* Прерывания не генерировать,
	 * это может нарушить ход синхронного приема.передачи
	LL_I2C_EnableIT_NACK(I2C1);
	LL_I2C_EnableIT_TC(I2C1);
	 */

	LL_I2C_EnableIT_STOP(I2C1);
	LL_I2C_EnableIT_ERR(I2C1);

	LL_I2C_Enable(I2C1);
}

int32_t i2c_read(uint8_t chip_addr, uint8_t reg_addr, uint8_t *buffer, size_t buffer_size)
{
	uint32_t guard_counter = GUARD_COUNTER_INIT;

	LL_I2C_HandleTransfer(I2C1, chip_addr, LL_I2C_ADDRSLAVE_7BIT, 1, LL_I2C_MODE_SOFTEND, LL_I2C_GENERATE_START_WRITE);

	if (LL_I2C_IsActiveFlag_NACK(I2C1) == 1)
		return -1;

	while (1) {

		if (LL_I2C_IsActiveFlag_TXIS(I2C1) == 1)
			LL_I2C_TransmitData8(I2C1, reg_addr);

		if (LL_I2C_IsActiveFlag_NACK(I2C1) == 1)
			return -2;

		if (LL_I2C_IsActiveFlag_TC(I2C1) == 1)
			break;

		if (guard_counter-- == 0)
			return -4;
	}

	LL_I2C_HandleTransfer(I2C1, chip_addr, LL_I2C_ADDRSLAVE_7BIT, buffer_size & 0xFF, LL_I2C_MODE_AUTOEND, LL_I2C_GENERATE_RESTART_7BIT_READ);

	if (LL_I2C_IsActiveFlag_NACK(I2C1) == 1)
		return -3;

	size_t pos = 0;
	guard_counter = GUARD_COUNTER_INIT;
	while (1) {

		if (LL_I2C_IsActiveFlag_RXNE(I2C1) == 1) {
			buffer[pos] = LL_I2C_ReceiveData8(I2C1);
			++pos;
		}

		if (LL_I2C_IsActiveFlag_TC(I2C1) == 1)
			break;

		if (guard_counter-- == 0)
			return -4;
	}
	return pos;
}

int32_t i2c_write(uint8_t chip_addr, uint8_t reg_addr, uint8_t *data, size_t data_size)
{
	uint32_t guard_counter = GUARD_COUNTER_INIT;

	LL_I2C_HandleTransfer(I2C1, chip_addr, LL_I2C_ADDRSLAVE_7BIT, data_size + 1, LL_I2C_MODE_AUTOEND, LL_I2C_GENERATE_START_WRITE);

	if (LL_I2C_IsActiveFlag_NACK(I2C1) == 1)
		return -1;

	while (1) {

		if (LL_I2C_IsActiveFlag_TXIS(I2C1) == 1) {
			LL_I2C_TransmitData8(I2C1, reg_addr);
			break;
		}

		if (LL_I2C_IsActiveFlag_NACK(I2C1) == 1)
			return -2;

		if (guard_counter-- == 0)
			return -3;
	}

	size_t pos = 0;
	guard_counter = GUARD_COUNTER_INIT;

	while (1) {

		if (LL_I2C_IsActiveFlag_TXIS(I2C1) == 1) {
			LL_I2C_TransmitData8(I2C1, data[pos++]);
			break;
		}

		if (LL_I2C_IsActiveFlag_NACK(I2C1) == 1)
			return -4;

		if (LL_I2C_IsActiveFlag_TC(I2C1) == 1)
			break;

		if (guard_counter-- == 0)
			return -5;
	}

	return pos;
}

void I2C1_EV_IRQHandler(void)
{
	uint32_t i2c1_isr = READ_BIT(I2C1->ISR, 0xFFFFFFFF);
	print("%s(): I2C1->ISR 0x%08X\r\n", __func__, i2c1_isr);

	/* Снять все прерывания. */
	WRITE_REG(I2C1->ICR, 0xFFFFFFFF);
}

void I2C1_ER_IRQHandler(void)
{
	uint32_t i2c1_isr = READ_BIT(I2C1->ISR, 0xFFFFFFFF);
	print("%s(): I2C1->ISR 0x%08X\r\n", __func__, i2c1_isr);

	/* Снять все прерывания. */
	WRITE_REG(I2C1->ICR, 0xFFFFFFFF);
}
