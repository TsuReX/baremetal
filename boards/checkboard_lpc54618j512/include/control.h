/*
 * control.h
 *
 *  Created on: Apr 7, 2020
 *      Author: vasily
 */

#ifndef BOARDS_CHECKBOARD_LPC54618J512_INCLUDE_CONTROL_H_
#define BOARDS_CHECKBOARD_LPC54618J512_INCLUDE_CONTROL_H_

#include <fsl_gpio.h>

static inline void power_on_hl1(void)
{
    GPIO_PinWrite(GPIO, 5, 0, 1);
}

static inline void power_off_hl1(void)
{
    GPIO_PinWrite(GPIO, 5, 0, 0);
}

static inline void power_on_hl2(void)
{
    GPIO_PinWrite(GPIO, 5, 2, 1);
}

static inline void power_off_hl2(void)
{
    GPIO_PinWrite(GPIO, 5, 2, 0);
}

static inline void power_on_bp(void)
{
    GPIO_PinWrite(GPIO, 3, 0, 0);
}

static inline void power_off_bp(void)
{
    GPIO_PinWrite(GPIO, 3, 0, 1);
}

static inline void power_on_spl(void)
{
    GPIO_PinWrite(GPIO, 3, 2, 0);
}

static inline void power_off_spl(void)
{
    GPIO_PinWrite(GPIO, 3, 2, 1);
}

#endif /* BOARDS_CHECKBOARD_LPC54618J512_INCLUDE_CONTROL_H_ */
