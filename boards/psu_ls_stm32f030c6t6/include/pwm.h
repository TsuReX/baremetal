/*
 * pwm.h
 *
 *  Created on: Mar 16, 2020
 *      Author: user
 */

#ifndef BOARDS_NUCLEO_STM32F303K8T6_INCLUDE_PWM_H_
#define BOARDS_NUCLEO_STM32F303K8T6_INCLUDE_PWM_H_

#include "stdint.h"

void pwm_init(void);
void pwm_close(void);
void pwd_duty(uint32_t duty);

#endif /* BOARDS_NUCLEO_STM32F303K8T6_INCLUDE_PWM_H_ */
