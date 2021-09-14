#include <string.h>
#include "time.h"
#include "drivers.h"
#include "console.h"
#include "init.h"
#include "config.h"
#include "debug.h"
#include "maxm86161.h"
#include "pulsometr.h"

#include "spo2_algorithm.h"

//void i2c2_master_init_1(void)
//{
//  /* (1) Enables GPIO clock and configures the I2C2 pins **********************/
//  /*    (SCL on PC.0, SDA on PC.1)                     **********************/
//
//  /* Enable the peripheral clock of GPIOC */
//  LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOC);
//
//  /* Configure SCL Pin as : Alternate function, High Speed, Open drain, Pull up */
//  LL_GPIO_SetPinMode(GPIOC, LL_GPIO_PIN_0, LL_GPIO_MODE_ALTERNATE);
//  LL_GPIO_SetAFPin_0_7(GPIOC, LL_GPIO_PIN_0, LL_GPIO_AF_4);
//  LL_GPIO_SetPinSpeed(GPIOC, LL_GPIO_PIN_0, LL_GPIO_SPEED_FREQ_HIGH);
//  LL_GPIO_SetPinOutputType(GPIOC, LL_GPIO_PIN_0, LL_GPIO_OUTPUT_OPENDRAIN);
//  LL_GPIO_SetPinPull(GPIOC, LL_GPIO_PIN_0, LL_GPIO_PULL_UP);
//
//  /* Configure SDA Pin as : Alternate function, High Speed, Open drain, Pull up */
//  LL_GPIO_SetPinMode(GPIOC, LL_GPIO_PIN_1, LL_GPIO_MODE_ALTERNATE);
//  LL_GPIO_SetAFPin_0_7(GPIOC, LL_GPIO_PIN_1, LL_GPIO_AF_4);
//  LL_GPIO_SetPinSpeed(GPIOC, LL_GPIO_PIN_1, LL_GPIO_SPEED_FREQ_HIGH);
//  LL_GPIO_SetPinOutputType(GPIOC, LL_GPIO_PIN_1, LL_GPIO_OUTPUT_OPENDRAIN);
//  LL_GPIO_SetPinPull(GPIOC, LL_GPIO_PIN_1, LL_GPIO_PULL_UP);
//
//  /* (2) Enable the I2C2 peripheral clock and I2C2 clock source ***************/
//
//  /* Enable the peripheral clock for I2C2 */
//  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_I2C2);
//
//  /* Set I2C2 clock source as SYSCLK */
//  LL_RCC_SetI2CClockSource(LL_RCC_I2C2_CLKSOURCE_SYSCLK);
//
//  /* (3) Configure I2C2 functional parameters ********************************/
//
//  /* Disable I2C2 prior modifying configuration registers */
//  LL_I2C_Disable(I2C2);
//
//  /* Configure the SDA setup, hold time and the SCL high, low period */
//  /* (uint32_t)0x00F02B86 = I2C_TIMING*/
//  LL_I2C_SetTiming(I2C2, I2C_TIMING);
//
//  /* Configure the Own Address1                   */
//  /* Reset Values of :
//   *     - OwnAddress1 is 0x00
//   *     - OwnAddrSize is LL_I2C_OWNADDRESS1_7BIT
//   *     - Own Address1 is disabled
//   */
//  //LL_I2C_SetOwnAddress1(I2C2, 0x00, LL_I2C_OWNADDRESS1_7BIT);
//  //LL_I2C_DisableOwnAddress1(I2C2);
//
//  /* Enable Clock stretching */
//  /* Reset Value is Clock stretching enabled */
//  //LL_I2C_EnableClockStretching(I2C2);
//
//  /* Configure Digital Noise Filter */
//  /* Reset Value is 0x00            */
//  //LL_I2C_SetDigitalFilter(I2C2, 0x00);
//
//  /* Enable Analog Noise Filter           */
//  /* Reset Value is Analog Filter enabled */
//  //LL_I2C_EnableAnalogFilter(I2C2);
//
//  /* Enable General Call                  */
//  /* Reset Value is General Call disabled */
//  //LL_I2C_EnableGeneralCall(I2C2);
//
//  /* Configure the 7bits Own Address2               */
//  /* Reset Values of :
//   *     - OwnAddress2 is 0x00
//   *     - OwnAddrMask is LL_I2C_OWNADDRESS2_NOMASK
//   *     - Own Address2 is disabled
//   */
//  //LL_I2C_SetOwnAddress2(I2C2, 0x00, LL_I2C_OWNADDRESS2_NOMASK);
//  //LL_I2C_DisableOwnAddress2(I2C2);
//
//  /* Configure the Master to operate in 7-bit or 10-bit addressing mode */
//  /* Reset Value is LL_I2C_ADDRESSING_MODE_7BIT                         */
//  //LL_I2C_SetMasterAddressingMode(I2C2, LL_I2C_ADDRESSING_MODE_7BIT);
//
//  /* Enable Peripheral in I2C mode */
//  /* Reset Value is I2C mode */
//  //LL_I2C_SetMode(I2C2, LL_I2C_MODE_I2C);
//
//  /* (4) Enable I2C2 **********************************************************/
//  LL_I2C_Enable(I2C2);
//}
//
//void Handle_I2C_Master_1(void)
//{
//  /* (1) Initiate a Start condition to the Slave device ***********************/
//
//  /* Master Generate Start condition for a write request :              */
//  /*    - to the Slave with a 7-Bit SLAVE_OWN_ADDRESS                   */
//  /*    - with a auto stop condition generation when transmit all bytes */
//  LL_I2C_HandleTransfer(I2C3, SLAVE_OWN_ADDRESS, LL_I2C_ADDRSLAVE_7BIT, ubNbDataToTransmit, LL_I2C_MODE_AUTOEND, LL_I2C_GENERATE_START_WRITE);
//
//  /* (2) Loop until end of transfer received (STOP flag raised) ***************/
//
//#if (USE_TIMEOUT == 1)
//  Timeout = I2C_SEND_TIMEOUT_TXIS_MS;
//#endif /* USE_TIMEOUT */
//
//  /* Loop until STOP flag is raised  */
//  while(!LL_I2C_IsActiveFlag_STOP(I2C3))
//  {
//    /* (2.1) Transmit data (TXIS flag raised) *********************************/
//
//    /* Check TXIS flag value in ISR register */
//    if(LL_I2C_IsActiveFlag_TXIS(I2C3))
//    {
//      /* Write data in Transmit Data register.
//      TXIS flag is cleared by writing data in TXDR register */
//      LL_I2C_TransmitData8(I2C3, (*pTransmitBuffer++));
//
//#if (USE_TIMEOUT == 1)
//      Timeout = I2C_SEND_TIMEOUT_TXIS_MS;
//#endif /* USE_TIMEOUT */
//    }
//
//#if (USE_TIMEOUT == 1)
//    /* Check Systick counter flag to decrement the time-out value */
//    if (LL_SYSTICK_IsActiveCounterFlag())
//    {
//      if(Timeout-- == 0)
//      {
//        /* Time-out occurred. Set LED1 to blinking mode */
//        LED_Blinking(LED_BLINK_SLOW);
//      }
//    }
//#endif /* USE_TIMEOUT */
//  }
//
//  /* (3) Clear pending flags, Data consistency are checking into Slave process */
//
//  /* End of I2C_SlaveReceiver_MasterTransmitter Process */
//  LL_I2C_ClearFlag_STOP(I2C3);
//
//  /* Turn LED1 On:
//   *  - Expected bytes have been sent
//   *  - Master Tx sequence completed successfully
//   */
//  LED_On();
//}
//
//void Configure_I2C_Master(void)
//{
//  /* (1) Enables GPIO clock and configures the I2C3 pins **********************/
//  /*    (SCL on PC.0, SDA on PC.1)                     **********************/
//
//  /* Enable the peripheral clock of GPIOC */
//  LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOC);
//
//  /* Configure SCL Pin as : Alternate function, High Speed, Open drain, Pull up */
//  LL_GPIO_SetPinMode(GPIOC, LL_GPIO_PIN_0, LL_GPIO_MODE_ALTERNATE);
//  LL_GPIO_SetAFPin_0_7(GPIOC, LL_GPIO_PIN_0, LL_GPIO_AF_4);
//  LL_GPIO_SetPinSpeed(GPIOC, LL_GPIO_PIN_0, LL_GPIO_SPEED_FREQ_HIGH);
//  LL_GPIO_SetPinOutputType(GPIOC, LL_GPIO_PIN_0, LL_GPIO_OUTPUT_OPENDRAIN);
//  LL_GPIO_SetPinPull(GPIOC, LL_GPIO_PIN_0, LL_GPIO_PULL_UP);
//
//  /* Configure SDA Pin as : Alternate function, High Speed, Open drain, Pull up */
//  LL_GPIO_SetPinMode(GPIOC, LL_GPIO_PIN_1, LL_GPIO_MODE_ALTERNATE);
//  LL_GPIO_SetAFPin_0_7(GPIOC, LL_GPIO_PIN_1, LL_GPIO_AF_4);
//  LL_GPIO_SetPinSpeed(GPIOC, LL_GPIO_PIN_1, LL_GPIO_SPEED_FREQ_HIGH);
//  LL_GPIO_SetPinOutputType(GPIOC, LL_GPIO_PIN_1, LL_GPIO_OUTPUT_OPENDRAIN);
//  LL_GPIO_SetPinPull(GPIOC, LL_GPIO_PIN_1, LL_GPIO_PULL_UP);
//
//  /* (2) Enable the I2C3 peripheral clock and I2C3 clock source ***************/
//
//  /* Enable the peripheral clock for I2C3 */
//  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_I2C3);
//
//  /* Set I2C3 clock source as SYSCLK */
//  LL_RCC_SetI2CClockSource(LL_RCC_I2C3_CLKSOURCE_SYSCLK);
//
//  /* (3) Configure NVIC for I2C3 **********************************************/
//
//  /* Configure Event IT:
//   *  - Set priority for I2C3_EV_IRQn
//   *  - Enable I2C3_EV_IRQn
//   */
//  NVIC_SetPriority(I2C3_EV_IRQn, 0);
//  NVIC_EnableIRQ(I2C3_EV_IRQn);
//
//  /* Configure Error IT:
//   *  - Set priority for I2C3_ER_IRQn
//   *  - Enable I2C3_ER_IRQn
//   */
//  NVIC_SetPriority(I2C3_ER_IRQn, 0);
//  NVIC_EnableIRQ(I2C3_ER_IRQn);
//
//  /* (4) Configure I2C3 functional parameters *********************************/
//
//  /* Disable I2C3 prior modifying configuration registers */
//  LL_I2C_Disable(I2C3);
//
//  /* Configure the SDA setup, hold time and the SCL high, low period */
//  /* (uint32_t)0x00F02B86 = I2C_TIMING*/
//  LL_I2C_SetTiming(I2C3, I2C_TIMING);
//
//  /* Configure the Own Address1                   */
//  /* Reset Values of :
//   *     - OwnAddress1 is 0x00
//   *     - OwnAddrSize is LL_I2C_OWNADDRESS1_7BIT
//   *     - Own Address1 is disabled
//   */
//  //LL_I2C_SetOwnAddress1(I2C3, 0x00, LL_I2C_OWNADDRESS1_7BIT);
//  //LL_I2C_DisableOwnAddress1(I2C3);
//
//  /* Enable Clock stretching */
//  /* Reset Value is Clock stretching enabled */
//  //LL_I2C_EnableClockStretching(I2C3);
//
//  /* Configure Digital Noise Filter */
//  /* Reset Value is 0x00            */
//  //LL_I2C_SetDigitalFilter(I2C3, 0x00);
//
//  /* Enable Analog Noise Filter           */
//  /* Reset Value is Analog Filter enabled */
//  //LL_I2C_EnableAnalogFilter(I2C3);
//
//  /* Enable General Call                  */
//  /* Reset Value is General Call disabled */
//  //LL_I2C_EnableGeneralCall(I2C3);
//
//  /* Configure the 7bits Own Address2               */
//  /* Reset Values of :
//   *     - OwnAddress2 is 0x00
//   *     - OwnAddrMask is LL_I2C_OWNADDRESS2_NOMASK
//   *     - Own Address2 is disabled
//   */
//  //LL_I2C_SetOwnAddress2(I2C3, 0x00, LL_I2C_OWNADDRESS2_NOMASK);
//  //LL_I2C_DisableOwnAddress2(I2C3);
//
//  /* Configure the Master to operate in 7-bit or 10-bit addressing mode */
//  /* Reset Value is LL_I2C_ADDRESSING_MODE_7BIT                         */
//  //LL_I2C_SetMasterAddressingMode(I2C3, LL_I2C_ADDRESSING_MODE_7BIT);
//
//  /* Enable Peripheral in I2C mode */
//  /* Reset Value is I2C mode */
//  //LL_I2C_SetMode(I2C3, LL_I2C_MODE_I2C);
//
//  /* (5) Enable I2C3 **********************************************************/
//  LL_I2C_Enable(I2C3);
//
//  /* (6) Enable I2C3 transfer complete/error interrupts:
//   *  - Enable Receive Interrupt
//   *  - Enable Not acknowledge received interrupt
//   *  - Enable Error interrupts
//   *  - Enable Stop interrupt
//   */
//  LL_I2C_EnableIT_RX(I2C3);
//  LL_I2C_EnableIT_NACK(I2C3);
//  LL_I2C_EnableIT_ERR(I2C3);
//  LL_I2C_EnableIT_STOP(I2C3);
//}
//
//void Handle_I2C_Master(void)
//{
//  /* (1) Initiate a Start condition to the Slave device ***********************/
//
//  /* Master Generate Start condition for a read request:
//   *  - to the Slave with a 7-Bit SLAVE_OWN_ADDRESS
//   *  - with a auto stop condition generation when receive 1 byte
//   */
//  LL_I2C_HandleTransfer(I2C3, SLAVE_OWN_ADDRESS, LL_I2C_ADDRSLAVE_7BIT, 1, LL_I2C_MODE_AUTOEND, LL_I2C_GENERATE_START_READ);
//}
//
//void Master_Reception_Callback(void)
//{
//  /* Read character in Receive Data register.
//  RXNE flag is cleared by reading data in RXDR register */
//  aReceiveBuffer[ubReceiveIndex++] = LL_I2C_ReceiveData8(I2C3);
//}

#define INT_RI_Pin LL_GPIO_PIN_4
#define INT_RI_GPIO_Port GPIOB
#define DOWN_Pin LL_GPIO_PIN_15
#define DOWN_GPIO_Port GPIOA
#define DOWN_EXTI_IRQn EXTI15_10_IRQn
#define LCD_CS_Pin LL_GPIO_PIN_0
#define LCD_CS_GPIO_Port GPIOI
#define UP_Pin LL_GPIO_PIN_14
#define UP_GPIO_Port GPIOH
#define UP_EXTI_IRQn EXTI15_10_IRQn
#define BLE_RST_Pin LL_GPIO_PIN_2
#define BLE_RST_GPIO_Port GPIOI
#define BLE_WKP_Pin LL_GPIO_PIN_12
#define BLE_WKP_GPIO_Port GPIOH
#define PW_KEY_Pin LL_GPIO_PIN_4
#define PW_KEY_GPIO_Port GPIOE
#define INT_GA1_Pin LL_GPIO_PIN_13
#define INT_GA1_GPIO_Port GPIOC
#define BLE_INT_Pin LL_GPIO_PIN_6
#define BLE_INT_GPIO_Port GPIOE
#define LCD_DC_Pin LL_GPIO_PIN_3
#define LCD_DC_GPIO_Port GPIOD
#define VCI_EN_Pin LL_GPIO_PIN_8
#define VCI_EN_GPIO_Port GPIOA
#define BLE_RX_Pin LL_GPIO_PIN_8
#define BLE_RX_GPIO_Port GPIOG
#define BLE_TX_Pin LL_GPIO_PIN_7
#define BLE_TX_GPIO_Port GPIOG
#define EN_VDDIO_LCD_Pin LL_GPIO_PIN_9
#define EN_VDDIO_LCD_GPIO_Port GPIOC
#define ALERT_TEMP_Pin LL_GPIO_PIN_3
#define ALERT_TEMP_GPIO_Port GPIOC
#define Enter_Pin LL_GPIO_PIN_0
#define Enter_GPIO_Port GPIOA
#define Enter_EXTI_IRQn EXTI0_IRQn
#define ALT_Bat_Pin LL_GPIO_PIN_5
#define ALT_Bat_GPIO_Port GPIOA
#define INT_HRV_Pin LL_GPIO_PIN_1
#define INT_HRV_GPIO_Port GPIOA
#define BLK_Pin LL_GPIO_PIN_11
#define BLK_GPIO_Port GPIOF
#define EN_LDO_Pin LL_GPIO_PIN_11
#define EN_LDO_GPIO_Port GPIOH
#define SOS_Pin LL_GPIO_PIN_2
#define SOS_GPIO_Port GPIOA
#define SOS_EXTI_IRQn EXTI2_IRQn
#define EN_GSM_Pin LL_GPIO_PIN_4
#define EN_GSM_GPIO_Port GPIOA
#define ON_HRV_Pin LL_GPIO_PIN_0
#define ON_HRV_GPIO_Port GPIOB
#define ON_Buz_Pin LL_GPIO_PIN_10
#define ON_Buz_GPIO_Port GPIOB
#define LCD_RESET_Pin LL_GPIO_PIN_12
#define LCD_RESET_GPIO_Port GPIOB

#define GUARD_COUNTER_INIT	1000


maxm86161_device_config_t default_maxim_config = {

	/* uint8_t int_level */
	3,

	/* maxm86161_ledsq_cfg_t ledsq_cfg */
	{
#if (PROX_SELECTION & PROX_USE_IR)
		0x02,//LED2 - IR
		0x01,//LED1 - green
		0x03,//LED3 - RED
#elif (PROX_SELECTION & PROX_USE_RED)
		0x03,//LED3 - RED
		0x02,//LED2 - IR
		0x01,//LED1 - green
#else // default use GREEN
		0x01,//LED1 - green
		0x02,//LED2 - IR
		0x03,//LED3 - RED
#endif
		0x00,
		0x00,
		0x00,
	},

	/* maxm86161_ledpa_t ledpa_cfg */
	{
			0xFB,// green
			0x00,// IR
			0x00,// LED
	},

	/* maxm86161_ppg_cfg_t ppg_cfg */
	{
		MAXM86161_PPG_CFG_ALC_DS,
		MAXM86161_PPG_CFG_OFFSET_NO,
//	  	MAXM86161_PPG_CFG_TINT_117p3_US,
		MAXM86161_PPG_CFG_TINT_58p7_US, //
//		MAXM86161_PPG_CFG_TINT_29p4_US,//��������
//		MAXM86161_PPG_CFG_LED_RANGE_16k,
		MAXM86161_PPG_CFG_LED_RANGE_32k,
//		MAXM86161_PPG_CFG_SMP_RATE_P1_24sps,
//		MAXM86161_PPG_CFG_SMP_RATE_P1_50sps,
		MAXM86161_PPG_CFG_SMP_RATE_P1_512sps,
		MAXM86161_PPG_CFG_SMP_AVG_1
	},

	/* maxm86161_int_t int_cfg */
	{
		MAXM86161_INT_ENABLE,  //full_fifo
		MAXM86161_INT_DISABLE, //data_rdy
		MAXM86161_INT_DISABLE, //alc_ovf
#ifdef PROXIMITY
		MAXM86161_INT_ENABLE, //proximity
#else
		MAXM86161_INT_DISABLE,
#endif
		MAXM86161_INT_DISABLE, //led_compliant
		MAXM86161_INT_DISABLE, //die_temp
		MAXM86161_INT_DISABLE, //pwr_rdy
		MAXM86161_INT_DISABLE  //sha
	}
};

static void i2c2_init(void)
{
	/**I2C2 GPIO Configuration
	 *
	PB14   ------> I2C2_SDA
	PB13   ------> I2C2_SCL
	*/

	LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOB);

	LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

	GPIO_InitStruct.Pin = LL_GPIO_PIN_14|LL_GPIO_PIN_13;
	GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
	GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
	GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
	GPIO_InitStruct.Alternate = LL_GPIO_AF_4;
	LL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	LL_RCC_SetI2CClockSource(LL_RCC_I2C2_CLKSOURCE_SYSCLK);

	LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_I2C2);

	LL_I2C_Disable(I2C2);

	/** I2C Initialization*/
	LL_I2C_EnableAutoEndMode(I2C2);
	LL_I2C_DisableOwnAddress2(I2C2);
	LL_I2C_DisableGeneralCall(I2C2);
	LL_I2C_EnableClockStretching(I2C2);

	LL_I2C_InitTypeDef I2C_InitStruct = {0};

	I2C_InitStruct.PeripheralMode = LL_I2C_MODE_I2C;

//	I2C_InitStruct.Timing = 0xB020F2FF; // 20KHz
//	I2C_InitStruct.Timing = 0x505092FA; // 50KHz
//	I2C_InitStruct.Timing = 0x30807DAA; // 100KHz
//	I2C_InitStruct.Timing = 0x107029FB; // 200KHz
	I2C_InitStruct.Timing = 0x00F046D9; // 400KHz
	I2C_InitStruct.AnalogFilter = LL_I2C_ANALOGFILTER_ENABLE;
	I2C_InitStruct.DigitalFilter = 0;
	I2C_InitStruct.OwnAddress1 = 0;
	I2C_InitStruct.TypeAcknowledge = LL_I2C_ACK;
	I2C_InitStruct.OwnAddrSize = LL_I2C_OWNADDRESS1_7BIT;

	LL_I2C_Init(I2C2, &I2C_InitStruct);
	LL_I2C_SetOwnAddress2(I2C2, 0, LL_I2C_OWNADDRESS2_NOMASK);

	LL_I2C_Enable(I2C2);
}

int32_t i2c_read(uint8_t chip_addr, uint8_t reg_addr, uint8_t *buffer, size_t buffer_size)
{
	uint32_t guard_counter = GUARD_COUNTER_INIT;

	LL_I2C_ClearFlag_NACK(I2C2);

	LL_I2C_HandleTransfer(I2C2, chip_addr, LL_I2C_ADDRSLAVE_7BIT, 1, LL_I2C_MODE_SOFTEND, LL_I2C_GENERATE_START_WRITE);

	if (LL_I2C_IsActiveFlag_NACK(I2C2) == 1)
		return -1;

	while (1) {

		if (LL_I2C_IsActiveFlag_TXIS(I2C2) == 1)
			LL_I2C_TransmitData8(I2C2, reg_addr);

		if (LL_I2C_IsActiveFlag_NACK(I2C2) == 1)
			return -2;

		if (LL_I2C_IsActiveFlag_TC(I2C2) == 1)
			break;

		if (guard_counter-- == 0)
			return -3;
	}

	LL_I2C_HandleTransfer(I2C2, chip_addr, LL_I2C_ADDRSLAVE_7BIT, buffer_size & 0xFF, LL_I2C_MODE_SOFTEND, LL_I2C_GENERATE_RESTART_7BIT_READ);

	if (LL_I2C_IsActiveFlag_NACK(I2C2) == 1)
		return -4;

	size_t pos = 0;
	guard_counter = GUARD_COUNTER_INIT;
	while (1) {

		if (LL_I2C_IsActiveFlag_RXNE(I2C2) == 1) {
			buffer[pos] = LL_I2C_ReceiveData8(I2C2);
			++pos;
		}

		if (LL_I2C_IsActiveFlag_TC(I2C2) == 1) {
			LL_I2C_GenerateStopCondition(I2C2);
			break;
		}

		if (guard_counter-- == 0)
			return -5;
	}
	return pos;
}

int32_t i2c_write(uint8_t chip_addr, uint8_t reg_addr, uint8_t *data, size_t data_size)
{
	uint32_t guard_counter = GUARD_COUNTER_INIT;

	LL_I2C_ClearFlag_NACK(I2C2);

	LL_I2C_HandleTransfer(I2C2, chip_addr, LL_I2C_ADDRSLAVE_7BIT, data_size + 1, LL_I2C_MODE_SOFTEND, LL_I2C_GENERATE_START_WRITE);

	if (LL_I2C_IsActiveFlag_NACK(I2C2) == 1)
		return -1;

	while (1) {

		if (LL_I2C_IsActiveFlag_TXIS(I2C2) == 1) {
			LL_I2C_TransmitData8(I2C2, reg_addr);
			break;
		}

		if (LL_I2C_IsActiveFlag_NACK(I2C2) == 1)
			return -2;

		if (guard_counter-- == 0)
			return -3;
	}

	size_t pos = 0;
	guard_counter = GUARD_COUNTER_INIT;

	while (1) {

		if (LL_I2C_IsActiveFlag_TXIS(I2C2) == 1) {
			LL_I2C_TransmitData8(I2C2, data[pos++]);
			break;
		}

		if (LL_I2C_IsActiveFlag_NACK(I2C2) == 1)
			return -4;

		if (LL_I2C_IsActiveFlag_TC(I2C2) == 1) {
			LL_I2C_GenerateStopCondition(I2C2);
			break;
		}

		if (guard_counter-- == 0)
			return -5;
	}

	return pos;
}

int32_t maxm86161_hrm_identify_part(int16_t *part_id) {
	int32_t valid_part = 0;

	*part_id = maxm86161_i2c_read_from_register(0xFE);

	switch(*part_id) { // Static HRM/SpO2 supports all maxm86161 parts

		case 0x36:
			valid_part = 1;
			break;
		default:
			valid_part = 0;
			break;
	}

	return valid_part;
}

int maxm86161_test(void) {
	int16_t part_id;
	maxm86161_device_config_t device_config = default_maxim_config;

//	if (!maxm86161_hrm_identify_part(&part_id)) {
//		return 1;
//	}
	while (1) {
		maxm86161_hrm_identify_part(&part_id);
		printk(DEBUG, "part_id: 0x%X \r\n", part_id);
		mdelay(100);
	}
	return 0;

	maxm86161_init_device(device_config);
	// maxm86161_shutdown_device(0);
	//	maxm86161_device_config_t device_config = default_maxim_config;
	//device_config.ppg_cfg.ppg_tint = MAXM86161_PPG_CFG_TINT_14p8_US; // MAXM86161_PPG_CFG_TINT_29p4_US, MAXM86161_PPG_CFG_TINT_58p7_US, MAXM86161_PPG_CFG_TINT_117p3_US
	//  maxm86161_init_device(device_config);
	maxm86161_led_range_curr_t current;
	current.green = MAXM86161_LED_RANGE_CURRENT_124_MA; // MAXM86161_LED_RANGE_CURRENT_62_MA, MAXM86161_LED_RANGE_CURRENT_93_MA, MAXM86161_LED_RANGE_CURRENT_124_MA
	current.ir = MAXM86161_LED_RANGE_CURRENT_31_MA;
	current.red = MAXM86161_LED_RANGE_CURRENT_31_MA;
	maxm86161_led_range_config(&current);
	maxm86161_shutdown_device(0);

	DataReadPuls();


  return 0;

}

int main(void)
{
	soc_init();

	board_init();

	mdelay(100);

	console_init();

	log_level_set(DEBUG);

	printk(DEBUG, "pulsox_stm32l4r9aii6u\r\n");

	/// Power-On LSM6DSLTR/TMP117/MAXM86161
	LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOH);
	LL_GPIO_SetPinMode(EN_LDO_GPIO_Port, EN_LDO_Pin, LL_GPIO_MODE_OUTPUT);
	LL_GPIO_SetOutputPin(EN_LDO_GPIO_Port, EN_LDO_Pin);
	printk(DEBUG, "EN_LDO set HIGH\r\n");

	i2c2_init();
	printk(DEBUG, "I2C2 controller initialized \r\n");

	maxm86161_test();
//	maxim_heart_rate_and_oxygen_saturation();
	uint32_t i = 0;
	while(1) {
		printk(DEBUG, "pulsox_stm32l4r9aii6u %d\r\n", i++);

		uint8_t chip_addr = 0x40;
		for (;chip_addr < 0x7F; ++chip_addr) {
			mdelay(100);
			uint8_t reg = 0;
			int32_t ret_val = i2c_read(chip_addr << 1, 0x00, &reg, sizeof(reg));
			if (ret_val < 0) {
//				printk(DEBUG, "0x%02X error: %d\r\n",chip_addr << 1, ret_val);
				;
			}
			else {
				printk(DEBUG, "0x%02X value: 0x%X\r\n", chip_addr, reg);
			}
		}

		mdelay(1000);
	}
}
