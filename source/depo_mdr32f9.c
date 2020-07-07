#include <drivers.h>

#define ALL_PORTS_CLK (RST_CLK_PCLK_PORTA | RST_CLK_PCLK_PORTB | \
                       RST_CLK_PCLK_PORTC | RST_CLK_PCLK_PORTD | \
                       RST_CLK_PCLK_PORTE | RST_CLK_PCLK_PORTF)


PORT_InitTypeDef PORT_InitStructure;

void Init_All_Ports(void)
{
	/* Enable the RTCHSE clock on all ports */
	RST_CLK_PCLKcmd(ALL_PORTS_CLK, ENABLE);

	/* Configure all ports to the state as after reset, i.e. low power consumption */
	PORT_StructInit(&PORT_InitStructure);
	PORT_Init(MDR_PORTA, &PORT_InitStructure);
	PORT_Init(MDR_PORTB, &PORT_InitStructure);
	PORT_Init(MDR_PORTC, &PORT_InitStructure);
	PORT_Init(MDR_PORTD, &PORT_InitStructure);
	PORT_Init(MDR_PORTE, &PORT_InitStructure);
	PORT_Init(MDR_PORTF, &PORT_InitStructure);

	/* Disable the RTCHSE clock on all ports */
	RST_CLK_PCLKcmd(ALL_PORTS_CLK, DISABLE);
}


/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */

int main(void)

{
	/*!< Usually, reset is done before the program is to be loaded into microcontroller,
	   and there is no need to perform any special operations to switch the ports
	   to low power consumption mode explicitly. So, the function Init_All_Ports
	   is used here for demonstration purpose only.
	*/
	Init_All_Ports();

	/* Enables the RTCHSE clock on PORTE and PORTF */
	RST_CLK_PCLKcmd(RST_CLK_PCLK_PORTA | RST_CLK_PCLK_PORTD, ENABLE);

	/* Configure PORTF pins 0,1 for output to switch LEDs on/off */

	PORT_InitStructure.PORT_Pin   = (PORT_Pin_7);
	PORT_InitStructure.PORT_OE    = PORT_OE_OUT;
	PORT_InitStructure.PORT_FUNC  = PORT_FUNC_PORT;
	PORT_InitStructure.PORT_MODE  = PORT_MODE_DIGITAL;
	PORT_InitStructure.PORT_SPEED = PORT_SPEED_SLOW;

	PORT_Init(MDR_PORTA, &PORT_InitStructure);

	PORT_InitStructure.PORT_Pin   = (PORT_Pin_7);
	PORT_InitStructure.PORT_OE    = PORT_OE_OUT;
	PORT_InitStructure.PORT_FUNC  = PORT_FUNC_PORT;
	PORT_InitStructure.PORT_MODE  = PORT_MODE_DIGITAL;
	PORT_InitStructure.PORT_SPEED = PORT_SPEED_SLOW;

	PORT_Init(MDR_PORTD, &PORT_InitStructure);

	PORT_SetBits(MDR_PORTA, PORT_Pin_7);

	PORT_SetBits(MDR_PORTD, PORT_Pin_7);

	while(1)
	{}

	return 0;
}
