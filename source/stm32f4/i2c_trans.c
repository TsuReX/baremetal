#include "i2c_trans.h"

#define GUARD_COUNTER_INIT	1000
#define I2C_REQUEST_WRITE	0x00
#define I2C_REQUEST_READ	0x01

int32_t i2c_read(I2C_TypeDef *i2c_bus, uint8_t chip_addr, uint8_t reg_addr, uint8_t *buffer, size_t buffer_size)
{
	uint32_t guard_counter = GUARD_COUNTER_INIT;

//	LL_I2C_DisableBitPOS(i2c_bus);

	LL_I2C_AcknowledgeNextData(i2c_bus, LL_I2C_ACK);

	LL_I2C_GenerateStartCondition(i2c_bus);

	while(!LL_I2C_IsActiveFlag_SB(i2c_bus)) {
		if (guard_counter-- == 0) {
//			printk(DEBUG, "i2c_bus->SR1: 0x%02lX\r\n", READ_BIT(i2c_bus->SR1, 0xFF));
//			printk(DEBUG, "i2c_bus->SR2: 0x%02lX\r\n", READ_BIT(i2c_bus->SR2, 0xFF));
			LL_I2C_GenerateStopCondition(i2c_bus);
			return -1;
		}
	}

	LL_I2C_TransmitData8(i2c_bus, chip_addr | I2C_REQUEST_WRITE);
//	return -10;
	guard_counter = GUARD_COUNTER_INIT;

	while(!LL_I2C_IsActiveFlag_ADDR(i2c_bus)) {
		if (guard_counter-- == 0) {
//			printk(DEBUG, "i2c_bus->SR1: 0x%02lX\r\n", READ_BIT(i2c_bus->SR1, 0xFF));
//			printk(DEBUG, "i2c_bus->SR2: 0x%02lX\r\n", READ_BIT(i2c_bus->SR2, 0xFF));
			LL_I2C_GenerateStopCondition(i2c_bus);
			return -2;
		}
	}

	LL_I2C_ClearFlag_ADDR(i2c_bus);

	LL_I2C_TransmitData8(i2c_bus, reg_addr);
	while(!LL_I2C_IsActiveFlag_TXE(i2c_bus)) {
		if (guard_counter-- == 0) {
//			printk(DEBUG, "i2c_bus->SR1: 0x%02lX\r\n", READ_BIT(i2c_bus->SR1, 0xFF));
//			printk(DEBUG, "i2c_bus->SR2: 0x%02lX\r\n", READ_BIT(i2c_bus->SR2, 0xFF));
			LL_I2C_GenerateStopCondition(i2c_bus);
			return -3;
		}
	}

/*********************************************/
	LL_I2C_GenerateStartCondition(i2c_bus);

	while(!LL_I2C_IsActiveFlag_SB(i2c_bus)) {
		if (guard_counter-- == 0) {
//			printk(DEBUG, "i2c_bus->SR1: 0x%02lX\r\n", READ_BIT(i2c_bus->SR1, 0xFF));
//			printk(DEBUG, "i2c_bus->SR2: 0x%02lX\r\n", READ_BIT(i2c_bus->SR2, 0xFF));
			LL_I2C_GenerateStopCondition(i2c_bus);
			return -4;
		}
	}

	LL_I2C_TransmitData8(i2c_bus, chip_addr | I2C_REQUEST_READ);

	guard_counter = GUARD_COUNTER_INIT;

	while(!LL_I2C_IsActiveFlag_ADDR(i2c_bus)) {
		if (guard_counter-- == 0) {
//			printk(DEBUG, "i2c_bus->SR1: 0x%02lX\r\n", READ_BIT(i2c_bus->SR1, 0xFF));
//			printk(DEBUG, "i2c_bus->SR2: 0x%02lX\r\n", READ_BIT(i2c_bus->SR2, 0xFF));
			LL_I2C_GenerateStopCondition(i2c_bus);
			return -5;
		}
	}

	LL_I2C_ClearFlag_ADDR(i2c_bus);

/*********************************************/
	guard_counter = GUARD_COUNTER_INIT;

	while(buffer_size > 1) {
		if(LL_I2C_IsActiveFlag_RXNE(i2c_bus)) {

			(*buffer++) = LL_I2C_ReceiveData8(i2c_bus);
			buffer_size--;
		}
		if (guard_counter-- == 0) {
//			printk(DEBUG, "i2c_bus->SR1: 0x%02lX\r\n", READ_BIT(i2c_bus->SR1, 0xFF));
//			printk(DEBUG, "i2c_bus->SR2: 0x%02lX\r\n", READ_BIT(i2c_bus->SR2, 0xFF));
			LL_I2C_GenerateStopCondition(i2c_bus);
			return -6;
		}
	}

	LL_I2C_AcknowledgeNextData(i2c_bus, LL_I2C_NACK);
	LL_I2C_GenerateStopCondition(i2c_bus);

	guard_counter = GUARD_COUNTER_INIT;
	while(buffer_size > 0) {
		if(LL_I2C_IsActiveFlag_RXNE(i2c_bus)) {

			(*buffer++) = LL_I2C_ReceiveData8(i2c_bus);
			buffer_size--;
		}
		if (guard_counter-- == 0) {
//			printk(DEBUG, "i2c_bus->SR1: 0x%02lX\r\n", READ_BIT(i2c_bus->SR1, 0xFF));
//			printk(DEBUG, "i2c_bus->SR2: 0x%02lX\r\n", READ_BIT(i2c_bus->SR2, 0xFF));
			LL_I2C_GenerateStopCondition(i2c_bus);
			return -6;
		}
	}

	return 0;
}

int32_t i2c_write(I2C_TypeDef *i2c_bus, uint8_t chip_addr, uint8_t reg_addr, uint8_t *buffer, size_t buffer_size)
{
	uint32_t guard_counter = GUARD_COUNTER_INIT;

	/* (1) Prepare acknowledge for Master data reception ************************/
	LL_I2C_AcknowledgeNextData(i2c_bus, LL_I2C_ACK);
//	printk(DEBUG, "i2c_write 1\r\n");
	/* (2) Initiate a Start condition to the Slave device ***********************/
	/* Master Generate Start condition */
	LL_I2C_GenerateStartCondition(i2c_bus);
//	printk(DEBUG, "i2c_write 2\r\n");
	/* (3) Loop until Start Bit transmitted (SB flag raised) ********************/
//	printk(DEBUG, "i2c_bus->SR1: 0x%02lX\r\n", READ_BIT(i2c_bus->SR1, 0xFF));
//	printk(DEBUG, "i2c_bus->SR2: 0x%02lX\r\n", READ_BIT(i2c_bus->SR2, 0xFF));
	/* Loop until SB flag is raised  */
	while(!LL_I2C_IsActiveFlag_SB(i2c_bus)) {
		if (guard_counter-- == 0)
			return -1;
	}
//	printk(DEBUG, "i2c_write 3\r\n");
	/* (4) Send Slave address with a 7-Bit SLAVE_OWN_ADDRESS for a write request */
	LL_I2C_TransmitData8(i2c_bus, chip_addr | I2C_REQUEST_WRITE);
//	printk(DEBUG, "i2c_write 4\r\n");
	/* (5) Loop until Address Acknowledgement received (ADDR flag raised) *******/

	guard_counter = GUARD_COUNTER_INIT;
	/* Loop until ADDR flag is raised  */
	while(!LL_I2C_IsActiveFlag_ADDR(i2c_bus)) {
		if (guard_counter-- == 0)
			return -2;
	}
//	printk(DEBUG, "i2c_write 5\r\n");
	/* (6) Clear ADDR flag and loop until end of transfer (ubNbDataToTransmit == 0) */

	/* Clear ADDR flag value in ISR register */
	LL_I2C_ClearFlag_ADDR(i2c_bus);
//	printk(DEBUG, "i2c_write 6\r\n");
	guard_counter = GUARD_COUNTER_INIT;
	/* Loop until TXE flag is raised  */
	while(buffer_size > 0) {
		/* (6.1) Transmit data (TXE flag raised) **********************************/

		/* Check TXE flag value in ISR register */
		if(LL_I2C_IsActiveFlag_TXE(i2c_bus)) {
			/* Write data in Transmit Data register.
			TXE flag is cleared by writing data in TXDR register */
			LL_I2C_TransmitData8(i2c_bus, (*buffer++));

			buffer_size--;
		}
		if (guard_counter-- == 0)
			return -3;
	}
//	printk(DEBUG, "i2c_write 7\r\n");
	/* (7) End of tranfer, Data consistency are checking into Slave process *****/
	/* Generate Stop condition */
	LL_I2C_GenerateStopCondition(i2c_bus);
//	printk(DEBUG, "i2c_write 8\r\n");
	return 0;
}
