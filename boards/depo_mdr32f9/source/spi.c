/*
 * spi.c
 *
 *  Created on: Oct 2, 2020
 *      Author: user
 */

#include <stddef.h>

#include "spi.h"
#include "drivers.h"

void spi_init(void)
{
	PORT_InitTypeDef port_descriptor;

	RST_CLK_PCLKcmd(RST_CLK_PCLK_PORTF, ENABLE);

	/*SPI GPIO*/
	port_descriptor.PORT_Pin   = (PORT_Pin_3);
	port_descriptor.PORT_OE    = PORT_OE_IN;
	port_descriptor.PORT_FUNC  = PORT_FUNC_ALTER;
	port_descriptor.PORT_MODE  = PORT_MODE_DIGITAL;
	port_descriptor.PORT_SPEED = PORT_SPEED_FAST;

	PORT_Init(MDR_PORTF, &port_descriptor);

	port_descriptor.PORT_Pin   = (PORT_Pin_0 | PORT_Pin_1);
	port_descriptor.PORT_OE    = PORT_OE_OUT;

	PORT_Init(MDR_PORTF, &port_descriptor);

	/*SPI*/
	RST_CLK_PCLKcmd((RST_CLK_PCLK_RST_CLK | RST_CLK_PCLK_SSP1), ENABLE);

	SSP_DeInit(MDR_SSP1);

	uint32_t temp= MDR_RST_CLK->SSP_CLOCK;

	temp |= RST_CLK_SSP_CLOCK_SSP1_CLK_EN;
	temp &= ~RST_CLK_SSP_CLOCK_SSP1_BRG_Msk;
	temp |= SSP_HCLKdiv1;

	MDR_RST_CLK->SSP_CLOCK = temp;

	MDR_SSP1->CPSR = 0x04;

	MDR_SSP1->CR0 = (0x00 << SSP_CR0_SCR_Pos)
					| SSP_SPH_1Edge
					| SSP_SPO_Low
					| SSP_FRF_SPI_Motorola
					| SSP_WordLength8b;

	MDR_SSP1->CR1 = SSP_HardwareFlowControl_None | SSP_ModeMaster;

	MDR_SSP1->CR1 |= 0x2;
}

uint32_t spi_data_xfer(uint8_t *src_buf, uint8_t *dst_buf, size_t data_size)
{
	size_t byte_idx = 0;
	if (src_buf == NULL && dst_buf == NULL)
		return 0;

	if (src_buf == NULL) {
		for (; byte_idx < data_size; ++byte_idx) {
			size_t i = 1000;

			while (SSP_GetFlagStatus(MDR_SSP1, SSP_FLAG_TFE) == RESET && i-- != 0);
			if (i == 0)
				return byte_idx;

			SSP_SendData(MDR_SSP1, 0x0);

			i = 1000;
			while (SSP_GetFlagStatus(MDR_SSP1, SSP_FLAG_RNE) == RESET && i-- != 0);

			if (i == 0)
				return byte_idx;

			dst_buf[byte_idx] = SSP_ReceiveData(MDR_SSP1);
		}
	} else if (dst_buf == NULL) {

		for (; byte_idx < data_size; ++byte_idx) {
			size_t i = 1000;

			while (SSP_GetFlagStatus(MDR_SSP1, SSP_FLAG_TFE) == RESET && i-- != 0);
			if (i == 0)
				return byte_idx;

			SSP_SendData(MDR_SSP1, src_buf[byte_idx]);

			i = 1000;
			while (SSP_GetFlagStatus(MDR_SSP1, SSP_FLAG_RNE) == RESET && i-- != 0);

			if (i == 0)
				return byte_idx;

			SSP_ReceiveData(MDR_SSP1);
		}
	} else {


		for (; byte_idx < data_size; ++byte_idx) {
			size_t i = 1000;

			while (SSP_GetFlagStatus(MDR_SSP1, SSP_FLAG_TFE) == RESET && i-- != 0);
			if (i == 0)
				return byte_idx;

			SSP_SendData(MDR_SSP1, src_buf[byte_idx]);

			i = 1000;
			while (SSP_GetFlagStatus(MDR_SSP1, SSP_FLAG_RNE) == RESET && i-- != 0);

			if (i == 0)
				return byte_idx;

			dst_buf[byte_idx] = SSP_ReceiveData(MDR_SSP1);
		}
	}
	return data_size;
}
