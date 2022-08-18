#include <string.h>
#include "time.h"
#include "drivers.h"
#include "console.h"
#include "spi.h"
#include "init.h"
#include "config.h"
#include "debug.h"
#include "spi_flash.h"

#define GUARD_COUNTER_INIT	1000
#define D47_ADDR 0x40
#define D48_ADDR 0x42
#define D49_ADDR 0x44
#define D50_ADDR 0x46
#define BMC_ADDR 0x10

static void i2c1_init(void)
{

	/* USER CODE BEGIN I2C1_Init 0 */

	/* USER CODE END I2C1_Init 0 */

	LL_I2C_InitTypeDef I2C_InitStruct = {0};

	LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

	LL_RCC_SetI2CClockSource(LL_RCC_I2C1_CLKSOURCE_PCLK1);

	LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOB);
	/**I2C1 GPIO Configuration
	PB6   ------> I2C1_SCL
	PB7   ------> I2C1_SDA
	*/
	GPIO_InitStruct.Pin = LL_GPIO_PIN_6|LL_GPIO_PIN_7;
	GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
	GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_OPENDRAIN;
	GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
	GPIO_InitStruct.Alternate = LL_GPIO_AF_4;
	LL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	/* Peripheral clock enable */
	LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_I2C1);

	/* I2C1 interrupt Init */
	NVIC_SetPriority(I2C1_EV_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(),0, 0));
	NVIC_EnableIRQ(I2C1_EV_IRQn);

	/* USER CODE BEGIN I2C1_Init 1 */

	/* USER CODE END I2C1_Init 1 */

	/** I2C Initialization
	*/
	LL_I2C_EnableAutoEndMode(I2C1);
	LL_I2C_DisableOwnAddress2(I2C1);
	LL_I2C_DisableGeneralCall(I2C1);
//	LL_I2C_EnableClockStretching(I2C1);
	I2C_InitStruct.PeripheralMode = LL_I2C_MODE_I2C;
	I2C_InitStruct.Timing = 0x10707DBC;
	I2C_InitStruct.AnalogFilter = LL_I2C_ANALOGFILTER_ENABLE;
	I2C_InitStruct.DigitalFilter = 0;
	I2C_InitStruct.OwnAddress1 = 0x10;
	I2C_InitStruct.TypeAcknowledge = LL_I2C_ACK;
	I2C_InitStruct.OwnAddrSize = LL_I2C_OWNADDRESS1_7BIT;
	LL_I2C_Init(I2C1, &I2C_InitStruct);
	LL_I2C_SetOwnAddress2(I2C1, 0, LL_I2C_OWNADDRESS2_NOMASK);
	/* USER CODE BEGIN I2C1_Init 2 */

	/* USER CODE END I2C1_Init 2 */


	LL_I2C_Enable(I2C1);
	LL_I2C_EnableIT_ADDR(I2C1);
	LL_I2C_EnableIT_RX(I2C1);
	LL_I2C_EnableIT_STOP(I2C1);
}

void error_calback() {

}

#define MBM_BMC_REG_PWROFF_RQ		0x05
#define MBM_BMC_REG_PWROFF_RQ_OFF	0x01
#define MBM_BMC_REG_PWROFF_RQ_RESET	0x02

const uint8_t rstreq[] = {
	MBM_BMC_REG_PWROFF_RQ,
	MBM_BMC_REG_PWROFF_RQ_RESET
};

uint16_t request = 0x0;
uint32_t reset = 0x0;

void power_off();

void i2c1_ev_handler(void)
{
//	power_off();
	/* Check ADDR flag value in ISR register */
	if(LL_I2C_IsActiveFlag_ADDR(I2C1)) {
//		reset = 1;
//		LL_I2C_ClearFlag_ADDR(I2C1);
//		return;
		/* Verify the Address Match with the OWN Slave address */
		if(LL_I2C_GetAddressMatchCode(I2C1) == (BMC_ADDR)) {
//			reset = 1;
			/* Verify the transfer direction, a read direction, Slave enters transmitter mode */
			if(LL_I2C_GetTransferDirection(I2C1) == LL_I2C_DIRECTION_READ) {

				/* Slave -> Master request */

				/* Clear ADDR flag value in ISR register */
				LL_I2C_ClearFlag_ADDR(I2C1);

				/* Enable Transmit Interrupt */
//				LL_I2C_EnableIT_TX(I2C1);

			} else {

				/* Master -> Slave request */

				/* Clear ADDR flag value in ISR register */
				LL_I2C_ClearFlag_ADDR(I2C1);

				/* Enable Receive Interrupt */
				LL_I2C_EnableIT_RX(I2C1);
			}
		} else {

			/* Illegal address requested */

			/* Clear ADDR flag value in ISR register */
			LL_I2C_ClearFlag_ADDR(I2C1);

			/* Call Error function */
			error_calback();
		}

	/* Check NACK flag value in ISR register */
//	}  else if(LL_I2C_IsActiveFlag_NACK(I2C1)) {
//
//		/* End of Transfer */
//		LL_I2C_ClearFlag_NACK(I2C1);

	/* Check RXNE flag value in ISR register */
	} else if(LL_I2C_IsActiveFlag_RXNE(I2C1)) {

		/* Call function Slave Ready to Receive Callback */
		uint8_t data = LL_I2C_ReceiveData8(I2C1);

		request = (request << 8) & data;
//		if (request == (MBM_BMC_REG_PWROFF_RQ & MBM_BMC_REG_PWROFF_RQ_RESET)) {
//			request = 0;
//			reset = 1;
//		}

	/* Check TXIS flag value in ISR register */
//	} else if(LL_I2C_IsActiveFlag_TXIS(I2C1)) {
//
//		/* Call function Slave Ready to Transmit Callback */
//		Slave_Ready_To_Transmit_Callback();

	/* Check STOP flag value in ISR register */
	}  else if(LL_I2C_IsActiveFlag_STOP(I2C1)) {

		/* Clear STOP flag value in ISR register */
		LL_I2C_ClearFlag_STOP(I2C1);

		/* Check TXE flag value in ISR register */
		if(!LL_I2C_IsActiveFlag_TXE(I2C1)) {
			/* Flush the TXDR register */
			LL_I2C_ClearFlag_TXE(I2C1);
		}

		/* Call function Slave Complete Callback */
//		Slave_Complete_Callback();

		if (request == (MBM_BMC_REG_PWROFF_RQ & MBM_BMC_REG_PWROFF_RQ_RESET)) {
			request = 0;
			reset = 1;

			/* Disable Receive Interrupt */
			LL_I2C_DisableIT_RX(I2C1);
		}

	/* Check TXE flag value in ISR register */
//	} else if(!LL_I2C_IsActiveFlag_TXE(I2C1)) {
//
//		/* Do nothing */
//		/* This Flag will be set by hardware when the TXDR register is empty */
//		/* If needed, use LL_I2C_ClearFlag_TXE() interface to flush the TXDR register  */

	} else {

		/* Call Error function */
		error_calback();
	}
}

void i2c1_er_handler(void)
{
	error_calback();
}

static void i2c3_init(void)
{
	LL_I2C_InitTypeDef I2C_InitStruct = {0};

	LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

	LL_RCC_SetI2CClockSource(LL_RCC_I2C3_CLKSOURCE_PCLK1);

	LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOA);
	LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOB);
	/**I2C3 GPIO Configuration
	PA7   ------> I2C3_SCL
	PB4 (NJTRST)   ------> I2C3_SDA
	*/
	GPIO_InitStruct.Pin = LL_GPIO_PIN_7;
	GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
	GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_OPENDRAIN;
	GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
	GPIO_InitStruct.Alternate = LL_GPIO_AF_4;
	LL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = LL_GPIO_PIN_4;
	GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
	GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_OPENDRAIN;
	GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
	GPIO_InitStruct.Alternate = LL_GPIO_AF_4;
	LL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	/* Peripheral clock enable */
	LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_I2C3);

	/** I2C Initialization
	*/
	LL_I2C_EnableAutoEndMode(I2C3);
	LL_I2C_DisableOwnAddress2(I2C3);
	LL_I2C_DisableGeneralCall(I2C3);
	LL_I2C_EnableClockStretching(I2C3);
	I2C_InitStruct.PeripheralMode = LL_I2C_MODE_I2C;
	I2C_InitStruct.Timing = 0x10707DBC;
	I2C_InitStruct.AnalogFilter = LL_I2C_ANALOGFILTER_ENABLE;
	I2C_InitStruct.DigitalFilter = 0;
	I2C_InitStruct.OwnAddress1 = 0;
	I2C_InitStruct.TypeAcknowledge = LL_I2C_ACK;
	I2C_InitStruct.OwnAddrSize = LL_I2C_OWNADDRESS1_7BIT;
	LL_I2C_Init(I2C3, &I2C_InitStruct);
	LL_I2C_SetOwnAddress2(I2C3, 0, LL_I2C_OWNADDRESS2_NOMASK);
}

int32_t i2c_read(uint8_t chip_addr, uint8_t reg_addr, uint8_t *buffer, size_t buffer_size)
{
	uint32_t guard_counter = GUARD_COUNTER_INIT;

	LL_I2C_ClearFlag_NACK(I2C3);

	LL_I2C_HandleTransfer(I2C3, chip_addr, LL_I2C_ADDRSLAVE_7BIT, 1, LL_I2C_MODE_SOFTEND, LL_I2C_GENERATE_START_WRITE);

	if (LL_I2C_IsActiveFlag_NACK(I2C3) == 1)
		return -1;

	while (1) {

		if (LL_I2C_IsActiveFlag_TXIS(I2C3) == 1)
			LL_I2C_TransmitData8(I2C3, reg_addr);

		if (LL_I2C_IsActiveFlag_NACK(I2C3) == 1)
			return -2;

		if (LL_I2C_IsActiveFlag_TC(I2C3) == 1)
			break;

		if (guard_counter-- == 0)
			return -3;
	}

	LL_I2C_HandleTransfer(I2C3, chip_addr, LL_I2C_ADDRSLAVE_7BIT, buffer_size & 0xFF, LL_I2C_MODE_SOFTEND, LL_I2C_GENERATE_RESTART_7BIT_READ);

	if (LL_I2C_IsActiveFlag_NACK(I2C3) == 1)
		return -4;

	size_t pos = 0;
	guard_counter = GUARD_COUNTER_INIT;
	while (1) {

		if (LL_I2C_IsActiveFlag_RXNE(I2C3) == 1) {
			buffer[pos] = LL_I2C_ReceiveData8(I2C3);
			++pos;
		}

		if (LL_I2C_IsActiveFlag_TC(I2C3) == 1) {
			LL_I2C_GenerateStopCondition(I2C3);
			break;
		}

		if (guard_counter-- == 0)
			return -5;
	}
	return pos;
}

int32_t i2c_write(uint8_t chip_addr, uint8_t reg_addr, uint8_t *data, size_t data_size)
{
	uint32_t guard_counter = GUARD_COUNTER_INIT;

	LL_I2C_ClearFlag_NACK(I2C3);

	LL_I2C_HandleTransfer(I2C3, chip_addr, LL_I2C_ADDRSLAVE_7BIT, data_size + 1, LL_I2C_MODE_SOFTEND, LL_I2C_GENERATE_START_WRITE);

	if (LL_I2C_IsActiveFlag_NACK(I2C3) == 1)
		return -1;

	while (1) {

		if (LL_I2C_IsActiveFlag_TXIS(I2C3) == 1) {
			LL_I2C_TransmitData8(I2C3, reg_addr);
			break;
		}

		if (LL_I2C_IsActiveFlag_NACK(I2C3) == 1)
			return -2;

		if (guard_counter-- == 0)
			return -3;
	}

	size_t pos = 0;
	guard_counter = GUARD_COUNTER_INIT;

	while (1) {

		if (LL_I2C_IsActiveFlag_TXIS(I2C3) == 1) {
			LL_I2C_TransmitData8(I2C3, data[pos++]);
			break;
		}

		if (LL_I2C_IsActiveFlag_NACK(I2C3) == 1)
			return -4;

		if (LL_I2C_IsActiveFlag_TC(I2C3) == 1) {
			LL_I2C_GenerateStopCondition(I2C3);
			break;
		}

		if (guard_counter-- == 0)
			return -5;
	}

	return pos;
}

void pwr_3v3_off()
{
	uint8_t reg1 = 0x0;
	i2c_read(D48_ADDR, 0x1, &reg1, sizeof(reg1));
	reg1 &= 0xFE;
	i2c_write(D48_ADDR, 0x1, &reg1, sizeof(reg1));
}

void pwr_3v3_on()
{
	uint8_t reg1 = 0x0;
	i2c_read(D48_ADDR, 0x1, &reg1, sizeof(reg1));
	reg1 |= 0x1;
	i2c_write(D48_ADDR, 0x1, &reg1, sizeof(reg1));
}

uint32_t pwr_3v3_status()
{
	uint8_t reg0 = 0xFF;
	i2c_read(D49_ADDR, 0x0, &reg0, sizeof(reg0));
	return (reg0 >> 0) & 0x1;
}

void pwr_5v_off()
{
	uint8_t reg1 = 0x0;
	i2c_read(D47_ADDR, 0x1, &reg1, sizeof(reg1));
	reg1 &= 0xFE;
	i2c_write(D47_ADDR, 0x1, &reg1, sizeof(reg1));
}

void pwr_5v_on()
{
	uint8_t reg1 = 0x0;
	i2c_read(D47_ADDR, 0x1, &reg1, sizeof(reg1));
	reg1 |= 0x1;
	i2c_write(D47_ADDR, 0x1, &reg1, sizeof(reg1));
}

uint32_t pwr_5v_status()
{
	uint8_t reg0 = 0xFF;
	i2c_read(D47_ADDR, 0x0, &reg0, sizeof(reg0));
	return (reg0 >> 4) & 0x1;
}

void pwr_usb_5v_off()
{
	uint8_t reg1 = 0x0;
	i2c_read(D47_ADDR, 0x1, &reg1, sizeof(reg1));
	reg1 &= (~0x2);
	i2c_write(D47_ADDR, 0x1, &reg1, sizeof(reg1));
}

void pwr_usb_5v_on()
{
	uint8_t reg1 = 0x0;
	i2c_read(D47_ADDR, 0x1, &reg1, sizeof(reg1));
	reg1 |= 0x2;
	i2c_write(D47_ADDR, 0x1, &reg1, sizeof(reg1));
}

uint32_t pwr_usb_5v_status()
{
	uint8_t reg0 = 0xFF;
	i2c_read(D47_ADDR, 0x0, &reg0, sizeof(reg0));
	return (reg0 >> 5) & 0x1;
}

void pwr_sw_1v_off()
{
	uint8_t reg1 = 0x0;
	i2c_read(D47_ADDR, 0x1, &reg1, sizeof(reg1));
	reg1 &= (~0x4);
	i2c_write(D47_ADDR, 0x1, &reg1, sizeof(reg1));
}

void pwr_sw_1v_on()
{
	uint8_t reg1 = 0x0;
	i2c_read(D47_ADDR, 0x1, &reg1, sizeof(reg1));
	reg1 |= 0x4;
	i2c_write(D47_ADDR, 0x1, &reg1, sizeof(reg1));
}

uint32_t pwr_sw_1v_status()
{
	uint8_t reg0 = 0xFF;
	i2c_read(D47_ADDR, 0x0, &reg0, sizeof(reg0));
	return (reg0 >> 6) & 0x1;
}

void pwr_hdd_off()
{
	uint8_t reg1 = 0x0;
	i2c_read(D47_ADDR, 0x1, &reg1, sizeof(reg1));
	reg1 &= (~0x80);
	i2c_write(D47_ADDR, 0x1, &reg1, sizeof(reg1));
}

void pwr_hdd_on()
{
	uint8_t reg1 = 0x0;
	i2c_read(D47_ADDR, 0x1, &reg1, sizeof(reg1));
	reg1 |= 0x80;
	i2c_write(D47_ADDR, 0x1, &reg1, sizeof(reg1));
}

void pwr_1v5_off()
{
	uint8_t reg1 = 0x0;
	i2c_read(D48_ADDR, 0x1, &reg1, sizeof(reg1));
	reg1 &= (~0x2);
	i2c_write(D48_ADDR, 0x1, &reg1, sizeof(reg1));
}

void pwr_1v5_on()
{
	uint8_t reg1 = 0x0;
	i2c_read(D48_ADDR, 0x1, &reg1, sizeof(reg1));
	reg1 |= 0x2;
	i2c_write(D48_ADDR, 0x1, &reg1, sizeof(reg1));
}

uint32_t pwr_1v5_status()
{
	uint8_t reg0 = 0xFF;
	i2c_read(D49_ADDR, 0x0, &reg0, sizeof(reg0));
	return (reg0 >> 1) & 0x1;
}

void pwr_vdram_off()
{
	uint8_t reg1 = 0x0;
	i2c_read(D48_ADDR, 0x1, &reg1, sizeof(reg1));
	reg1 &= (~0x4);
	i2c_write(D48_ADDR, 0x1, &reg1, sizeof(reg1));
}

void pwr_vdram_on()
{
	uint8_t reg1 = 0x0;
	i2c_read(D48_ADDR, 0x1, &reg1, sizeof(reg1));
	reg1 |= 0x4;
	i2c_write(D48_ADDR, 0x1, &reg1, sizeof(reg1));
}

uint32_t pwr_vdram_status()
{
	uint8_t reg0 = 0xFF;
	i2c_read(D49_ADDR, 0x0, &reg0, sizeof(reg0));
	return (reg0 >> 2) & 0x1;
}

void pwr_1v8_off()
{
	uint8_t reg1 = 0x0;
	i2c_read(D48_ADDR, 0x1, &reg1, sizeof(reg1));
	reg1 &= (~0x8);
	i2c_write(D48_ADDR, 0x1, &reg1, sizeof(reg1));
}

void pwr_1v8_on()
{
	uint8_t reg1 = 0x0;
	i2c_read(D48_ADDR, 0x1, &reg1, sizeof(reg1));
	reg1 |= 0x8;
	i2c_write(D48_ADDR, 0x1, &reg1, sizeof(reg1));
}

uint32_t pwr_1v8_status()
{
	uint8_t reg0 = 0xFF;
	i2c_read(D49_ADDR, 0x0, &reg0, sizeof(reg0));
	return (reg0 >> 3) & 0x1;
}

void pwr_0v95_off()
{
	uint8_t reg1 = 0x0;
	i2c_read(D48_ADDR, 0x1, &reg1, sizeof(reg1));
	reg1 &= (~0x10);
	i2c_write(D48_ADDR, 0x1, &reg1, sizeof(reg1));
}

void pwr_0v95_on()
{
	uint8_t reg1 = 0x0;
	i2c_read(D48_ADDR, 0x1, &reg1, sizeof(reg1));
	reg1 |= 0x10;
	i2c_write(D48_ADDR, 0x1, &reg1, sizeof(reg1));
}

uint32_t pwr_0v95_status()
{
	uint8_t reg0 = 0xFF;
	i2c_read(D49_ADDR, 0x0, &reg0, sizeof(reg0));
	return (reg0 >> 4) & 0x1;
}

void cpu_clk_off()
{
	uint8_t reg1 = 0x0;
	i2c_read(D48_ADDR, 0x1, &reg1, sizeof(reg1));
	reg1 &= (~0x20);
	i2c_write(D48_ADDR, 0x1, &reg1, sizeof(reg1));
}

void cpu_clk_on()
{
	uint8_t reg1 = 0x0;
	i2c_read(D48_ADDR, 0x1, &reg1, sizeof(reg1));
	reg1 |= 0x20;
	i2c_write(D48_ADDR, 0x1, &reg1, sizeof(reg1));
}

void scp_trstn_low()
{
	uint8_t reg1 = 0x0;
	i2c_read(D48_ADDR, 0x1, &reg1, sizeof(reg1));
	reg1 &= (~0x40);
	i2c_write(D48_ADDR, 0x1, &reg1, sizeof(reg1));
}

void scp_trstn_high()
{
	uint8_t reg1 = 0x0;
	i2c_read(D48_ADDR, 0x1, &reg1, sizeof(reg1));
	reg1 |= 0x40;
	i2c_write(D48_ADDR, 0x1, &reg1, sizeof(reg1));
}

void cpu_trstn_low()
{
	uint8_t reg1 = 0x0;
	i2c_read(D48_ADDR, 0x1, &reg1, sizeof(reg1));
	reg1 &= (~0x80);
	i2c_write(D48_ADDR, 0x1, &reg1, sizeof(reg1));
}

void cpu_trstn_high()
{
	uint8_t reg1 = 0x0;
	i2c_read(D48_ADDR, 0x1, &reg1, sizeof(reg1));
	reg1 |= 0x80;
	i2c_write(D48_ADDR, 0x1, &reg1, sizeof(reg1));
}

void pwr_pll_0v9_off()
{
	uint8_t reg1 = 0x0;
	i2c_read(D49_ADDR, 0x1, &reg1, sizeof(reg1));
	reg1 &= (~0x20);
	i2c_write(D49_ADDR, 0x1, &reg1, sizeof(reg1));
}

void pwr_pll_0v9_on()
{
	uint8_t reg1 = 0x0;
	i2c_read(D49_ADDR, 0x1, &reg1, sizeof(reg1));
	reg1 |= 0x20;
	i2c_write(D49_ADDR, 0x1, &reg1, sizeof(reg1));
}

void sysfl_connect_to_cpu()
{
	uint8_t reg1 = 0x0;
	i2c_read(D50_ADDR, 0x1, &reg1, sizeof(reg1));
	reg1 &= (~0x1);
	i2c_write(D50_ADDR, 0x1, &reg1, sizeof(reg1));
}

void sysfl_connect_to_bmc()
{
	uint8_t reg1 = 0x0;
	i2c_read(D49_ADDR, 0x1, &reg1, sizeof(reg1));
	reg1 |= 0x1;
	i2c_write(D49_ADDR, 0x1, &reg1, sizeof(reg1));
}

void btfl_connect_to_cpu()
{
	uint8_t reg1 = 0x0;
	i2c_read(D50_ADDR, 0x1, &reg1, sizeof(reg1));
	reg1 &= (~0x2);
	i2c_write(D50_ADDR, 0x1, &reg1, sizeof(reg1));
}

void btfl_connect_to_bmc()
{
	uint8_t reg1 = 0x0;
	i2c_read(D50_ADDR, 0x1, &reg1, sizeof(reg1));
	reg1 |= 0x2;
	i2c_write(D50_ADDR, 0x1, &reg1, sizeof(reg1));
}

void hdmi_27mhz_off()
{
	uint8_t reg1 = 0x0;
	i2c_read(D50_ADDR, 0x1, &reg1, sizeof(reg1));
	reg1 &= (~0x4);
	i2c_write(D50_ADDR, 0x1, &reg1, sizeof(reg1));
}

void hdmi_27mhz_on()
{
	uint8_t reg1 = 0x0;
	i2c_read(D50_ADDR, 0x1, &reg1, sizeof(reg1));
	reg1 |= 0x4;
	i2c_write(D50_ADDR, 0x1, &reg1, sizeof(reg1));
}

void usb_clk_off()
{
	uint8_t reg1 = 0x0;
	i2c_read(D50_ADDR, 0x1, &reg1, sizeof(reg1));
	reg1 &= (~0x8);
	i2c_write(D50_ADDR, 0x1, &reg1, sizeof(reg1));
}

void usb_clk_on()
{
	uint8_t reg1 = 0x0;
	i2c_read(D50_ADDR, 0x1, &reg1, sizeof(reg1));
	reg1 |= 0x8;
	i2c_write(D50_ADDR, 0x1, &reg1, sizeof(reg1));
}

void cpu_speed_low()
{
	uint8_t reg1 = 0x0;
	i2c_read(D50_ADDR, 0x1, &reg1, sizeof(reg1));
	reg1 &= (~0x10);
	i2c_write(D50_ADDR, 0x1, &reg1, sizeof(reg1));
}

void cpu_speed_high()
{
	uint8_t reg1 = 0x0;
	i2c_read(D50_ADDR, 0x1, &reg1, sizeof(reg1));
	reg1 |= 0x10;
	i2c_write(D50_ADDR, 0x1, &reg1, sizeof(reg1));
}

/* This mode is correct and cpu can be booted */
void cpu_boot_mode_rom()
{
	uint8_t reg1 = 0x0;
	i2c_read(D50_ADDR, 0x1, &reg1, sizeof(reg1));
	reg1 &= (~0x20);
	i2c_write(D50_ADDR, 0x1, &reg1, sizeof(reg1));
}

/* This mode has incrystal problem and doesn't work correctly! */
void cpu_boot_mode_spi()
{
	uint8_t reg1 = 0x0;
	i2c_read(D50_ADDR, 0x1, &reg1, sizeof(reg1));
	reg1 |= 0x20;
	i2c_write(D50_ADDR, 0x1, &reg1, sizeof(reg1));
}

void cpu_reset_low()
{
	LL_GPIO_ResetOutputPin(GPIOB, LL_GPIO_PIN_3);

	// PCIE assert reset
	// D49.12 <= 0;
	uint8_t reg1 = 0x0;
	i2c_read(D49_ADDR, 0x1, &reg1, sizeof(reg1));
	reg1 &= (~0x80);
	i2c_write(D49_ADDR, 0x1, &reg1, sizeof(reg1));

}

void cpu_reset_high()
{
	LL_GPIO_SetOutputPin(GPIOB, LL_GPIO_PIN_3);

	// PCIE deassert reset
	// D49.12 <= 1;
	uint8_t reg1 = 0x0;
	i2c_read(D49_ADDR, 0x1, &reg1, sizeof(reg1));
	reg1 |= 0x80;
	i2c_write(D49_ADDR, 0x1, &reg1, sizeof(reg1));
}

int32_t power_on(void)
{
	cpu_reset_low();
	sysfl_connect_to_cpu();
	btfl_connect_to_cpu();

//	cpu_boot_mode_rom();
//	cpu_speed_low();

	pwr_5v_on();
	mdelay(100);
	if (pwr_5v_status() != 1) {
		printk(DEBUG, "5v startup failure\r\n");
		return -2;
	}
	printk(DEBUG, "5v startup ok\r\n");

	pwr_sw_1v_on();

	pwr_usb_5v_on();

	pwr_3v3_on();
	mdelay(100);
	if (pwr_3v3_status() != 1) {
		printk(DEBUG, "3v3 startup failure\r\n");
		return -1;
	}
	printk(DEBUG, "3v3 startup ok\r\n");

	pwr_1v5_on();
	mdelay(100);
	if (pwr_1v5_status() != 1) {
		printk(DEBUG, "1v5 startup failure\r\n");
		return -3;
	}
	printk(DEBUG, "1v5 startup ok\r\n");

	pwr_1v8_on();
	mdelay(100);
	if (pwr_1v8_status() != 1) {
		printk(DEBUG, "1v8 startup failure\r\n");
		return -5;
	}
	printk(DEBUG, "1v8 startup ok\r\n");

	hdmi_27mhz_on();
	usb_clk_on();
	mdelay(10);
	printk(DEBUG, "HDMI clock startup\r\n");

	pwr_pll_0v9_on();
	mdelay(1);
	printk(DEBUG, "PLL 0v9 startup\r\n");

	pwr_vdram_on();
	mdelay(100);
	if (pwr_vdram_status() != 1) {
		printk(DEBUG, "vdram startup failure\r\n");
		return -4;
	}
	printk(DEBUG, "vdram startup ok\r\n");

	cpu_trstn_high();
	mdelay(100);
	printk(DEBUG, "cpu trstn high\r\n");

	scp_trstn_high();
	mdelay(100);
	printk(DEBUG, "scp trstn high\r\n");

	pwr_0v95_on();
	mdelay(100);
	if (pwr_0v95_status() != 1) {
		printk(DEBUG, "0v95 startup failure\r\n");
		return -6;
	}
	printk(DEBUG, "0v95 startup ok\r\n");

	cpu_clk_on();
	mdelay(100);
	printk(DEBUG, "CPU clock startup\r\n");

	cpu_reset_high();
	printk(DEBUG, "cpu reset high\r\n");

	pwr_hdd_on();
	printk(DEBUG, "pwr hdd on\r\n");

	return 0;
}

void power_off(void)
{
	cpu_reset_low();

	pwr_hdd_off();

	pwr_3v3_off();

	pwr_5v_off();

	cpu_clk_off();

	hdmi_27mhz_off();

	pwr_1v8_off();

	pwr_pll_0v9_off();

	pwr_vdram_off();

	cpu_trstn_low();

	scp_trstn_low();

	pwr_1v5_off();

	pwr_0v95_off();
}

void pwr_switches_init()
{
	/* Disable all outputs */
	uint8_t reg1 = 0x0;

	/* 0 - output direction
	 * 1 - input direction
	 */
	/* 0111X000 */
	uint8_t reg3 = 0x70;
	i2c_write(D47_ADDR, 0x1, &reg1, sizeof(reg1));
	i2c_write(D47_ADDR, 0x3, &reg3, sizeof(reg3));

	/* 00000000 */
	reg3 = 0x00;
	i2c_write(D48_ADDR, 0x1, &reg1, sizeof(reg1));
	i2c_write(D48_ADDR, 0x3, &reg3, sizeof(reg3));

	/* 0X011111 */
	reg3 = 0x1F;
	i2c_write(D49_ADDR, 0x1, &reg1, sizeof(reg1));
	i2c_write(D49_ADDR, 0x3, &reg3, sizeof(reg3));

	/* XX110000 */
	reg3 = 0x30;
	i2c_write(D50_ADDR, 0x1, &reg1, sizeof(reg1));
	i2c_write(D50_ADDR, 0x3, &reg3, sizeof(reg3));
}

int main(void)
{
	soc_init();

	board_init();

	i2c1_init();
	i2c3_init();

	pwr_switches_init();

	console_init();

	mdelay(2000);
	power_off();

	log_level_set(DEBUG);
	mdelay(100);

	printk(DEBUG, "BAIKAL BMC\r\n");
	printk(DEBUG, "Start power up\r\n");
	int32_t ret_val = power_on();

	if (ret_val != 0) {
		power_off();
		printk(DEBUG, "Power up failure, step %ld\r\n", -1 * ret_val);
		printk(DEBUG, "Power down\r\n");
	} else {
		printk(DEBUG, "Power up is successful\r\n");
	}

	while(1) {
		if (LL_GPIO_IsInputPinSet(GPIOA, LL_GPIO_PIN_10) == 0) {
			cpu_reset_low();
		} else {
			cpu_reset_high();
		}

		if (reset == 1) {
			cpu_reset_low();
			reset = 0;
		}

		mdelay(10);

	}

	return 0;
}
