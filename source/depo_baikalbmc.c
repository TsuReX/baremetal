#include "time.h"
#include "drivers.h"
#include "console.h"
#include "init.h"
#include "config.h"
#include "debug.h"
#include "usb_device.h"

void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

void HAL_Delay(uint32_t Delay) {
	mdelay(Delay);
}

int main(void)
{
	soc_init();

	board_init();

	mdelay(100);

	console_init();

	log_level_set(DEBUG);

	printk(DEBUG, "depo_baikalbmc\r\n");

//	MX_USB_DEVICE_Init();

	while(1) {
		printk(DEBUG, "cycle\r\n");
		LL_GPIO_SetOutputPin(GPIOA, LL_GPIO_PIN_6);
		LL_GPIO_ResetOutputPin(GPIOA, LL_GPIO_PIN_7);
		mdelay(500);
		LL_GPIO_ResetOutputPin(GPIOA, LL_GPIO_PIN_6);
		LL_GPIO_SetOutputPin(GPIOA, LL_GPIO_PIN_7);
		mdelay(500);
	}

	return 0;
}
