/**
 * @file	src/i2c.c
 *
 * @brief	Определения функций работы с шиной I2C.
 *
 * @author	Vasily Yurchenko <vasily.v.yurchenko@yandex.ru>
 */

#include "i2c.h"
#include "console.h"
#include "debug.h"
#include "time.h"

#define GUARD_COUNTER_INIT 10000

void i2c_init(void)
{
	/** I2C1 GPIO Configuration
	 PB6   ------> I2C1_SCL
	 PB7   ------> I2C1_SDA
	 */
#if 0
	LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOB);

	LL_GPIO_SetPinMode(GPIOB, LL_GPIO_PIN_6, LL_GPIO_MODE_ALTERNATE);
//	LL_GPIO_SetAFPin_0_7(GPIOB, LL_GPIO_PIN_6, LL_GPIO_AF_4);
	LL_GPIO_SetPinSpeed(GPIOB, LL_GPIO_PIN_6, LL_GPIO_SPEED_FREQ_HIGH);
	LL_GPIO_SetPinOutputType(GPIOB, LL_GPIO_PIN_6, LL_GPIO_OUTPUT_PUSHPULL);
	LL_GPIO_SetPinPull(GPIOB, LL_GPIO_PIN_6, LL_GPIO_PULL_UP);

	LL_GPIO_SetPinMode(GPIOB, LL_GPIO_PIN_7, LL_GPIO_MODE_ALTERNATE);
//	LL_GPIO_SetAFPin_0_7(GPIOB, LL_GPIO_PIN_7, LL_GPIO_AF_4);
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


	/* Прерывания не генерировать,
	 * это может нарушить ход синхронного приема.передачи
	LL_I2C_EnableIT_NACK(I2C1);
	LL_I2C_EnableIT_ADDR(I2C1);
	LL_I2C_EnableIT_TC(I2C1);
	LL_I2C_EnableIT_RX(I2C1);
	LL_I2C_EnableIT_TX(I2C1);
	LL_I2C_EnableIT_STOP(I2C1);
	LL_I2C_EnableIT_ERR(I2C1);
	 */


	LL_I2C_Enable(I2C1);
#endif

	/* (1) Enables GPIO clock  **********************/

	/* Enable the peripheral clock of GPIOB */
	LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOB);

	/* (2) Enable the I2C1 peripheral clock *************************************/

	/* Enable the peripheral clock for I2C1 */
	LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_I2C1);

	/* Configure SCL Pin as : Alternate function, High Speed, Open drain, Pull up */
	LL_GPIO_SetPinMode(GPIOB, LL_GPIO_PIN_6, LL_GPIO_MODE_ALTERNATE);
	LL_GPIO_SetPinSpeed(GPIOB, LL_GPIO_PIN_6, LL_GPIO_SPEED_FREQ_HIGH);
	LL_GPIO_SetPinOutputType(GPIOB, LL_GPIO_PIN_6, LL_GPIO_OUTPUT_OPENDRAIN);
	LL_GPIO_SetPinPull(GPIOB, LL_GPIO_PIN_6, LL_GPIO_PULL_UP);

	/* Configure SDA Pin as : Alternate function, High Speed, Open drain, Pull up */
	LL_GPIO_SetPinMode(GPIOB, LL_GPIO_PIN_7, LL_GPIO_MODE_ALTERNATE);
	LL_GPIO_SetPinSpeed(GPIOB, LL_GPIO_PIN_7, LL_GPIO_SPEED_FREQ_HIGH);
	LL_GPIO_SetPinOutputType(GPIOB, LL_GPIO_PIN_7, LL_GPIO_OUTPUT_OPENDRAIN);
	LL_GPIO_SetPinPull(GPIOB, LL_GPIO_PIN_7, LL_GPIO_PULL_UP);

	/* (3) Configure NVIC for I2C1 **********************************************/

	/* Configure Event IT:
	*  - Set priority for I2C1_EV_IRQn
	*  - Enable I2C1_EV_IRQn
	*/
	NVIC_SetPriority(I2C1_EV_IRQn, 0);
	NVIC_EnableIRQ(I2C1_EV_IRQn);

	/* Configure Error IT:
	*  - Set priority for I2C1_ER_IRQn
	*  - Enable I2C1_ER_IRQn
	*/
	NVIC_SetPriority(I2C1_ER_IRQn, 0);
	NVIC_EnableIRQ(I2C1_ER_IRQn);

	/* (4) Configure I2C1 functional parameters ********************************/

	/* Disable I2C1 prior modifying configuration registers */
	LL_I2C_Disable(I2C1);

	/* Configure the SCL Clock Speed */
	LL_I2C_ConfigSpeed(I2C1, 24000000, 100000, LL_I2C_DUTYCYCLE_2);

	/* Configure the Own Address1                   */
	/* Reset Values of :
	*     - OwnAddress1 is 0x00
	*     - OwnAddrSize is LL_I2C_OWNADDRESS1_7BIT
	*/
	//LL_I2C_SetOwnAddress1(I2C1, 0x00, LL_I2C_OWNADDRESS1_7BIT);

	/* Enable Clock stretching */
	/* Reset Value is Clock stretching enabled */
	//LL_I2C_EnableClockStretching(I2C1);


	/* Enable General Call                  */
	/* Reset Value is General Call disabled */
	//LL_I2C_EnableGeneralCall(I2C1);

	/* Configure the 7bits Own Address2     */
	/* Reset Values of :
	*     - OwnAddress2 is 0x00
	*     - Own Address2 is disabled
	*/
	//LL_I2C_SetOwnAddress2(I2C1, 0x00);
	//LL_I2C_DisableOwnAddress2(I2C1);

	/* Enable Peripheral in I2C mode */
	/* Reset Value is I2C mode */
	//LL_I2C_SetMode(I2C1, LL_I2C_MODE_I2C);

	LL_I2C_Enable(I2C1);

	/* (2) Enable I2C1 transfer event/error interrupts:
	*  - Enable Events interrupts
	*  - Enable Errors interrupts
	*/
//	LL_I2C_EnableIT_EVT(I2C1);
//	LL_I2C_EnableIT_ERR(I2C1);
}

int32_t i2c_read(uint8_t chip_addr, uint8_t reg_addr, uint8_t *buffer, size_t buffer_size)
{
#if defined(STM32F303x8)
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
			return -3;
	}

	LL_I2C_HandleTransfer(I2C1, chip_addr, LL_I2C_ADDRSLAVE_7BIT, buffer_size & 0xFF, LL_I2C_MODE_SOFTEND, LL_I2C_GENERATE_RESTART_7BIT_READ);

	if (LL_I2C_IsActiveFlag_NACK(I2C1) == 1)
		return -4;

	size_t pos = 0;
	guard_counter = GUARD_COUNTER_INIT;
	while (1) {

		if (LL_I2C_IsActiveFlag_RXNE(I2C1) == 1) {
			buffer[pos] = LL_I2C_ReceiveData8(I2C1);
			++pos;
		}

		if (LL_I2C_IsActiveFlag_TC(I2C1) == 1) {
			LL_I2C_GenerateStopCondition(I2C1);
			break;
		}

		if (guard_counter-- == 0)
			return -5;
	}
	return pos;

#elif defined(STM32F103xB)

	/* (2) Initiate a Start condition to the Slave device ***********************/
	/* Master Generate Start condition */
	LL_I2C_GenerateStartCondition(I2C1);
	printk(DEBUG, "1\r\n");
	/* (3) Loop until Start Bit transmitted (SB flag raised) ********************/
	/* Loop until SB flag is raised  */
	while(!LL_I2C_IsActiveFlag_SB(I2C1)) {
		;
	}
	printk(DEBUG, "2\r\n");
	/* (4) Send Slave address with a 7-Bit SLAVE_OWN_ADDRESS for a write request */
	LL_I2C_TransmitData8(I2C1, chip_addr | 0x0);
	printk(DEBUG, "3\r\n");
//	mdelay(200);
//	printk(DEBUG, "I2C1->SR1:0x%02lX\r\n", I2C1->SR1);
//	printk(DEBUG, "I2C1->SR2:0x%02lX\r\n", I2C1->SR2);
//	if (LL_I2C_IsActiveFlag_AF(I2C1))
//		return -1;

	/* (5) Loop until Address Acknowledgement received (ADDR flag raised) *******/
	/* Loop until ADDR flag is raised  */
	while(!LL_I2C_IsActiveFlag_ADDR(I2C1)) {
		;
	}
	printk(DEBUG, "4\r\n");
	/* (6) Clear ADDR flag and loop until end of transfer (ubNbDataToTransmit == 0) */
	/* Clear ADDR flag value in ISR register */
	LL_I2C_ClearFlag_ADDR(I2C1);

	LL_I2C_TransmitData8(I2C1, reg_addr);

	while(!LL_I2C_IsActiveFlag_TXE(I2C1)) {
		;
	}
	printk(DEBUG, "5\r\n");
/*******************************************************/

	LL_I2C_GenerateStartCondition(I2C1);

	while(!LL_I2C_IsActiveFlag_SB(I2C1)) {
			;
	}

	LL_I2C_TransmitData8(I2C1, chip_addr | 0x1);

	while(!LL_I2C_IsActiveFlag_ADDR(I2C1)) {
		;
	}

	LL_I2C_ClearFlag_ADDR(I2C1);


	size_t i = 0;
	while(i < buffer_size) {
		if(LL_I2C_IsActiveFlag_RXNE(I2C1)) {
			buffer[i] = LL_I2C_ReceiveData8(I2C1);
			if (++i == buffer_size)
				LL_I2C_AcknowledgeNextData(I2C1, LL_I2C_NACK);
			else
				LL_I2C_AcknowledgeNextData(I2C1, LL_I2C_ACK);
		}
	}
	/* (7) End of tranfer, Data consistency are checking into Slave process *****/
	/* Generate Stop condition */
	LL_I2C_GenerateStopCondition(I2C1);

	return 0;
#else
#warning "Unknown SoC type"
#endif
}

int32_t i2c_write(uint8_t chip_addr, uint8_t reg_addr, uint8_t *data, size_t data_size)
{
#if defined(STM32F303x8)
	uint32_t guard_counter = GUARD_COUNTER_INIT;

	LL_I2C_HandleTransfer(I2C1, chip_addr, LL_I2C_ADDRSLAVE_7BIT, data_size + 1, LL_I2C_MODE_SOFTEND, LL_I2C_GENERATE_START_WRITE);

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

		if (LL_I2C_IsActiveFlag_TC(I2C1) == 1) {
			LL_I2C_GenerateStopCondition(I2C1);
			break;
		}

		if (guard_counter-- == 0)
			return -5;
	}


	return pos;
#elif defined(STM32F103xB)
	return 0;
#else
#warning "Unknown SoC type"
#endif
}

void I2C1_EV_IRQHandler(void)
{
#if defined(STM32F303x8)
	uint32_t i2c1_isr = READ_BIT(I2C1->ISR, 0xFFFFFFFF);
	print("%s(): I2C1->ISR 0x%08X\r\n", __func__, i2c1_isr);

	/* Снять все прерывания. */
	WRITE_REG(I2C1->ICR, 0xFFFFFFFF);
#elif defined(STM32F103xB)
#else
#warning "Unknown SoC type"
#endif
}

void I2C1_ER_IRQHandler(void)
{
#if defined(STM32F303x8)
	uint32_t i2c1_isr = READ_BIT(I2C1->ISR, 0xFFFFFFFF);
	print("%s(): I2C1->ISR 0x%08X\r\n", __func__, i2c1_isr);

	/* Снять все прерывания. */
	WRITE_REG(I2C1->ICR, 0xFFFFFFFF);
#elif defined(STM32F103xB)

#else
#warning "Unknown SoC type"
#endif
}
