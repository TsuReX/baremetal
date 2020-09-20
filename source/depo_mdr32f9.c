#include "drivers.h"
#include "console.h"

#define SPI_USB_REGNUMOFF		0x03
#define SPI_USB_WROP			0x02
#define SPI_USB_RDOP			0x00

#define SPI_USB_RCVFIFOREG		0x01
#define SPI_USB_SNDFIFOREG		0x02
#define SPI_USB_SUDFIFOREG		0x04
#define SPI_USB_RCVBCREG		0x06
#define SPI_USB_SNDBCREG		0x07

#define SPI_USB_USBIRQREG		0x0D
#define SPI_USB_USBCTLREG		0x0F
#define SPI_USB_PINCTLREG		0x11
#define SPI_USB_REVREG			0x12
#define SPI_USB_IOPINS1REG		0x14
#define SPI_USB_IOPINS2REG		0x15
#define SPI_USB_HIRQREG			0x19
#define SPI_USB_MODEREG			0x1B
#define SPI_USB_PERADDRREG		0x1C
#define SPI_USB_HCTLREG			0x1D
#define SPI_USB_HXFRREG			0x1E
#define SPI_USB_HRSLREG			0x1F

//#define SPI_USB_HIENREG			0x1A

#define SPI_CMD_RDRCVFIFO	((SPI_USB_RCVFIFOREG << SPI_USB_REGNUMOFF) | SPI_USB_RDOP)
#define SPI_CMD_WRRCVFIFO	((SPI_USB_RCVFIFOREG << SPI_USB_REGNUMOFF) | SPI_USB_WROP)

#define SPI_CMD_RDSNDFIFO	((SPI_USB_SNDFIFOREG << SPI_USB_REGNUMOFF) | SPI_USB_RDOP)
#define SPI_CMD_WRSNDFIFO	((SPI_USB_SNDFIFOREG << SPI_USB_REGNUMOFF) | SPI_USB_WROP)

#define SPI_CMD_RDSUDFIFO	((SPI_USB_SUDFIFOREG << SPI_USB_REGNUMOFF) | SPI_USB_RDOP)
#define SPI_CMD_WRSUDFIFO	((SPI_USB_SUDFIFOREG << SPI_USB_REGNUMOFF) | SPI_USB_WROP)

#define SPI_CMD_RDRCVBC		((SPI_USB_RCVBCREG << SPI_USB_REGNUMOFF) | SPI_USB_RDOP)
#define SPI_CMD_WRRCVBC		((SPI_USB_RCVBCREG << SPI_USB_REGNUMOFF) | SPI_USB_WROP)

#define SPI_CMD_RDSNDBC		((SPI_USB_SNDBCREG << SPI_USB_REGNUMOFF) | SPI_USB_RDOP)
#define SPI_CMD_WRSNDBC		((SPI_USB_SNDBCREG << SPI_USB_REGNUMOFF) | SPI_USB_WROP)

#define SPI_CMD_RDREV		((SPI_USB_REVREG << SPI_USB_REGNUMOFF) | SPI_USB_RDOP)

#define SPI_CMD_WRPINCTL	((SPI_USB_PINCTLREG << SPI_USB_REGNUMOFF) | SPI_USB_WROP)
#define SPI_CMD_RDPINCTL	((SPI_USB_PINCTLREG << SPI_USB_REGNUMOFF) | SPI_USB_RDOP)

#define SPI_CMD_WRUSBIRQ	((SPI_USB_USBIRQREG << SPI_USB_REGNUMOFF) | SPI_USB_WROP)
#define SPI_CMD_RDUSBIRQ	((SPI_USB_USBIRQREG << SPI_USB_REGNUMOFF) | SPI_USB_RDOP)

#define SPI_CMD_WRUSBCTL	((SPI_USB_USBCTLREG << SPI_USB_REGNUMOFF) | SPI_USB_WROP)
#define SPI_CMD_RDUSBCTL	((SPI_USB_USBCTLREG << SPI_USB_REGNUMOFF) | SPI_USB_RDOP)

#define SPI_CMD_WRIOPINS1	((SPI_USB_IOPINS1REG << SPI_USB_REGNUMOFF) | SPI_USB_WROP)
#define SPI_CMD_RDIOPINS1	((SPI_USB_IOPINS1REG << SPI_USB_REGNUMOFF) | SPI_USB_RDOP)

#define SPI_CMD_WRIOPINS2	((SPI_USB_IOPINS2REG << SPI_USB_REGNUMOFF) | SPI_USB_WROP)
#define SPI_CMD_RDIOPINS2	((SPI_USB_IOPINS2REG << SPI_USB_REGNUMOFF) | SPI_USB_RDOP)

#define SPI_CMD_WRHIRQ	((SPI_USB_HIRQREG << SPI_USB_REGNUMOFF) | SPI_USB_WROP)
#define SPI_CMD_RDHIRQ	((SPI_USB_HIRQREG << SPI_USB_REGNUMOFF) | SPI_USB_RDOP)

#define SPI_CMD_WRMODE	((SPI_USB_MODEREG << SPI_USB_REGNUMOFF) | SPI_USB_WROP)
#define SPI_CMD_RDMODE	((SPI_USB_MODEREG << SPI_USB_REGNUMOFF) | SPI_USB_RDOP)

#define SPI_CMD_WRPERADDR	((SPI_USB_PERADDRREG << SPI_USB_REGNUMOFF) | SPI_USB_WROP)
#define SPI_CMD_RDPERADDR	((SPI_USB_PERADDRREG << SPI_USB_REGNUMOFF) | SPI_USB_RDOP)

#define SPI_CMD_WRHCTL	((SPI_USB_HCTLREG << SPI_USB_REGNUMOFF) | SPI_USB_WROP)
#define SPI_CMD_RDHCTL	((SPI_USB_HCTLREG << SPI_USB_REGNUMOFF) | SPI_USB_RDOP)

#define SPI_CMD_WRHXFR	((SPI_USB_HXFRREG << SPI_USB_REGNUMOFF) | SPI_USB_WROP)
#define SPI_CMD_RDHXFR	((SPI_USB_HXFRREG << SPI_USB_REGNUMOFF) | SPI_USB_RDOP)

#define SPI_CMD_RDHRSL	((SPI_USB_HRSLREG << SPI_USB_REGNUMOFF) | SPI_USB_RDOP)

#define BufferSize         32

#define ALL_PORTS_CLK (RST_CLK_PCLK_PORTA | RST_CLK_PCLK_PORTB | \
                       RST_CLK_PCLK_PORTC | RST_CLK_PCLK_PORTD | \
                       RST_CLK_PCLK_PORTE | RST_CLK_PCLK_PORTF)

/* Max delay can be used in LL_mDelay */
#define LL_MAX_DELAY                  0xFFFFFFFFU
#define CR1_EN_Set	((uint16_t)0x0002)  /*!< SSP Enable Mask */
#define HCLKFrequency 8000000

__STATIC_INLINE void LL_InitTick(uint32_t HCLKFreq, uint32_t Ticks)
{
  /* Configure the SysTick to have interrupt in 1ms time base */
  SysTick->LOAD  = (uint32_t)((HCLKFreq / Ticks) - 1UL);  /* set reload register */
  SysTick->VAL   = 0UL;                                       /* Load the SysTick Counter Value */
  SysTick->CTRL  = SysTick_CTRL_CLKSOURCE_Msk |
                   SysTick_CTRL_ENABLE_Msk;                   /* Enable the Systick Timer */
}

void mdelay(uint32_t delay)
{
	delay *= 10;
  __IO uint32_t  tmp = SysTick->CTRL;  /* Clear the COUNTFLAG first */
  /* Add this code to indicate that local variable is not used */
  ((void)tmp);

  /* Add a period to guaranty minimum wait */
  if (delay < LL_MAX_DELAY) {
    delay++;
  }

  while (delay) {
    if ((SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk) != 0U) {
      delay--;
    }
  }
}

void u100delay(uint32_t delay)
{
  __IO uint32_t  tmp = SysTick->CTRL;  /* Clear the COUNTFLAG first */
  /* Add this code to indicate that local variable is not used */
  ((void)tmp);

  /* Add a period to guaranty minimum wait */
  if (delay < LL_MAX_DELAY) {
    delay++;
  }

  while (delay) {
    if ((SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk) != 0U) {
      delay--;
    }
  }
}

void spi_init()
{
	RST_CLK_PCLKcmd((RST_CLK_PCLK_RST_CLK | RST_CLK_PCLK_SSP1), ENABLE);

	SSP_DeInit(MDR_SSP1);

	uint32_t temp= MDR_RST_CLK->SSP_CLOCK;

	temp |= RST_CLK_SSP_CLOCK_SSP1_CLK_EN;
	temp &= ~RST_CLK_SSP_CLOCK_SSP1_BRG_Msk;
	temp |= SSP_HCLKdiv1;

	MDR_RST_CLK->SSP_CLOCK = temp;

	MDR_SSP1->CPSR = 0x08;

	MDR_SSP1->CR0 = (0x00 << SSP_CR0_SCR_Pos)
					| SSP_SPH_1Edge
					| SSP_SPO_Low
					| SSP_FRF_SPI_Motorola
					| SSP_WordLength8b;

	MDR_SSP1->CR1 = SSP_HardwareFlowControl_None | SSP_ModeMaster;

	MDR_SSP1->CR1 |= CR1_EN_Set;
}

static uint32_t spi_data_xfer(uint8_t *src_buf, uint8_t *dst_buf, size_t data_size)
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

			SSP_SendData(MDR_SSP1, 0x0);

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

void gpio_init(void)
{
//	/*General initialization*/
//	RST_CLK_PCLKcmd(ALL_PORTS_CLK, ENABLE);
//
	PORT_InitTypeDef port_descriptor;
	PORT_StructInit(&port_descriptor);
//
//	PORT_Init(MDR_PORTA, &port_descriptor);
//	PORT_Init(MDR_PORTB, &port_descriptor);
//	PORT_Init(MDR_PORTC, &port_descriptor);
//	PORT_Init(MDR_PORTD, &port_descriptor);
//	PORT_Init(MDR_PORTE, &port_descriptor);
//	PORT_Init(MDR_PORTF, &port_descriptor);
//
//	RST_CLK_PCLKcmd(ALL_PORTS_CLK, DISABLE);
	RST_CLK_PCLKcmd(RST_CLK_PCLK_PORTA, ENABLE);
	RST_CLK_PCLKcmd(RST_CLK_PCLK_PORTB, ENABLE);
	RST_CLK_PCLKcmd(RST_CLK_PCLK_PORTD, ENABLE);
	RST_CLK_PCLKcmd(RST_CLK_PCLK_PORTE, ENABLE);
	RST_CLK_PCLKcmd(RST_CLK_PCLK_PORTF, ENABLE);

	/****************************************************/
	/*Control*/
	port_descriptor.PORT_Pin   = (PORT_Pin_7);
	port_descriptor.PORT_OE    = PORT_OE_OUT;
	port_descriptor.PORT_FUNC  = PORT_FUNC_PORT;
	port_descriptor.PORT_MODE  = PORT_MODE_DIGITAL;
	port_descriptor.PORT_SPEED = PORT_SPEED_SLOW;

	/*PA7 PPS_ON*/
	PORT_Init(MDR_PORTA, &port_descriptor);
	/*PD7 DEBUG_LED*/
	PORT_Init(MDR_PORTD, &port_descriptor);

	port_descriptor.PORT_Pin   = (PORT_Pin_0);
	/*PE0 KM.KB_CS*/
	PORT_Init(MDR_PORTE, &port_descriptor);

//	port_descriptor.PORT_Pin   = (PORT_Pin_8);
//	/*PB8 KM.MS_CS*/
//	PORT_Init(MDR_PORTB, &port_descriptor);

	PORT_SetBits(MDR_PORTA, PORT_Pin_7);

//	PORT_SetBits(MDR_PORTB, PORT_Pin_8);

	/****************************************************/
	/*SPI*/

	port_descriptor.PORT_Pin   = (PORT_Pin_3);
	port_descriptor.PORT_OE    = PORT_OE_IN;
	port_descriptor.PORT_FUNC  = PORT_FUNC_ALTER;
	port_descriptor.PORT_MODE  = PORT_MODE_DIGITAL;
	port_descriptor.PORT_SPEED = PORT_SPEED_FAST;

	PORT_Init(MDR_PORTF, &port_descriptor);

	port_descriptor.PORT_Pin   = (PORT_Pin_0 | PORT_Pin_1);
	port_descriptor.PORT_OE    = PORT_OE_OUT;

	PORT_Init(MDR_PORTF, &port_descriptor);
//
//	port_descriptor.PORT_Pin   = (PORT_Pin_2);
//	port_descriptor.PORT_OE    = PORT_OE_OUT;
//	port_descriptor.PORT_FUNC  = PORT_FUNC_PORT;
//	port_descriptor.PORT_MODE  = PORT_MODE_DIGITAL;
//	port_descriptor.PORT_SPEED = PORT_SPEED_SLOW;
//
//	PORT_Init(MDR_PORTF, &port_descriptor);
//
//	/****************************************************/
	/*USART 1,2*/

	port_descriptor.PORT_PULL_UP = PORT_PULL_UP_OFF;
	port_descriptor.PORT_PULL_DOWN = PORT_PULL_DOWN_OFF;
	port_descriptor.PORT_PD_SHM = PORT_PD_SHM_OFF;
	port_descriptor.PORT_PD = PORT_PD_DRIVER;
	port_descriptor.PORT_GFEN = PORT_GFEN_OFF;
	port_descriptor.PORT_FUNC = PORT_FUNC_ALTER;
	port_descriptor.PORT_SPEED = PORT_SPEED_MAXFAST;
	port_descriptor.PORT_MODE = PORT_MODE_DIGITAL;

	/* Configure PORTB pins 5 (UART1_TX) as output */
	port_descriptor.PORT_OE = PORT_OE_OUT;
	port_descriptor.PORT_Pin = PORT_Pin_5;
	PORT_Init(MDR_PORTB, &port_descriptor);

	/* Configure PORTB pins 6 (UART1_RX) as input */
	port_descriptor.PORT_OE = PORT_OE_IN;
	port_descriptor.PORT_Pin = PORT_Pin_6;
	PORT_Init(MDR_PORTB, &port_descriptor);
//
//	/* Configure PORTD pins 1 (UART2_TX) as output */
//	port_descriptor.PORT_OE = PORT_OE_OUT;
//	port_descriptor.PORT_Pin = PORT_Pin_1;
//	PORT_Init(MDR_PORTD, &port_descriptor);
//
//	/* Configure PORTD pins 0 (UART2_RX) as input */
//	port_descriptor.PORT_OE = PORT_OE_IN;
//	port_descriptor.PORT_Pin = PORT_Pin_0;
//	PORT_Init(MDR_PORTD, &port_descriptor);
}

void clock_init(void)
{
	//	RST_CLK_CPU_PLLconfig (RST_CLK_CPU_PLLsrcHSIdiv1, RST_CLK_CPU_PLLmul8);

//	  /* Select CPUPLL source */
//	uint32_t temp = MDR_RST_CLK->CPU_CLOCK;
//	/* Clear CPU_C1_SEL bits */
//	temp &= ~RST_CLK_CPU_CLOCK_CPU_C1_SEL_Msk;
//	/* Set the CPU_C1_SEL bits */
//	temp |= RST_CLK_CPU_PLLsrcHSIdiv1;
//	/* Store the new value */
//	MDR_RST_CLK->CPU_CLOCK = temp;
//
//	/* Set CPUPLL multiplier */
//	temp = MDR_RST_CLK->PLL_CONTROL;
//	/* Clear PLLMUL[3:0] bits */
//	temp &= ~RST_CLK_PLL_CONTROL_PLL_CPU_MUL_Msk;
//	/* Set the PLLMUL[3:0] bits */
//	temp |= (RST_CLK_CPU_PLLmul8 << RST_CLK_PLL_CONTROL_PLL_CPU_MUL_Pos);
//	/* Store the new value */
//	MDR_RST_CLK->PLL_CONTROL = temp;
//
//	if ((MDR_RST_CLK->PLL_CONTROL & RST_CLK_PLL_CONTROL_PLL_CPU_ON) == RST_CLK_PLL_CONTROL_PLL_CPU_ON ) {
//		temp = MDR_RST_CLK->PLL_CONTROL;
//		temp |= RST_CLK_PLL_CONTROL_PLL_CPU_PLD;
//		MDR_RST_CLK->PLL_CONTROL = temp;
//		temp &= ~RST_CLK_PLL_CONTROL_PLL_CPU_PLD;
//		MDR_RST_CLK->PLL_CONTROL = temp;
//	}

	LL_InitTick(HCLKFrequency, 10000U);
}

uint8_t usbirq_read(void)
{
	uint8_t	buffer[2] = {SPI_CMD_RDUSBIRQ, 0x00};
	spi_data_xfer(buffer, buffer, sizeof(buffer) / sizeof(buffer[0]));
	return buffer[1];
}

void usbirq_write(uint8_t usbirq)
{
	uint8_t	buffer[2] = {SPI_CMD_WRUSBIRQ, usbirq};
	spi_data_xfer(buffer, buffer, sizeof(buffer) / sizeof(buffer[0]));
}

uint8_t usbctl_read(void)
{
	uint8_t	buffer[2] = {SPI_CMD_RDUSBCTL, 0x00};
	spi_data_xfer(buffer, buffer, sizeof(buffer) / sizeof(buffer[0]));
	return buffer[1];
}

void usbctl_write(uint8_t usbctl)
{
	uint8_t	buffer[2] = {SPI_CMD_WRUSBCTL, usbctl};
	spi_data_xfer(buffer, buffer, sizeof(buffer) / sizeof(buffer[0]));
}

uint8_t pinctl_read(void)
{
	uint8_t	buffer[2] = {SPI_CMD_RDPINCTL, 0x00};
	spi_data_xfer(buffer, buffer, sizeof(buffer) / sizeof(buffer[0]));
	return buffer[1];
}

void pinctl_write(uint8_t pinctl)
{
	uint8_t	buffer[2] = {SPI_CMD_WRPINCTL, pinctl};
	spi_data_xfer(buffer, buffer, sizeof(buffer) / sizeof(buffer[0]));
}

uint8_t iopins1_read(void)
{
	uint8_t	buffer[2] = {SPI_CMD_RDIOPINS1, 0x00};
	spi_data_xfer(buffer, buffer, sizeof(buffer) / sizeof(buffer[0]));
	return buffer[1];
}

void iopins1_write(uint8_t iopins1)
{
	uint8_t	buffer[2] = {SPI_CMD_WRIOPINS1, iopins1};
	spi_data_xfer(buffer, buffer, sizeof(buffer) / sizeof(buffer[0]));
}

uint8_t iopins2_read(void)
{
	uint8_t	buffer[2] = {SPI_CMD_RDIOPINS2, 0x00};
	spi_data_xfer(buffer, buffer, sizeof(buffer) / sizeof(buffer[0]));
	return buffer[1];
}

void iopins2_write(uint8_t iopins1)
{
	uint8_t	buffer[2] = {SPI_CMD_WRIOPINS2, iopins1};
	spi_data_xfer(buffer, buffer, sizeof(buffer) / sizeof(buffer[0]));
}

uint8_t revision_read(void)
{
	uint8_t	buffer[2] = {SPI_CMD_RDREV, 0x00};
	spi_data_xfer(buffer, buffer, sizeof(buffer) / sizeof(buffer[0]));
	return buffer[1];
}

uint8_t hirq_read(void)
{
	uint8_t	buffer[2] = {SPI_CMD_RDHIRQ, 0x00};
	spi_data_xfer(buffer, buffer, sizeof(buffer) / sizeof(buffer[0]));
	return buffer[1];
}

void hirq_write(uint8_t hirq)
{
	uint8_t	buffer[2] = {SPI_CMD_WRHIRQ, hirq};
	spi_data_xfer(buffer, buffer, sizeof(buffer) / sizeof(buffer[0]));
}

uint8_t mode_read(void)
{
	uint8_t	buffer[2] = {SPI_CMD_RDMODE, 0x00};
	spi_data_xfer(buffer, buffer, sizeof(buffer) / sizeof(buffer[0]));
	return buffer[1];
}

void mode_write(uint8_t mode)
{
	uint8_t	buffer[2] = {SPI_CMD_WRMODE, mode};
	spi_data_xfer(buffer, buffer, sizeof(buffer) / sizeof(buffer[0]));
}

uint8_t peraddr_read(void)
{
	uint8_t	buffer[2] = {SPI_CMD_RDPERADDR, 0x00};
	spi_data_xfer(buffer, buffer, sizeof(buffer) / sizeof(buffer[0]));
	return buffer[1];
}

void peraddr_write(uint8_t peraddr)
{
	uint8_t	buffer[2] = {SPI_CMD_WRPERADDR, peraddr};
	spi_data_xfer(buffer, buffer, sizeof(buffer) / sizeof(buffer[0]));
}

uint8_t hctl_read(void)
{
	uint8_t	buffer[2] = {SPI_CMD_RDHCTL, 0x00};
	spi_data_xfer(buffer, buffer, sizeof(buffer) / sizeof(buffer[0]));
	return buffer[1];
}

void hctl_write(uint8_t hctl)
{
	uint8_t	buffer[2] = {SPI_CMD_WRHCTL, hctl};
	spi_data_xfer(buffer, buffer, sizeof(buffer) / sizeof(buffer[0]));
}

uint8_t hxfr_read(void)
{
	uint8_t	buffer[2] = {SPI_CMD_RDHXFR, 0x00};
	spi_data_xfer(buffer, buffer, sizeof(buffer) / sizeof(buffer[0]));
	return buffer[1];
}

void hxfr_write(uint8_t hxfr)
{
	uint8_t	buffer[2] = {SPI_CMD_WRHXFR, hxfr};
	spi_data_xfer(buffer, buffer, sizeof(buffer) / sizeof(buffer[0]));
}

uint8_t hrsl_read(void)
{
	uint8_t	buffer[2] = {SPI_CMD_RDHRSL, 0x00};
	spi_data_xfer(buffer, buffer, sizeof(buffer) / sizeof(buffer[0]));
	return buffer[1];
}

uint8_t rcvfifo_write(uint8_t *src_buf, size_t data_size)
{
	uint8_t	cmd = SPI_CMD_WRRCVFIFO;
	spi_data_xfer(&cmd, NULL, 1);
	size_t size = spi_data_xfer(src_buf, NULL, data_size);
	return size;
}

uint8_t rcvfifo_read(uint8_t *dst_buf, size_t data_size)
{
	uint8_t	cmd = SPI_CMD_RDRCVFIFO;
	spi_data_xfer(&cmd, NULL, 1);
	size_t size = spi_data_xfer(NULL, dst_buf, data_size);
	return size;
}

uint8_t sndfifo_write(uint8_t *src_buf, size_t data_size)
{
	uint8_t	cmd = SPI_CMD_WRSNDFIFO;
	spi_data_xfer(&cmd, NULL, 1);
	size_t size = spi_data_xfer(src_buf, NULL, data_size);
	return size;
}

uint8_t sndfifo_read(uint8_t *dst_buf, size_t data_size)
{
	uint8_t	cmd = SPI_CMD_RDSNDFIFO;
	spi_data_xfer(&cmd, NULL, 1);
	size_t size = spi_data_xfer(NULL, dst_buf, data_size);
	return size;
}

uint8_t sudfifo_write(uint8_t *src_buf, size_t data_size)
{
	uint8_t	cmd = SPI_CMD_WRSUDFIFO;
	spi_data_xfer(&cmd, NULL, 1);
	size_t size = spi_data_xfer(src_buf, NULL, data_size);
	return size;
}

uint8_t sudfifo_read(uint8_t *dst_buf, size_t data_size)
{
	uint8_t	cmd = SPI_CMD_RDSUDFIFO;
	spi_data_xfer(&cmd, NULL, 1);
	size_t size = spi_data_xfer(NULL, dst_buf, data_size);
	return size;
}

void rcvbc_write(uint8_t size)
{
	uint8_t	buffer[2] = {SPI_CMD_WRRCVBC, size};
	spi_data_xfer(buffer, buffer, sizeof(buffer) / sizeof(buffer[0]));
}

uint8_t rcvbc_read(void)
{
	uint8_t	buffer[2] = {SPI_CMD_RDRCVBC, 0x00};
	spi_data_xfer(buffer, buffer, sizeof(buffer) / sizeof(buffer[0]));
	return buffer[1];
}

void sndbc_write(uint8_t size)
{
	uint8_t	buffer[2] = {SPI_CMD_WRSNDBC, size};
	spi_data_xfer(buffer, buffer, sizeof(buffer) / sizeof(buffer[0]));
}

uint8_t sndbc_read(void)
{
	uint8_t	buffer[2] = {SPI_CMD_RDSNDBC, 0x00};
	spi_data_xfer(buffer, buffer, sizeof(buffer) / sizeof(buffer[0]));
	return buffer[1];
}

void kb_usb_fullduplex_spi_set(void)
{
	d_print("Set full duplex SPI transmission\r\n");
	PORT_ResetBits(MDR_PORTE, PORT_Pin_0);
	pinctl_write(0x1E);
	PORT_SetBits(MDR_PORTE, PORT_Pin_0);
}

void kb_usb_revision_read(void)
{
	PORT_ResetBits(MDR_PORTE, PORT_Pin_0);
	d_print("REVISION 0x%02X\r\n", revision_read());
	PORT_SetBits(MDR_PORTE, PORT_Pin_0);
}

void kb_usb_chip_reset(void)
{
	PORT_ResetBits(MDR_PORTE, PORT_Pin_0);
	d_print("USBIRQ: 0x%02X\r\n", usbirq_read());
	PORT_SetBits(MDR_PORTE, PORT_Pin_0);

	PORT_ResetBits(MDR_PORTE, PORT_Pin_0);
	uint8_t val = usbctl_read();
	PORT_SetBits(MDR_PORTE, PORT_Pin_0);
	d_print("USBCTL: 0x%02X\r\n", val);

	d_print("Reset chip\r\n");
	PORT_ResetBits(MDR_PORTE, PORT_Pin_0);
	usbctl_write(val | (0x1 << 5));
	PORT_SetBits(MDR_PORTE, PORT_Pin_0);
	mdelay(1);

	PORT_ResetBits(MDR_PORTE, PORT_Pin_0);
	usbctl_write(val);
	PORT_SetBits(MDR_PORTE, PORT_Pin_0);

	PORT_ResetBits(MDR_PORTE, PORT_Pin_0);
	d_print("USBCTL: 0x%02X\r\n", usbctl_read());
	PORT_SetBits(MDR_PORTE, PORT_Pin_0);

	PORT_ResetBits(MDR_PORTE, PORT_Pin_0);
	d_print("USBIRQ: 0x%02X\r\n", usbirq_read());
	PORT_SetBits(MDR_PORTE, PORT_Pin_0);

	d_print("Waiting the oscillator stabilization\r\n");

	PORT_ResetBits(MDR_PORTE, PORT_Pin_0);
	while((usbirq_read() & 0x1) != 0x1) {
		PORT_SetBits(MDR_PORTE, PORT_Pin_0);
		mdelay(1);
		PORT_ResetBits(MDR_PORTE, PORT_Pin_0);
	}
	PORT_SetBits(MDR_PORTE, PORT_Pin_0);

	PORT_ResetBits(MDR_PORTE, PORT_Pin_0);
	d_print("USBCTL: 0x%02X\r\n", usbctl_read());
	PORT_SetBits(MDR_PORTE, PORT_Pin_0);

	PORT_ResetBits(MDR_PORTE, PORT_Pin_0);
	d_print("USBIRQ: 0x%02X\r\n", usbirq_read());
	PORT_SetBits(MDR_PORTE, PORT_Pin_0);

	PORT_ResetBits(MDR_PORTE, PORT_Pin_0);
	usbirq_write(0x1);
	PORT_SetBits(MDR_PORTE, PORT_Pin_0);

	PORT_ResetBits(MDR_PORTE, PORT_Pin_0);
	d_print("USBIRQ: 0x%02X\r\n", usbirq_read());
	PORT_SetBits(MDR_PORTE, PORT_Pin_0);

}

void kb_usb_power_enable(void)
{
	PORT_ResetBits(MDR_PORTE, PORT_Pin_0);
	uint8_t iopins1 = iopins1_read();
	PORT_SetBits(MDR_PORTE, PORT_Pin_0);

	d_print("IOPINS1: 0x%02X\r\n", iopins1);

	PORT_ResetBits(MDR_PORTE, PORT_Pin_0);
	iopins1_write(iopins1 | 0x1);
	PORT_SetBits(MDR_PORTE, PORT_Pin_0);

	d_print("USB power is enabled\r\n");

	PORT_ResetBits(MDR_PORTE, PORT_Pin_0);
	iopins1 = iopins1_read();
	PORT_SetBits(MDR_PORTE, PORT_Pin_0);
	d_print("IOPINS1: 0x%02X\r\n", iopins1);
}

void kb_usb_mode_set(void)
{
	PORT_ResetBits(MDR_PORTE, PORT_Pin_0);
	uint8_t mode = mode_read();
	PORT_SetBits(MDR_PORTE, PORT_Pin_0);

	d_print("MODE: 0x%02X\r\n", mode);

	PORT_ResetBits(MDR_PORTE, PORT_Pin_0);
	mode_write(mode |0x80 | 0x40 |0x1);
	PORT_SetBits(MDR_PORTE, PORT_Pin_0);

	d_print("HOST mode, DPPULLDN and DNPULLDN are set\r\n");

	PORT_ResetBits(MDR_PORTE, PORT_Pin_0);
	d_print("MODE: 0x%02X\r\n", mode_read());
	PORT_SetBits(MDR_PORTE, PORT_Pin_0);
}

void kb_usb_bus_reset(void)
{
	PORT_ResetBits(MDR_PORTE, PORT_Pin_0);
	uint8_t hctl = hctl_read();
	PORT_SetBits(MDR_PORTE, PORT_Pin_0);
	d_print("HCTL: 0x%02X\r\n", hctl);

	PORT_ResetBits(MDR_PORTE, PORT_Pin_0);
	mode_write(hctl |0x1);
	PORT_SetBits(MDR_PORTE, PORT_Pin_0);

	PORT_ResetBits(MDR_PORTE, PORT_Pin_0);
	d_print("HCTL: 0x%02X\r\n", hctl_read());
	PORT_SetBits(MDR_PORTE, PORT_Pin_0);

	PORT_ResetBits(MDR_PORTE, PORT_Pin_0);
	while((hctl_read() & 0x1) == 0x1) {
		PORT_SetBits(MDR_PORTE, PORT_Pin_0);
		mdelay(1);
		PORT_ResetBits(MDR_PORTE, PORT_Pin_0);
	}
	PORT_SetBits(MDR_PORTE, PORT_Pin_0);

	PORT_ResetBits(MDR_PORTE, PORT_Pin_0);
	uint8_t mode = mode_read();
	PORT_SetBits(MDR_PORTE, PORT_Pin_0);
	d_print("MODE: 0x%02X\r\n", mode);

	PORT_ResetBits(MDR_PORTE, PORT_Pin_0);
	mode_write(mode |0x08);
	PORT_SetBits(MDR_PORTE, PORT_Pin_0);

	PORT_ResetBits(MDR_PORTE, PORT_Pin_0);
	d_print("MODE: 0x%02X\r\n", mode_read());
	PORT_SetBits(MDR_PORTE, PORT_Pin_0);

	PORT_ResetBits(MDR_PORTE, PORT_Pin_0);
	while((hirq_read() & 0x40) != 0x40) {
		PORT_SetBits(MDR_PORTE, PORT_Pin_0);
		mdelay(1);
		PORT_ResetBits(MDR_PORTE, PORT_Pin_0);
	}
	PORT_SetBits(MDR_PORTE, PORT_Pin_0);

	d_print("USB bus was reset\r\n");
}

void kb_usb_device_detection_cycle(void)
{

	while(1) {
		d_print("----------------------\r\n");
		PORT_ResetBits(MDR_PORTE, PORT_Pin_0);
		while((hirq_read() & 0x20) == 0x0) {
			PORT_SetBits(MDR_PORTE, PORT_Pin_0);
			mdelay(1000);
			PORT_ResetBits(MDR_PORTE, PORT_Pin_0);
		}
		PORT_SetBits(MDR_PORTE, PORT_Pin_0);

		PORT_ResetBits(MDR_PORTE, PORT_Pin_0);
		d_print("HIRQ: 0x%02X\r\n", hirq_read());
		PORT_SetBits(MDR_PORTE, PORT_Pin_0);

		PORT_ResetBits(MDR_PORTE, PORT_Pin_0);
		hirq_write(0x20);
		PORT_SetBits(MDR_PORTE, PORT_Pin_0);

		PORT_ResetBits(MDR_PORTE, PORT_Pin_0);
		d_print("HIRQ: 0x%02X\r\n", hirq_read());
		PORT_SetBits(MDR_PORTE, PORT_Pin_0);

		PORT_ResetBits(MDR_PORTE, PORT_Pin_0);
		d_print("HRSL: 0x%02X\r\n", hrsl_read());
		PORT_SetBits(MDR_PORTE, PORT_Pin_0);

		PORT_ResetBits(MDR_PORTE, PORT_Pin_0);
		d_print("HIRQ: 0x%02X\r\n", hirq_read());
		PORT_SetBits(MDR_PORTE, PORT_Pin_0);

		mdelay(1000);

		PORT_ResetBits(MDR_PORTE, PORT_Pin_0);
		d_print("HIRQ: 0x%02X\r\n", hirq_read());
		PORT_SetBits(MDR_PORTE, PORT_Pin_0);
	}
}

void kb_usb_device_detect(void)
{
	PORT_ResetBits(MDR_PORTE, PORT_Pin_0);
	uint8_t hirq = hirq_read();
	PORT_SetBits(MDR_PORTE, PORT_Pin_0);
	d_print("HIRQ: 0x%02X\r\n", hirq);

	PORT_ResetBits(MDR_PORTE, PORT_Pin_0);
	uint8_t hctl = hctl_read();
	PORT_SetBits(MDR_PORTE, PORT_Pin_0);

	PORT_ResetBits(MDR_PORTE, PORT_Pin_0);
	d_print("HRSL: 0x%02X\r\n", hrsl_read());
	PORT_SetBits(MDR_PORTE, PORT_Pin_0);

	PORT_ResetBits(MDR_PORTE, PORT_Pin_0);
	hctl_write(hctl | 0x04);
	PORT_SetBits(MDR_PORTE, PORT_Pin_0);

	PORT_ResetBits(MDR_PORTE, PORT_Pin_0);
	uint8_t hrsl = hrsl_read();
	d_print("HRSL: 0x%02X\r\n", hrsl);
	PORT_SetBits(MDR_PORTE, PORT_Pin_0);
	/*
		(J,K)
		(0,0) - Single Ended Zero
		(0,1) - Low Speed
		(1,0) - Full Speed
		(1,1) - Single Ended One (Illegal state)
	*/
	switch(hrsl & 0xD0){
	case 0x40:
			d_print("Low speed device connected\r\n");
			PORT_ResetBits(MDR_PORTE, PORT_Pin_0);
			uint8_t mode = mode_read();
			PORT_SetBits(MDR_PORTE, PORT_Pin_0);

			PORT_ResetBits(MDR_PORTE, PORT_Pin_0);
			mode_write(mode | 0x02);
			PORT_SetBits(MDR_PORTE, PORT_Pin_0);
			break;
	case 0x80:
			d_print("High speed device connected\r\n");
			break;
	case 0xD0:
			d_print("Bus illegal state\r\n");
			break;
	case 0x00:
			d_print("Device not connected\r\n");
			break;
	}
}

void kb_usb_setup_send(void)
{
	struct std_request {
		uint8_t		bm_request_type;
		uint8_t		b_request;
		uint16_t	w_value;
		uint16_t	w_index;
		uint16_t	w_length;
	};
	struct std_request set_addr = {0x0, 0x5, 0x34, 0x0, 0x0};

	d_print("bm_request_type: 0x%02X\r\n", set_addr.bm_request_type);
	d_print("b_request: 0x%02X\r\n", set_addr.b_request);
	d_print("w_value: 0x%04X\r\n", set_addr.w_value);
	d_print("w_index: 0x%04X\r\n", set_addr.w_index);
	d_print("w_length: 0x%04X\r\n", set_addr.w_length);

	PORT_ResetBits(MDR_PORTE, PORT_Pin_0);
	sudfifo_write((uint8_t*)&set_addr, sizeof(set_addr));
	PORT_SetBits(MDR_PORTE, PORT_Pin_0);

	PORT_ResetBits(MDR_PORTE, PORT_Pin_0);
	sudfifo_read((uint8_t*)&set_addr, sizeof(set_addr));
	PORT_SetBits(MDR_PORTE, PORT_Pin_0);

	d_print("bm_request_type: 0x%02X\r\n", set_addr.bm_request_type);
	d_print("b_request: 0x%02X\r\n", set_addr.b_request);
	d_print("w_value: 0x%04X\r\n", set_addr.w_value);
	d_print("w_index: 0x%04X\r\n", set_addr.w_index);
	d_print("w_length: 0x%04X\r\n", set_addr.w_length);

	PORT_ResetBits(MDR_PORTE, PORT_Pin_0);
	peraddr_write(0x00);
	PORT_SetBits(MDR_PORTE, PORT_Pin_0);

	PORT_ResetBits(MDR_PORTE, PORT_Pin_0);
	hxfr_write(0x10);
	PORT_SetBits(MDR_PORTE, PORT_Pin_0);

	PORT_ResetBits(MDR_PORTE, PORT_Pin_0);
	while((hirq_read() & 0x80) != 0x80) {
		PORT_SetBits(MDR_PORTE, PORT_Pin_0);
//		u100delay(1);
		__DSB();
		PORT_ResetBits(MDR_PORTE, PORT_Pin_0);
	}
	PORT_SetBits(MDR_PORTE, PORT_Pin_0);

	PORT_ResetBits(MDR_PORTE, PORT_Pin_0);
	d_print("HRSLT: 0x%01X\r\n", hrsl_read() & 0x0F);
	PORT_SetBits(MDR_PORTE, PORT_Pin_0);

	PORT_ResetBits(MDR_PORTE, PORT_Pin_0);
	peraddr_write(0x00);
	PORT_SetBits(MDR_PORTE, PORT_Pin_0);

	PORT_ResetBits(MDR_PORTE, PORT_Pin_0);
	hxfr_write(0x80);
	PORT_SetBits(MDR_PORTE, PORT_Pin_0);

	PORT_ResetBits(MDR_PORTE, PORT_Pin_0);
	while((hirq_read() & 0x80) != 0x80) {
		PORT_SetBits(MDR_PORTE, PORT_Pin_0);
//		u100delay(1);
		__DSB();
		PORT_ResetBits(MDR_PORTE, PORT_Pin_0);
	}
	PORT_SetBits(MDR_PORTE, PORT_Pin_0);

	PORT_ResetBits(MDR_PORTE, PORT_Pin_0);
	d_print("HRSLT: 0x%01X\r\n", hrsl_read() & 0x0F);
	PORT_SetBits(MDR_PORTE, PORT_Pin_0);
}

int main(void)
{

/*****************************************************************************************/
	clock_init();

	gpio_init();

	console_init();

	spi_init();
	/* 0. FDUPSPI */
	kb_usb_fullduplex_spi_set();

	/* 1. REVISON */
	kb_usb_revision_read();

	/* 2. CHIPRES, OSCOKIRQ */
	kb_usb_chip_reset();

	/* 2.1 GPOUT0 */
	kb_usb_power_enable();

	/* 3. HOST, DPPULLDN, DMPULLDN */
	kb_usb_mode_set();

	/* 4. BUSRST, SOFKAENAB, FRAMEIRQ */
	kb_usb_bus_reset();

	mdelay(7000);

	/* 5. CONDETIRQ, SAMPLEBUS, JSTATUS, KTATUS */
	kb_usb_device_detect();
//	kb_usb_device_detection_cycle();

	/* 6. SETUP HS-IN */
	kb_usb_setup_send();

	/* 7. BULK-IN */
/*****************************************************************************************/

/*****************************************************************************************/

/*****************************************************************************************/
	PORT_SetBits(MDR_PORTD, PORT_Pin_7);
	while(1)
	{
		PORT_SetBits(MDR_PORTD, PORT_Pin_7);
		mdelay(500);

		PORT_ResetBits(MDR_PORTD, PORT_Pin_7);
		mdelay(500);

	}

	return 0;
}
