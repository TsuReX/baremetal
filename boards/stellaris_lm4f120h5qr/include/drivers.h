#ifndef __DRIVERS_H
#define __DRIVERS_H

#include <TIVA.h>
#include "hw_sysctl.h"
#include "hw_uart.h"
#include "hw_timer.h"
#include "hw_ints.h"

#define RCC_MOSCDIS				(0)
#define RCC_OSCSRC				(4)
#define RCC_XTAL				(6)
#define RCC_BYPASS				(11)
#define RCC_PWRDN				(13)
#define RCC_USESYSDIV			(22)
#define RCC_SYSDIV				(23)
#define RCC_ACG					(27)

#define RCC2_OSCSRC2			(4)
#define RCC2_BYPASS2			(11)
#define RCC2_PWRDN2				(13)
#define RCC2_USBPWRDN			(14)
#define RCC2_SYSDIV2LSB			(22)
#define RCC2_SYSDIV2			(23)
#define RCC2_DIV400				(30)
#define RCC2_USERCC2			(31)

#define RCGCGPIO_GPIOA			(0)
#define RCGCGPIO_GPIOB			(1)
#define RCGCGPIO_GPIOC			(2)
#define RCGCGPIO_GPIOD			(3)
#define RCGCGPIO_GPIOE			(4)
#define RCGCGPIO_GPIOF			(5)
#define RCGCGPIO_GPIOG			(6)
#define RCGCGPIO_GPIOH			(7)

#define RCGCUART_UART0			(0)
#define RCGCUART_UART1			(1)
#define RCGCUART_UART2			(2)
#define RCGCUART_UART3			(3)
#define RCGCUART_UART4			(4)
#define RCGCUART_UART5			(5)
#define RCGCUART_UART6			(6)
#define RCGCUART_UART7			(7)

#define RCGCTIMER_TIMER0		(0)

#define GPIO_PIN_0				(0)
#define GPIO_PIN_1				(1)
#define GPIO_PIN_2				(2)
#define GPIO_PIN_3				(3)
#define GPIO_PIN_4				(4)
#define GPIO_PIN_5				(5)
#define GPIO_PIN_6				(6)
#define GPIO_PIN_7				(7)

#define GPIOPCTL_F0				(0x0)
#define GPIOPCTL_F1				(0x1)
#define GPIOPCTL_F2				(0x2)
#define GPIOPCTL_F3				(0x3)
#define GPIOPCTL_F4				(0x4)
#define GPIOPCTL_F5				(0x5)
#define GPIOPCTL_F6				(0x6)
#define GPIOPCTL_F7				(0x7)
#define GPIOPCTL_F8				(0x8)
#define GPIOPCTL_F9				(0x9)
#define GPIOPCTL_F10			(0xA)
#define GPIOPCTL_F11			(0xB)
#define GPIOPCTL_F12			(0xC)
#define GPIOPCTL_F13			(0xD)
#define GPIOPCTL_F14			(0xE)
#define GPIOPCTL_F15			(0xF)


#endif /* __DRIVERS_H */

