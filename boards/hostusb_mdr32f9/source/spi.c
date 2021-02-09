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

	RST_CLK_PCLKcmd(RST_CLK_PCLK_PORTF, ENABLE);

	PORT_InitTypeDef port_descriptor;
	port_descriptor.PORT_FUNC  = PORT_FUNC_ALTER;
	port_descriptor.PORT_MODE  = PORT_MODE_DIGITAL;
	port_descriptor.PORT_SPEED = PORT_SPEED_FAST;

	/*SPI MISO*/
	port_descriptor.PORT_Pin   = (PORT_Pin_3);
	port_descriptor.PORT_OE    = PORT_OE_IN;

	PORT_Init(MDR_PORTF, &port_descriptor);

	/*SPI MOSI CLK*/
	port_descriptor.PORT_Pin   = (PORT_Pin_0 | PORT_Pin_1);
	port_descriptor.PORT_OE    = PORT_OE_OUT;

	PORT_Init(MDR_PORTF, &port_descriptor);

	/*SPI*/
	RST_CLK_PCLKcmd((RST_CLK_PCLK_RST_CLK | RST_CLK_PCLK_SSP1), ENABLE);

	SSP_DeInit(MDR_SSP1);

#if 0
//	uint32_t temp= MDR_RST_CLK->SSP_CLOCK;
//
//	temp |= RST_CLK_SSP_CLOCK_SSP1_CLK_EN;
//	temp &= ~RST_CLK_SSP_CLOCK_SSP1_BRG_Msk;
//	temp |= SSP_HCLKdiv1;
//
//	MDR_RST_CLK->SSP_CLOCK = temp;
#else
	SSP_BRGInit(MDR_SSP1, SSP_HCLKdiv2);
#endif

#if 0

	MDR_SSP1->CPSR = 0x04;

	MDR_SSP1->CR0 = (0x00 << SSP_CR0_SCR_Pos)
					| SSP_SPH_1Edge
					| SSP_SPO_Low
					| SSP_FRF_SPI_Motorola
					| SSP_WordLength8b;

	MDR_SSP1->CR1 = SSP_HardwareFlowControl_None | SSP_ModeMaster;

	MDR_SSP1->CR1 |= 0x2;

#else

	SSP_InitTypeDef sSSP;
	SSP_StructInit (&sSSP);

	sSSP.SSP_CPSDVSR = 0x04;
	sSSP.SSP_SCR  = 0x00;
	sSSP.SSP_Mode = SSP_ModeMaster;
	sSSP.SSP_WordLength = SSP_WordLength8b;
	sSSP.SSP_SPH = SSP_SPH_1Edge;
	sSSP.SSP_SPO = SSP_SPO_Low;
	sSSP.SSP_FRF = SSP_FRF_SPI_Motorola;
	sSSP.SSP_HardwareFlowControl = SSP_HardwareFlowControl_None;

	SSP_Init (MDR_SSP1, &sSSP);

	SSP_Cmd(MDR_SSP1, ENABLE);

#endif
}

uint32_t spi_data_xfer(const uint8_t *src_buf, uint8_t *dst_buf, size_t data_size)
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
