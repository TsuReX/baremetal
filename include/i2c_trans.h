/*
 * i2c_trans.h
 *
 *  Created on: Sep 20, 2021
 *      Author: user
 */

#ifndef INCLUDE_I2C_TRANS_H_
#define INCLUDE_I2C_TRANS_H_

#include "stddef.h"
#include "drivers.h"

int32_t i2c_write(I2C_TypeDef *i2c_bus, uint8_t chip_addr, uint8_t reg_addr, uint8_t *buffer, size_t buffer_size);
int32_t i2c_read(I2C_TypeDef *i2c_bus, uint8_t chip_addr, uint8_t reg_addr, uint8_t *buffer, size_t buffer_size);

#endif /* INCLUDE_I2C_TRANS_H_ */
