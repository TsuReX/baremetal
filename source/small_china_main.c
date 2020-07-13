/**
 * @file	src/main.c
 *
 * @brief	Объявление точки вхождения в C-код.
 *
 * @author	Vasily Yurchenko <vasily.v.yurchenko@yandex.ru>
 */

#include "drivers.h"
#include "config.h"
#include "console.h"
#include "scheduler.h"
/*#include "i2c.h"
#include "ds3231m.h"
#include "ina3221.h"
#include "gpio.h"
*/
#include <usb_init.h>
#include <usb_istr.h>
//#include "../boards/china_stm32f103c8t6/include/_hw_config.h"
#include "usb_lib.h"
#include "usb_desc.h"
#include "usb_pwr.h"

/**
 * @brief	C-code entry point.
 *
 * @retval	0 in case of success, otherwise error code
 */
int main(void)
{
	/** Configure internal subsystems of SoC. */
	soc_init();

	/** Configure board's peripherals. */
	board_init();

	console_init();

	scheduler_init();

	/*i2c_init();*/

	/*gpio_setup_it();*/

//	Set_USBClock();
//	USB_Interrupts_Config();

	NVIC_SetPriority(USB_LP_CAN1_RX0_IRQn, 0);
	NVIC_EnableIRQ(USB_LP_CAN1_RX0_IRQn);

	NVIC_SetPriority(USBWakeUp_IRQn, 0);
	NVIC_EnableIRQ(USBWakeUp_IRQn);

	USB_Init();

	uint32_t i = 0;
	while (1) {
		LL_mDelay(DELAY_500_MS * 2);
		print("Main thread iteration %ld\r\n", i++);
		/*ina3221_print_voltage_current();*/
//		LL_GPIO_TogglePin(GPIOA, LL_GPIO_PIN_0);
//		LL_GPIO_TogglePin(GPIOC, LL_GPIO_PIN_13);
//		LL_GPIO_TogglePin(GPIOC, LL_GPIO_PIN_14);
//		LL_GPIO_TogglePin(GPIOC, LL_GPIO_PIN_15);
	}
}


void USB_LP_CAN1_RX0_IRQHandler(void)
{
	USB_Istr();
}


void EVAL_COM1_IRQHandler(void)
{
//  if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
//  {
//    /* Send the received data to the PC Host*/
//    USART_To_USB_Send_Data();
//  }
//
//  /* If overrun condition occurs, clear the ORE flag and recover communication */
//  if (USART_GetFlagStatus(USART1, USART_FLAG_ORE) != RESET)
//  {
//    (void)USART_ReceiveData(USART1);
//  }
}

void USBWakeUp_IRQHandler(void)
{
//	EXTI_ClearITPendingBit(EXTI_Line18);
	/* TODO: USB implement pending flag clearing */
}
