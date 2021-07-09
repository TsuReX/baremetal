#include <string.h>
#include "time.h"
#include "drivers.h"
#include "console.h"
#include "spi.h"
#include "init.h"
#include "config.h"
#include "debug.h"
#include "spi_flash.h"


int32_t power_on(void)
{
	LL_GPIO_SetOutputPin(DRVRST_N_PORT, DRVRST_N_PIN);
	mdelay(10);

	LL_GPIO_SetOutputPin(EN_VDD_PORT, EN_VDD_PIN);
	mdelay(1);
	uint32_t pg = LL_GPIO_ReadInputPort(PG_VDD_PORT) & PG_VDD_PIN;
	if (!(pg & PG_VDD_PIN))
		return -1;
	mdelay(10);

	LL_GPIO_SetOutputPin(EN_VL_PORT, EN_VL_PIN);
	mdelay(1);
	pg = LL_GPIO_ReadInputPort(PG_VL_PORT) & PG_VL_PIN;
	if (!(pg & PG_VL_PIN))
		return -2;
	mdelay(10);

	LL_GPIO_SetOutputPin(EN_VCORE_PORT, EN_VCORE_PIN);
	mdelay(1);
	pg = LL_GPIO_ReadInputPort(PG_VCORE_PORT) & PG_VCORE_PIN;
	if (!(pg & PG_VCORE_PIN))
		return -3;
	mdelay(10);

	LL_GPIO_SetOutputPin(EN_VLL_PORT, EN_VLL_PIN);
	mdelay(1);
	pg = LL_GPIO_ReadInputPort(PG_VLL_PORT) & PG_VLL_PIN;
	if (!(pg & PG_VLL_PIN))
		return -4;
	mdelay(10);

	LL_GPIO_SetOutputPin(EN_VDRAM_PORT, EN_VDRAM_PIN);
	mdelay(1);
	pg = LL_GPIO_ReadInputPort(PG_VDRAM_PORT) & PG_VDRAM_PIN;
	if (!(pg & PG_VDRAM_PIN))
		return -5;
	mdelay(10);

	LL_GPIO_SetOutputPin(EN_AUX_PORT, EN_AUX_PIN);
	mdelay(1);
	pg = LL_GPIO_ReadInputPort(PG_AUX_PORT) & PG_AUX_PIN;
	if (!(pg & PG_AUX_PIN))
		return -6;
	mdelay(10);

	LL_GPIO_SetOutputPin(EN_VCC_PORT, EN_VCC_PIN);
	mdelay(1);
	pg = LL_GPIO_ReadInputPort(PG_VCC_PORT) & PG_VCC_PIN;
	if (!(pg & PG_VCC_PIN))
		return -7;
	mdelay(10);

	LL_GPIO_ResetOutputPin(DRVRST_N_PORT, DRVRST_N_PIN);

	return 0;
}

void power_off(void)
{
	LL_GPIO_SetOutputPin(DRVRST_N_PORT, DRVRST_N_PIN);

	LL_GPIO_ResetOutputPin(EN_VCC_PORT, EN_VCC_PIN);
	mdelay(1);

	LL_GPIO_ResetOutputPin(EN_AUX_PORT, EN_AUX_PIN);
	mdelay(1);

	LL_GPIO_ResetOutputPin(EN_VDRAM_PORT, EN_VDRAM_PIN);
	mdelay(1);

	LL_GPIO_ResetOutputPin(EN_VLL_PORT, EN_VLL_PIN);
	mdelay(1);

	LL_GPIO_ResetOutputPin(EN_VCORE_PORT, EN_VCORE_PIN);
	mdelay(1);

	LL_GPIO_ResetOutputPin(EN_VL_PORT, EN_VL_PIN);
	mdelay(1);

	LL_GPIO_ResetOutputPin(EN_VDD_PORT, EN_VDD_PIN);
	mdelay(1);
}

int main(void)
{
	soc_init();

	board_init();

	console_init();

	log_level_set(DEBUG);

	int32_t ret_val = power_on();

	if (ret_val != 0 )
		power_off();

	__WFE();

	while(1);

	return 0;
}
