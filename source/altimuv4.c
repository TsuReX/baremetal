#include "altimuv4.h"
#include "debug.h"

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
