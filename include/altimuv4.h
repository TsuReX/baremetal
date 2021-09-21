/*
 * altimuv4.h
 *
 *  Created on: Sep 21, 2021
 *      Author: user
 */

#ifndef INCLUDE_ALTIMUV4_H_
#define INCLUDE_ALTIMUV4_H_

/*
 * AltIMU-10
 * https://www.pololu.com/product/1269
 */
#define GYRO_ADDR 0x6B /* Gyro (L3GD20) */
#define ACCL_ADDR 0x19 /* Accelerometer (LSM303DLHC) */
#define MGNT_ADDR 0x1E /* Magnetometer (LSM303DLHC) */
#define BMTR_ADDR 0x5D /* Barometer (LPS331AP) */

void gyro_init();
void gyro_read();
void accel_init();
void magnet_init();
void accel_read();
void magnet_read();

#endif /* INCLUDE_ALTIMUV4_H_ */
