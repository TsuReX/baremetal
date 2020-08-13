#include "usb_lib.h"
#include "usb_conf.h"
#include "usb_prop.h"
#include "usb_desc.h"
#include "usb_pwr.h"

#define ID1		(0x1FFFF7E8)
#define ID2		(0x1FFFF7EC)
#define ID3		(0x1FFFF7F0)

uint32_t ProtocolValue;
__IO uint8_t EXTI_Enable;
__IO uint8_t Request = 0;
uint8_t Report_Buf[wMaxPacketSize];
uint8_t Buffer[RPT4_COUNT+1];
uint8_t *HID_SetReport_Feature(uint16_t Length);
ErrorStatus HSEStartUpStatus;

DEVICE Device_Table = {
	EP_NUM,
	1
};

DEVICE_PROP property = {
	HID_init,
	HID_Reset,
	HID_Status_In,
	HID_Status_Out,
	hid_setup_with_data_process,
	hid_setup_without_data_process,
	HID_Get_Interface_Setting,
	HID_GetDeviceDescriptor,
	HID_GetConfigDescriptor,
	HID_GetStringDescriptor,
	0x40 /*MAX PACKET SIZE*/
};

USER_STANDARD_REQUESTS standard_requests = {
	NOP_HID_GetConfiguration,
	HID_SetConfiguration,
	NOP_HID_GetInterface,
	NOP_HID_SetInterface,
	NOP_HID_GetStatus,
	NOP_HID_ClearFeature,
	NOP_HID_SetEndPointFeature,
	NOP_HID_SetDeviceFeature,
	HID_SetDeviceAddress
};

ONE_DESCRIPTOR Device_Descriptor = {
	(uint8_t*)rhid_device_descriptor,
	RHID_SIZ_DEVICE_DESC
};

ONE_DESCRIPTOR Config_Descriptor = {
	(uint8_t*)rhid_configuration_descriptor,
	RHID_SIZ_CONFIG_DESC
};

ONE_DESCRIPTOR RHID_Report_Descriptor = {
	(uint8_t *)rhid_report_descriptor,
	RHID_SIZ_REPORT_DESC
};

ONE_DESCRIPTOR RHID_Hid_Descriptor = {
	(uint8_t*)rhid_configuration_descriptor + RHID_OFF_HID_DESC,
	RHID_SIZ_HID_DESC
};

ONE_DESCRIPTOR String_Descriptor[4] = {
	{(uint8_t*)rhid_string_lang_id, RHID_SIZ_STRING_LANGID},
	{(uint8_t*)rhid_string_vendor, RHID_SIZ_STRING_VENDOR},
	{(uint8_t*)rhid_string_product, RHID_SIZ_STRING_PRODUCT},
	{(uint8_t*)rhid_string_serial, RHID_SIZ_STRING_SERIAL}
};

/*******************************************************************************
* Function Name  : Enter_LowPowerMode.
* Description    : Power-off system clocks and power while entering suspend mode.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void Enter_LowPowerMode(void)
{
	/* Set the device state to suspend */
	bDeviceState = SUSPENDED;

	/* Clear EXTI Line18 pending bit */
	//  EXTI_ClearITPendingBit(KEY_BUTTON_EXTI_LINE);

	/* Request to enter STOP mode with regulator in low power mode */
	//  PWR_EnterSTOPMode(PWR_Regulator_LowPower, PWR_STOPEntry_WFI);
}

/*******************************************************************************
* Function Name  : HexToChar.
* Description    : Convert Hex 32Bits value into char.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
static void IntToUnicode (uint32_t value , uint8_t *pbuf , uint8_t len)
{
  uint8_t idx = 0;

	for (idx = 0 ; idx < len ; idx ++) {

		if( ((value >> 28)) < 0xA ) {
			pbuf[2 * idx] = (value >> 28) + '0';

		} else {
			pbuf[2 * idx] = (value >> 28) + 'A' - 10;
		}

		value = value << 4;
		pbuf[2 * idx + 1] = 0;
	}
}

void Get_SerialNum(void)
{
	uint32_t Device_Serial0, Device_Serial1, Device_Serial2;

	Device_Serial0 = *(uint32_t*)ID1;
	Device_Serial1 = *(uint32_t*)ID2;
	Device_Serial2 = *(uint32_t*)ID3;

	Device_Serial0 += Device_Serial2;

	if (Device_Serial0 != 0) {
		IntToUnicode (Device_Serial0, &rhid_string_serial[2] , 8);
		IntToUnicode (Device_Serial1, &rhid_string_serial[18], 4);
	}
}
/*******************************************************************************
* Function Name  : HID_init.
* Description    : HID init routine.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void HID_init(void)
{

	/* Update the serial number string descriptor with the data from the unique
	ID*/
	Get_SerialNum();

	usb_device_info->Current_Configuration = 0;
	/* Connect the device */
	PowerOn();

	/* Perform basic device initialization operations */
	USB_SIL_Init();

	bDeviceState = UNCONNECTED;
}

/*******************************************************************************
* Function Name  : HID_Reset.
* Description    : HID reset routine.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void HID_Reset(void)
{
	/* Set HID_DEVICE as not configured */
	usb_device_info->Current_Configuration = 0;
	usb_device_info->Current_Interface = 0;/*the default Interface*/

	/* Current Feature initialization */
	usb_device_info->Current_Feature = rhid_configuration_descriptor[7];
	SetBTABLE(BTABLE_ADDRESS);
	/* Initialize Endpoint 0 */
	SetEPType(ENDP0, EP_CONTROL);
	SetEPTxStatus(ENDP0, EP_TX_STALL);
	SetEPRxAddr(ENDP0, ENDP0_RXADDR);
	SetEPTxAddr(ENDP0, ENDP0_TXADDR);
	Clear_Status_Out(ENDP0);
	SetEPRxCount(ENDP0, property.MaxPacketSize);
	SetEPRxValid(ENDP0);

	/* Initialize Endpoint 1 */
	SetEPType(ENDP1, EP_INTERRUPT);
	SetEPTxAddr(ENDP1, ENDP1_TXADDR);
	SetEPRxAddr(ENDP1, ENDP1_RXADDR);
	SetEPTxCount(ENDP1, EP1TxCount);
	SetEPRxCount(ENDP1, EP1RxCount);
	SetEPRxStatus(ENDP1, EP_RX_VALID);
	SetEPTxStatus(ENDP1, EP_TX_NAK);

	/* Set this device to response on default address */
	SetDeviceAddress(0);
	bDeviceState = ATTACHED;
}
/*******************************************************************************
* Function Name  : HID_SetConfiguration.
* Description    : Update the device state to configured.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void HID_SetConfiguration(void)
{
	DEVICE_INFO *pInfo = &device_info;

	if (pInfo->Current_Configuration != 0) {
		/* Device configured */
		bDeviceState = CONFIGURED;
	}
}
/*******************************************************************************
* Function Name  : HID_SetConfiguration.
* Description    : Update the device state to addressed.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void HID_SetDeviceAddress (void)
{
	bDeviceState = ADDRESSED;
}
/*******************************************************************************
* Function Name  : HID_Status_In.
* Description    : HID status IN routine.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void HID_Status_In(void)
{
//  BitAction Led_State;
//
//  if (Report_Buf[1] == 0)
//  {
//    Led_State = Bit_RESET;
//  }
//  else
//  {
//    Led_State = Bit_SET;
//  }
//
//  switch (Report_Buf[0])
//  {
//    case 1: /* Led 1 */
//     if (Led_State != Bit_RESET)
//     {
//       GPIO_SetBits(LED_PORT,LED1_PIN);
//     }
//     else
//     {
//       GPIO_ResetBits(LED_PORT,LED1_PIN);
//     }
//     break;
//    case 2: /* Led 2 */
//     if (Led_State != Bit_RESET)
//     {
//       GPIO_SetBits(LED_PORT,LED2_PIN);
//     }
//     else
//     {
//       GPIO_ResetBits(LED_PORT,LED2_PIN);
//     }
//      break;
//    case 3: /* Led 1&2 */
//       Buffer[4]=Report_Buf[1];
//     break;
//  }
}

/*******************************************************************************
* Function Name  : HID_Status_Out
* Description    : HID status OUT routine.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void HID_Status_Out (void)
{}

/*******************************************************************************
* Function Name  : HID_Data_Setup
* Description    : Handle the data class specific requests.
* Input          : Request Nb.
* Output         : None.
* Return         : USB_UNSUPPORT or USB_SUCCESS.
*******************************************************************************/
RESULT hid_setup_with_data_process(uint8_t RequestNo)
{
	uint8_t *(*CopyRoutine)(uint16_t);

	CopyRoutine = NULL;
	if ((RequestNo == GET_DESCRIPTOR) &&
		((usb_device_info->bm_request_type & (REQUEST_TYPE | REQUEST_RECIPIENT)) == INTERFACE_RECIPIENT) &&
		((usb_device_info->w_index & 0xFF) == 0)) {

		if ((usb_device_info->w_value >> 8) == REPORT_DESCRIPTOR) {
			CopyRoutine = HID_GetReportDescriptor;

		} else if ((usb_device_info->w_value >> 8) == HID_DESCRIPTOR_TYPE) {
			CopyRoutine = HID_GetHIDDescriptor;
		}

	}
	/*** GET_PROTOCOL, GET_REPORT, SET_REPORT ***/
	else if (((usb_device_info->bm_request_type & (REQUEST_TYPE | REQUEST_RECIPIENT))  == (CLASS_REQUEST_TYPE | INTERFACE_RECIPIENT)) ) {
		switch( RequestNo ) {
			case GET_PROTOCOL:
				CopyRoutine = HID_GetProtocolValue;
				break;

			case SET_REPORT:
				CopyRoutine = HID_SetReport_Feature;
				Request = SET_REPORT;
				break;
			default:
				break;
		}
	}

	if (CopyRoutine == NULL) {
		return USB_UNSUPPORT;
	}

	usb_device_info->ep0_ctrl_info.data_copy = CopyRoutine;
	usb_device_info->ep0_ctrl_info.data_buffer_offset = 0;

	(*CopyRoutine)(0);

	return USB_SUCCESS;
}


/*******************************************************************************
* Function Name  : HID_NoData_Setup
* Description    : handle the no data class specific requests
* Input          : Request Nb.
* Output         : None.
* Return         : USB_UNSUPPORT or USB_SUCCESS.
*******************************************************************************/
RESULT hid_setup_without_data_process(uint8_t RequestNo)
{
	if (((usb_device_info->bm_request_type & (REQUEST_TYPE | REQUEST_RECIPIENT)) == (CLASS_REQUEST_TYPE | INTERFACE_RECIPIENT)) &&
		(RequestNo == SET_PROTOCOL)) {

		return HID_SetProtocol();
	} else {
		return USB_UNSUPPORT;
	}
}

/*******************************************************************************
* Function Name  : HID_SetReport_Feature
* Description    : Set Feature request handling
* Input          : Length.
* Output         : None.
* Return         : Buffer
*******************************************************************************/
uint8_t *HID_SetReport_Feature(uint16_t Length)
{
	if (Length == 0) {
		usb_device_info->ep0_ctrl_info.remaining_data_size = wMaxPacketSize;
		return NULL;

	} else {
		return Report_Buf;
	}
}

/*******************************************************************************
* Function Name  : HID_GetDeviceDescriptor.
* Description    : Gets the device descriptor.
* Input          : Length
* Output         : None.
* Return         : The address of the device descriptor.
*******************************************************************************/
uint8_t *HID_GetDeviceDescriptor(uint16_t Length)
{
	return Standard_GetDescriptorData(Length, &Device_Descriptor);
}

/*******************************************************************************
* Function Name  : HID_GetConfigDescriptor.
* Description    : Gets the configuration descriptor.
* Input          : Length
* Output         : None.
* Return         : The address of the configuration descriptor.
*******************************************************************************/
uint8_t *HID_GetConfigDescriptor(uint16_t Length)
{
	return Standard_GetDescriptorData(Length, &Config_Descriptor);
}

/*******************************************************************************
* Function Name  : HID_GetStringDescriptor
* Description    : Gets the string descriptors according to the needed index
* Input          : Length
* Output         : None.
* Return         : The address of the string descriptors.
*******************************************************************************/
uint8_t *HID_GetStringDescriptor(uint16_t Length)
{
	uint8_t wValue0 = (usb_device_info->w_value & 0xFF);
	if (wValue0 > 4) {
		return NULL;

	} else {
		return Standard_GetDescriptorData(Length, &String_Descriptor[wValue0]);
	}
}

/*******************************************************************************
* Function Name  : HID_GetReportDescriptor.
* Description    : Gets the HID report descriptor.
* Input          : Length
* Output         : None.
* Return         : The address of the configuration descriptor.
*******************************************************************************/
uint8_t *HID_GetReportDescriptor(uint16_t Length)
{
	return Standard_GetDescriptorData(Length, &RHID_Report_Descriptor);
}

/*******************************************************************************
* Function Name  : HID_GetHIDDescriptor.
* Description    : Gets the HID descriptor.
* Input          : Length
* Output         : None.
* Return         : The address of the configuration descriptor.
*******************************************************************************/
uint8_t *HID_GetHIDDescriptor(uint16_t Length)
{
	return Standard_GetDescriptorData(Length, &RHID_Hid_Descriptor);
}

/*******************************************************************************
* Function Name  : HID_Get_Interface_Setting.
* Description    : tests the interface and the alternate setting according to the
*                  supported one.
* Input          : - Interface : interface number.
*                  - AlternateSetting : Alternate Setting number.
* Output         : None.
* Return         : USB_SUCCESS or USB_UNSUPPORT.
*******************************************************************************/
RESULT HID_Get_Interface_Setting(uint8_t Interface, uint8_t AlternateSetting)
{
	if (AlternateSetting > 0) {
		return USB_UNSUPPORT;

	} else if (Interface > 0) {
		return USB_UNSUPPORT;
	}

	return USB_SUCCESS;
}

/*******************************************************************************
* Function Name  : HID_SetProtocol
* Description    : HID Set Protocol request routine.
* Input          : None.
* Output         : None.
* Return         : USB SUCCESS.
*******************************************************************************/
RESULT HID_SetProtocol(void)
{
	uint8_t wValue0 = (usb_device_info->w_value & 0xFF);
	ProtocolValue = wValue0;

	return USB_SUCCESS;
}

/*******************************************************************************
* Function Name  : HID_GetProtocolValue
* Description    : get the protocol value
* Input          : Length.
* Output         : None.
* Return         : address of the protocol value.
*******************************************************************************/
uint8_t *HID_GetProtocolValue(uint16_t Length)
{
	if (Length == 0) {
		usb_device_info->ep0_ctrl_info.remaining_data_size = 1;
		return NULL;

	} else {
		return (uint8_t *)(&ProtocolValue);
	}
}
