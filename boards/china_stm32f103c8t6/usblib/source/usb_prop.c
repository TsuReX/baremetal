#include "usb_prop.h"
#include "usb_core.h"
#include "usb_desc.h"
#include "console.h"

#include <stddef.h>

#define ID1		(0x1FFFF7E8)
#define ID2		(0x1FFFF7EC)
#define ID3		(0x1FFFF7F0)

extern DEVICE_INFO *usb_device_info;

uint32_t ProtocolValue;
__IO uint8_t EXTI_Enable;
__IO uint8_t Request = 0;
uint8_t Report_Buf[EP0_MAX_PACKET_SIZE];
uint8_t Buffer[RPT4_COUNT+1];
uint8_t *HID_SetReport_Feature(uint16_t Length);
ErrorStatus HSEStartUpStatus;

#define ValBit(VAR,Place)    (VAR & (1 << Place))
#define SetBit(VAR,Place)    (VAR |= (1 << Place))
#define ClrBit(VAR,Place)    (VAR &= ((1 << Place) ^ 255))

uint16_t device_status;
uint16_t interface_status;
uint16_t endpoint_status;

extern DEVICE_INFO *usb_device_info;
extern USER_STANDARD_REQUESTS  *usb_standard_requests;
extern DEVICE_PROP *usb_device_property;

const uint8_t rhid_device_qualifier[0xA] = {
	0xA,
	0x6,		/*< bDescriptorType */
	0x00,0x02,	/*< bcdUSB */
	0x00,		/*< bDeviceClass */
	0x00,		/*< bDeviceSubClass */
	0x00,		/*< bDeviceProtocol */
	0x40,		/*< bMaxPacketSize */
	0x00,		/*< bNumConfigurations */
	0x00		/*< bReserved */
};

DEVICE Device_Table = {
	EP_COUNT,
	1
};

DEVICE_PROP property = {
	init,
	reset,
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
* Function Name  : Standard_GetConfiguration.
* Description    : Return the current configuration variable address.
* Input          : Length - How many bytes are needed.
* Output         : None.
* Return         : Return 1 , if the request is invalid when "Length" is 0.
*                  Return "Buffer" if the "Length" is not 0.
*******************************************************************************/
uint8_t *Standard_GetConfiguration(uint16_t Length)
{
  if (Length == 0)
  {
    usb_device_info->ep0_ctrl_info.remaining_data_size =
      sizeof(usb_device_info->Current_Configuration);
    return 0;
  }
  usb_standard_requests->User_GetConfiguration();
  return (uint8_t *)&usb_device_info->Current_Configuration;
}

/*******************************************************************************
* Function Name  : Standard_SetConfiguration.
* Description    : This routine is called to set the configuration value
*                  Then each class should configure device itself.
* Input          : None.
* Output         : None.
* Return         : Return USB_SUCCESS, if the request is performed.
*                  Return USB_UNSUPPORT, if the request is invalid.
*******************************************************************************/
RESULT Standard_SetConfiguration(void)
{

  if (((usb_device_info->w_value & 0xFF) <=
      Device_Table.Total_Configuration) && ((usb_device_info->w_value >> 8) == 0)
      && (usb_device_info->w_index == 0)) /*call Back usb spec 2.0*/
  {
    usb_device_info->Current_Configuration = usb_device_info->w_value & 0xFF;
    usb_standard_requests->User_SetConfiguration();
    return USB_SUCCESS;
  }
  else
  {
    return USB_UNSUPPORT;
  }
}

/*******************************************************************************
* Function Name  : Standard_GetInterface.
* Description    : Return the Alternate Setting of the current interface.
* Input          : Length - How many bytes are needed.
* Output         : None.
* Return         : Return 0, if the request is invalid when "Length" is 0.
*                  Return "Buffer" if the "Length" is not 0.
*******************************************************************************/
uint8_t *Standard_GetInterface(uint16_t Length)
{
  if (Length == 0)
  {
    usb_device_info->ep0_ctrl_info.remaining_data_size =
      sizeof(usb_device_info->Current_AlternateSetting);
    return 0;
  }
  usb_standard_requests->User_GetInterface();
  return (uint8_t *)&usb_device_info->Current_AlternateSetting;
}

/*******************************************************************************
* Function Name  : Standard_SetInterface.
* Description    : This routine is called to set the interface.
*                  Then each class should configure the interface them self.
* Input          : None.
* Output         : None.
* Return         : - Return USB_SUCCESS, if the request is performed.
*                  - Return USB_UNSUPPORT, if the request is invalid.
*******************************************************************************/
RESULT Standard_SetInterface(void)
{
  RESULT Re;
  /*Test if the specified Interface and Alternate Setting are supported by
    the application Firmware*/
  Re = (*usb_device_property->Class_Get_Interface_Setting)(usb_device_info->w_index & 0xFF, usb_device_info->w_value & 0xFF);

  if (usb_device_info->Current_Configuration != 0)
  {
    if ((Re != USB_SUCCESS) ||
    	((usb_device_info->w_index >> 8) != 0) ||
		((usb_device_info->w_value >> 8) != 0))
    {
      return  USB_UNSUPPORT;
    }
    else if (Re == USB_SUCCESS)
    {
      usb_standard_requests->User_SetInterface();
      usb_device_info->Current_Interface = usb_device_info->w_index & 0xFF;
      usb_device_info->Current_AlternateSetting = usb_device_info->w_value & 0xFF;
      return USB_SUCCESS;
    }

  }

  return USB_UNSUPPORT;
}

/*******************************************************************************
* Function Name  : Standard_GetStatus.
* Description    : Copy the device request data to "StatusInfo buffer".
* Input          : - Length - How many bytes are needed.
* Output         : None.
* Return         : Return 0, if the request is at end of data block,
*                  or is invalid when "Length" is 0.
*******************************************************************************/
uint8_t *Standard_GetStatus(uint16_t Length)
{
	d_print("%s()\r\n",  __func__);
	uint8_t	request_recipient = usb_device_info->bm_request_type & REQUEST_RECIPIENT;

	if (Length == 0) {
		usb_device_info->ep0_ctrl_info.remaining_data_size = 2;
		return 0;
	}

/*Get Device Status */
	if (request_recipient ==  DEVICE_RECIPIENT_TYPE) {
		uint8_t Feature = usb_device_info->Current_Feature;

		/* Remote Wakeup enabled */
		if (ValBit(Feature, 5)) {
			SetBit(device_status, 1);
		} else {
			ClrBit(device_status, 1);
		}

		/* Bus-powered */
		if (ValBit(Feature, 6)) {
			SetBit(device_status, 0);
		} else /* Self-powered */  {
			ClrBit(device_status, 0);
		}
		usb_standard_requests->User_GetStatus();
		return (uint8_t *)&device_status;


/*Interface Status*/
	} else if (request_recipient == INTERFACE_RECIPIENT_TYPE) {
		usb_standard_requests->User_GetStatus();
		return (uint8_t *)&interface_status;


/*Get EndPoint Status*/
	} else if (request_recipient == ENDPOINT_RECIPIENT_TYPE) {
		uint8_t Related_Endpoint;
		uint8_t wIndex0 = usb_device_info->w_index & 0xFF;

		Related_Endpoint = (wIndex0 & 0x0f);
		if (ValBit(wIndex0, 7)) {
			/* IN endpoint */
			if (_GetTxStallStatus(Related_Endpoint)) {
				SetBit(endpoint_status, 0); /* IN Endpoint stalled */
			}
		} else {

			/* OUT endpoint */
			if (_GetRxStallStatus(Related_Endpoint)) {
				SetBit(endpoint_status, 0); /* OUT Endpoint stalled */
			}
		}

		usb_standard_requests->User_GetStatus();
		return (uint8_t *)&endpoint_status;
	}

	return NULL;
}

/*******************************************************************************
* Function Name  : Standard_ClearFeature.
* Description    : Clear or disable a specific feature.
* Input          : None.
* Output         : None.
* Return         : - Return USB_SUCCESS, if the request is performed.
*                  - Return USB_UNSUPPORT, if the request is invalid.
*******************************************************************************/
RESULT Standard_ClearFeature(void)
{
  uint32_t     Type_Rec = (usb_device_info->bm_request_type & (REQUEST_TYPE | REQUEST_RECIPIENT));
  uint32_t     Status;


  if (Type_Rec == (STANDARD_REQUEST_TYPE | DEVICE_RECIPIENT))
  {/*Device Clear Feature*/
    ClrBit(usb_device_info->Current_Feature, 5);
    return USB_SUCCESS;
  }
  else if (Type_Rec == (STANDARD_REQUEST_TYPE | ENDPOINT_RECIPIENT))
  {/*EndPoint Clear Feature*/
    DEVICE* pDev;
    uint32_t Related_Endpoint;
    uint32_t wIndex0;
    uint32_t rEP;

    if ((usb_device_info->w_value != ENDPOINT_STALL)
        || ((usb_device_info->w_index >> 8) != 0))
    {
      return USB_UNSUPPORT;
    }

    pDev = &Device_Table;
    wIndex0 = usb_device_info->w_index & 0xFF;
    rEP = wIndex0 & ~0x80;
    Related_Endpoint = ENDP0 + rEP;

    if (ValBit(usb_device_info->w_index & 0xFF, 7))
    {
      /*Get Status of endpoint & stall the request if the related_ENdpoint
      is Disabled*/
      Status = _GetEPTxStatus(Related_Endpoint);
    }
    else
    {
      Status = _GetEPRxStatus(Related_Endpoint);
    }

    if ((rEP >= pDev->Total_Endpoint) || (Status == 0)
        || (usb_device_info->Current_Configuration == 0))
    {
      return USB_UNSUPPORT;
    }


    if (wIndex0 & 0x80)
    {
      /* IN endpoint */
      if (_GetTxStallStatus(Related_Endpoint ))
      {
        _ClearDTOG_TX(Related_Endpoint);
        _SetEPTxStatus(Related_Endpoint, EP_TX_VALID);
      }
    }
    else
    {
      /* OUT endpoint */
      if (_GetRxStallStatus(Related_Endpoint))
      {
        if (Related_Endpoint == ENDP0)
        {
          /* After clear the STALL, enable the default endpoint receiver */
          _SetEPRxCount(Related_Endpoint, property.MaxPacketSize);
          _SetEPRxStatus(Related_Endpoint, EP_RX_VALID);
        }
        else
        {
          _ClearDTOG_RX(Related_Endpoint);
          _SetEPRxStatus(Related_Endpoint, EP_RX_VALID);
        }
      }
    }
    usb_standard_requests->User_ClearFeature();
    return USB_SUCCESS;
  }

  return USB_UNSUPPORT;
}

/*******************************************************************************
* Function Name  : Standard_SetEndPointFeature
* Description    : Set or enable a specific feature of EndPoint
* Input          : None.
* Output         : None.
* Return         : - Return USB_SUCCESS, if the request is performed.
*                  - Return USB_UNSUPPORT, if the request is invalid.
*******************************************************************************/
RESULT Standard_SetEndPointFeature(void)
{
  uint32_t    wIndex0;
  uint32_t    Related_Endpoint;
  uint32_t    rEP;
  uint32_t    Status;

  wIndex0 = usb_device_info->w_index & 0xFF;
  rEP = wIndex0 & ~0x80;
  Related_Endpoint = ENDP0 + rEP;

  if (ValBit(usb_device_info->w_index & 0xFF, 7))
  {
    /* get Status of endpoint & stall the request if the related_ENdpoint
    is Disabled*/
    Status = _GetEPTxStatus(Related_Endpoint);
  }
  else
  {
    Status = _GetEPRxStatus(Related_Endpoint);
  }

  if (Related_Endpoint >= Device_Table.Total_Endpoint
      || usb_device_info->w_value != 0 || Status == 0
      || usb_device_info->Current_Configuration == 0)
  {
    return USB_UNSUPPORT;
  }
  else
  {
    if (wIndex0 & 0x80)
    {
      /* IN endpoint */
      _SetEPTxStatus(Related_Endpoint, EP_TX_STALL);
    }

    else
    {
      /* OUT endpoint */
      _SetEPRxStatus(Related_Endpoint, EP_RX_STALL);
    }
  }
  usb_standard_requests->User_SetEndPointFeature();
  return USB_SUCCESS;
}

/*******************************************************************************
* Function Name  : Standard_SetDeviceFeature.
* Description    : Set or enable a specific feature of Device.
* Input          : None.
* Output         : None.
* Return         : - Return USB_SUCCESS, if the request is performed.
*                  - Return USB_UNSUPPORT, if the request is invalid.
*******************************************************************************/
RESULT Standard_SetDeviceFeature(void)
{
  SetBit(usb_device_info->Current_Feature, 5);
  usb_standard_requests->User_SetDeviceFeature();
  return USB_SUCCESS;
}

/*******************************************************************************
* Function Name  : Standard_GetDescriptorData.
* Description    : Standard_GetDescriptorData is used for descriptors transfer.
*                : This routine is used for the descriptors resident in Flash
*                  or RAM
*                  pDesc can be in either Flash or RAM
*                  The purpose of this routine is to have a versatile way to
*                  response descriptors request. It allows user to generate
*                  certain descriptors with software or read descriptors from
*                  external storage part by part.
* Input          : - Length - Length of the data in this transfer.
*                  - pDesc - A pointer points to descriptor struct.
*                  The structure gives the initial address of the descriptor and
*                  its original size.
* Output         : None.
* Return         : Address of a part of the descriptor pointed by the Usb_
*                  wOffset The buffer pointed by this address contains at least
*                  Length bytes.
*******************************************************************************/
uint8_t *Standard_GetDescriptorData(uint16_t Length, ONE_DESCRIPTOR *pDesc)
{
	uint32_t  wOffset;

	wOffset = usb_device_info->ep0_ctrl_info.data_buffer_offset;
	if (Length == 0) {
		usb_device_info->ep0_ctrl_info.remaining_data_size = pDesc->Descriptor_Size - wOffset;
//		d_print("Standard_GetDescriptorData 1\r\n");
		return 0;
	}
//	d_print("Standard_GetDescriptorData size:0x%04X\r\n", Length);
	return pDesc->Descriptor + wOffset;
}

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
	d_print("%s()\r\n",  __func__);
}

/*******************************************************************************
* Function Name  : HID_Status_Out
* Description    : HID status OUT routine.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void HID_Status_Out (void)
{
	d_print("%s()\r\n",  __func__);
}

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
//			d_print("GET_REPORT_DESCRIPTOR\r\n");
			CopyRoutine = HID_GetReportDescriptor;

		} else if ((usb_device_info->w_value >> 8) == HID_DESCRIPTOR_TYPE) {
//			d_print("GET_HID_DESCRIPTOR\r\n");
			CopyRoutine = HID_GetHIDDescriptor;
		}

	}
	/*** GET_PROTOCOL, GET_REPORT, SET_REPORT ***/
	else if (((usb_device_info->bm_request_type & (REQUEST_TYPE | REQUEST_RECIPIENT))  == (CLASS_REQUEST_TYPE | INTERFACE_RECIPIENT)) ) {
		switch( RequestNo ) {
			case GET_PROTOCOL:
//				d_print("GET_PROTOCOL\r\n");
				CopyRoutine = HID_GetProtocolValue;
				break;

			case SET_REPORT:
//				d_print("SET_REPORT\r\n");
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
//		d_print("SET_PROTOCOL\r\n");
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
		usb_device_info->ep0_ctrl_info.remaining_data_size = EP0_MAX_PACKET_SIZE;
		return NULL;

	} else {
		return Report_Buf;
	}
}

uint8_t *HID_GetDeviceDescriptorQualifier(uint16_t Length)
{
	uint32_t  wOffset;

	wOffset = usb_device_info->ep0_ctrl_info.data_buffer_offset;
	if (Length == 0) {
		usb_device_info->ep0_ctrl_info.remaining_data_size = rhid_device_qualifier[0] - wOffset;

		return 0;
	}
	return (uint8_t *)rhid_device_qualifier + wOffset;
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
//		d_print("HID_GetStringDescriptor 1\r\n");
		return NULL;

	} else {
//		d_print("HID_GetStringDescriptor 2\r\n");
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
