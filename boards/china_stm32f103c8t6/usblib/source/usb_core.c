#include "usb_lib.h"
#include "stddef.h"

#define ValBit(VAR,Place)    (VAR & (1 << Place))
#define SetBit(VAR,Place)    (VAR |= (1 << Place))
#define ClrBit(VAR,Place)    (VAR &= ((1 << Place) ^ 255))

#define StatusInfo0 StatusInfo.bw.bb1 /* Reverse bb0 & bb1 */
#define StatusInfo1 StatusInfo.bw.bb0

uint16_t_uint8_t StatusInfo;

bool multiple_max_packet_size = FALSE;

static void ep0_data_stage_out_process(void);
static void ep0_data_stage_in_process(void);
static void setup_without_data_process(void);
static void setup_with_data_process(void);

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
    pInformation->ep0_ctrl_info.remaining_data_size =
      sizeof(pInformation->Current_Configuration);
    return 0;
  }
  pUser_Standard_Requests->User_GetConfiguration();
  return (uint8_t *)&pInformation->Current_Configuration;
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

  if ((pInformation->USBwValue0 <=
      Device_Table.Total_Configuration) && (pInformation->USBwValue1 == 0)
      && (pInformation->USBwIndex == 0)) /*call Back usb spec 2.0*/
  {
    pInformation->Current_Configuration = pInformation->USBwValue0;
    pUser_Standard_Requests->User_SetConfiguration();
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
    pInformation->ep0_ctrl_info.remaining_data_size =
      sizeof(pInformation->Current_AlternateSetting);
    return 0;
  }
  pUser_Standard_Requests->User_GetInterface();
  return (uint8_t *)&pInformation->Current_AlternateSetting;
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
  Re = (*pProperty->Class_Get_Interface_Setting)(pInformation->USBwIndex0, pInformation->USBwValue0);

  if (pInformation->Current_Configuration != 0)
  {
    if ((Re != USB_SUCCESS) || (pInformation->USBwIndex1 != 0)
        || (pInformation->USBwValue1 != 0))
    {
      return  USB_UNSUPPORT;
    }
    else if (Re == USB_SUCCESS)
    {
      pUser_Standard_Requests->User_SetInterface();
      pInformation->Current_Interface = pInformation->USBwIndex0;
      pInformation->Current_AlternateSetting = pInformation->USBwValue0;
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
  if (Length == 0)
  {
    pInformation->ep0_ctrl_info.remaining_data_size = 2;
    return 0;
  }

  /* Reset Status Information */
  StatusInfo.w = 0;

  if (Type_Recipient == (STANDARD_REQUEST | DEVICE_RECIPIENT))
  {
    /*Get Device Status */
    uint8_t Feature = pInformation->Current_Feature;

    /* Remote Wakeup enabled */
    if (ValBit(Feature, 5))
    {
      SetBit(StatusInfo0, 1);
    }
    else
    {
      ClrBit(StatusInfo0, 1);
    }      

    /* Bus-powered */
    if (ValBit(Feature, 6))
    {
      SetBit(StatusInfo0, 0);
    }
    else /* Self-powered */
    {
      ClrBit(StatusInfo0, 0);
    }
  }
  /*Interface Status*/
  else if (Type_Recipient == (STANDARD_REQUEST | INTERFACE_RECIPIENT))
  {
    return (uint8_t *)&StatusInfo;
  }
  /*Get EndPoint Status*/
  else if (Type_Recipient == (STANDARD_REQUEST | ENDPOINT_RECIPIENT))
  {
    uint8_t Related_Endpoint;
    uint8_t wIndex0 = pInformation->USBwIndex0;

    Related_Endpoint = (wIndex0 & 0x0f);
    if (ValBit(wIndex0, 7))
    {
      /* IN endpoint */
      if (_GetTxStallStatus(Related_Endpoint))
      {
        SetBit(StatusInfo0, 0); /* IN Endpoint stalled */
      }
    }
    else
    {
      /* OUT endpoint */
      if (_GetRxStallStatus(Related_Endpoint))
      {
        SetBit(StatusInfo0, 0); /* OUT Endpoint stalled */
      }
    }

  }
  else
  {
    return NULL;
  }
  pUser_Standard_Requests->User_GetStatus();
  return (uint8_t *)&StatusInfo;
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
  uint32_t     Type_Rec = Type_Recipient;
  uint32_t     Status;


  if (Type_Rec == (STANDARD_REQUEST | DEVICE_RECIPIENT))
  {/*Device Clear Feature*/
    ClrBit(pInformation->Current_Feature, 5);
    return USB_SUCCESS;
  }
  else if (Type_Rec == (STANDARD_REQUEST | ENDPOINT_RECIPIENT))
  {/*EndPoint Clear Feature*/
    DEVICE* pDev;
    uint32_t Related_Endpoint;
    uint32_t wIndex0;
    uint32_t rEP;

    if ((pInformation->USBwValue != ENDPOINT_STALL)
        || (pInformation->USBwIndex1 != 0))
    {
      return USB_UNSUPPORT;
    }

    pDev = &Device_Table;
    wIndex0 = pInformation->USBwIndex0;
    rEP = wIndex0 & ~0x80;
    Related_Endpoint = ENDP0 + rEP;

    if (ValBit(pInformation->USBwIndex0, 7))
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
        || (pInformation->Current_Configuration == 0))
    {
      return USB_UNSUPPORT;
    }


    if (wIndex0 & 0x80)
    {
      /* IN endpoint */
      if (_GetTxStallStatus(Related_Endpoint ))
      {
        ClearDTOG_TX(Related_Endpoint);
        SetEPTxStatus(Related_Endpoint, EP_TX_VALID);
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
          SetEPRxCount(Related_Endpoint, Device_Property.MaxPacketSize);
          _SetEPRxStatus(Related_Endpoint, EP_RX_VALID);
        }
        else
        {
          ClearDTOG_RX(Related_Endpoint);
          _SetEPRxStatus(Related_Endpoint, EP_RX_VALID);
        }
      }
    }
    pUser_Standard_Requests->User_ClearFeature();
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

  wIndex0 = pInformation->USBwIndex0;
  rEP = wIndex0 & ~0x80;
  Related_Endpoint = ENDP0 + rEP;

  if (ValBit(pInformation->USBwIndex0, 7))
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
      || pInformation->USBwValue != 0 || Status == 0
      || pInformation->Current_Configuration == 0)
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
  pUser_Standard_Requests->User_SetEndPointFeature();
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
  SetBit(pInformation->Current_Feature, 5);
  pUser_Standard_Requests->User_SetDeviceFeature();
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

  wOffset = pInformation->ep0_ctrl_info.data_buffer_offset;
  if (Length == 0)
  {
    pInformation->ep0_ctrl_info.remaining_data_size = pDesc->Descriptor_Size - wOffset;
    return 0;
  }

  return pDesc->Descriptor + wOffset;
}

/*******************************************************************************
* Function Name  : DataStageOut.
* Description    : Data stage of a Control Write Transfer.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void ep0_data_stage_out_process(void)
{
	ENDPOINT_INFO	*ep0_ctrl_info = &pInformation->ep0_ctrl_info;
	uint8_t 		*data_buffer;
	size_t			single_transfer_size;

	if (ep0_ctrl_info->data_copy && ep0_ctrl_info->remaining_data_size) {

		single_transfer_size = ep0_ctrl_info->single_transfer_size;

		if (single_transfer_size > ep0_ctrl_info->remaining_data_size) {
			single_transfer_size = ep0_ctrl_info->remaining_data_size;
		}

		data_buffer = (*ep0_ctrl_info->data_copy)(single_transfer_size);

		ep0_ctrl_info->remaining_data_size -= single_transfer_size;
		ep0_ctrl_info->data_buffer_offset += single_transfer_size;

		copy_from_usb(data_buffer, _GetEPRxAddr(ENDP0), single_transfer_size);
	}

	if (ep0_ctrl_info->remaining_data_size != 0) {
		/* re-enable for next data reception */
		ep0_rx_state = EP_RX_VALID;

		_SetEPTxCount(ENDP0, 0);

		/* Expect the host to abort the data OUT stage */
		ep0_tx_state = EP_TX_VALID;
	}

	/* Set the next State*/
	if (ep0_ctrl_info->remaining_data_size >= ep0_ctrl_info->single_transfer_size) {
		pInformation->control_state = OUT_DATA;

	} else {

		if (ep0_ctrl_info->remaining_data_size > 0) {
			pInformation->control_state = LAST_OUT_DATA;

		} else if (ep0_ctrl_info->remaining_data_size == 0) {
			pInformation->control_state = WAIT_STATUS_IN;
			_SetEPTxCount(ENDP0, 0);
			ep0_tx_state = EP_TX_VALID;
		}
	}
}

/*******************************************************************************
* Function Name  : DataStageIn.
* Description    : Data stage of a Control Read Transfer.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void ep0_data_stage_in_process(void)
{
	ENDPOINT_INFO	*ep0_ctrl_info = &pInformation->ep0_ctrl_info;
	uint8_t			*data_buffer;
	size_t			single_transfer_size;

	if ((ep0_ctrl_info->remaining_data_size == 0) && (pInformation->control_state == LAST_IN_DATA)) {

		if(multiple_max_packet_size == TRUE) {
			/* No more data to send and empty packet */
			_SetEPTxCount(ENDP0, 0);
			ep0_tx_state = EP_TX_VALID;

			pInformation->control_state = LAST_IN_DATA;
			multiple_max_packet_size = FALSE;

		} else  {
			/* No more data to send so STALL the TX Status*/
			pInformation->control_state = WAIT_STATUS_OUT;
			ep0_tx_state = EP_TX_STALL;
		}

		return;
	}

	single_transfer_size = ep0_ctrl_info->single_transfer_size;

	pInformation->control_state = (ep0_ctrl_info->remaining_data_size <= single_transfer_size) ? LAST_IN_DATA : IN_DATA;

	if (single_transfer_size > ep0_ctrl_info->remaining_data_size) {
		single_transfer_size = ep0_ctrl_info->remaining_data_size;
	}

	data_buffer = (*ep0_ctrl_info->data_copy)(single_transfer_size);

	copy_to_usb(data_buffer, _GetEPTxAddr(ENDP0), single_transfer_size);

	_SetEPTxCount(ENDP0, single_transfer_size);

	ep0_ctrl_info->remaining_data_size -= single_transfer_size;
	ep0_ctrl_info->data_buffer_offset += single_transfer_size;

	ep0_tx_state = EP_TX_VALID;

	/* Expect the host to abort the data IN stage */
	ep0_rx_state = EP_RX_VALID;
}

/*******************************************************************************
* Function Name  : NoData_Setup0.
* Description    : Proceed the processing of setup request without data stage.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void setup_without_data_process(void)
{
	RESULT		result = USB_UNSUPPORT;
	uint32_t	control_state;
	uint32_t	request_number = pInformation->b_request;
	uint8_t		request_type = pInformation->bm_request_type & (REQUEST_TYPE | RECIPIENT);

	/************************************************/
	/************************************************/

	//if (request_type == (STANDARD_REQUEST | DEVICE_RECIPIENT)) {
	if (request_type == DEVICE_RECIPIENT) {
	/* Device Request*/

		/************************************************/
		/* SET_CONFIGURATION*/
		if (request_number == SET_CONFIGURATION) {
			result = Standard_SetConfiguration();
		}

		/************************************************/
		/*SET ADDRESS*/
		else if (request_number == SET_ADDRESS) {

			if ((pInformation->USBwValue0 > 127) ||
				(pInformation->USBwValue1 != 0) ||
				(pInformation->USBwIndex != 0) ||
				(pInformation->Current_Configuration != 0)) {

				/* Device Address should be 127 or less*/
				control_state = STALLED;
				pInformation->control_state = control_state;
				return;

			} else {
				result = USB_SUCCESS;
			}
		}

		/************************************************/
		/*SET FEATURE for Device*/
		else if (request_number == SET_FEATURE) {

			if ((pInformation->USBwValue0 == DEVICE_REMOTE_WAKEUP) &&
				(pInformation->USBwIndex == 0)) {

				result = Standard_SetDeviceFeature();

			} else {
				result = USB_UNSUPPORT;
			}
		}

		/************************************************/
		/*Clear FEATURE for Device */
		else if (request_number == CLEAR_FEATURE) {

			if (pInformation->USBwValue0 == DEVICE_REMOTE_WAKEUP &&
				pInformation->USBwIndex == 0 &&
				(pInformation->Current_Feature & (1 << 5))) {

				result = Standard_ClearFeature();

			} else {
				result = USB_UNSUPPORT;
			}
		}
	}

	/************************************************/
	/************************************************/
	/* Interface Request*/
	else if (request_type == INTERFACE_RECIPIENT) {

		/************************************************/
		/*SET INTERFACE*/
		if (request_number == SET_INTERFACE) {
			result = Standard_SetInterface();
		}
	}

	/************************************************/
	/************************************************/
	/* EndPoint Request*/
	else if (request_type == ENDPOINT_RECIPIENT) {

		/************************************************/
		/*CLEAR FEATURE for EndPoint*/
		if (request_number == CLEAR_FEATURE) {
			result = Standard_ClearFeature();
		}
		/************************************************/
		/* SET FEATURE for EndPoint*/
		else if (request_number == SET_FEATURE) {
			result = Standard_SetEndPointFeature();
		}
	}

	/************************************************/
	/************************************************/
	else {
		result = USB_UNSUPPORT;
	}

	if (result != USB_SUCCESS) {
		result = (*pProperty->Class_NoData_Setup)(request_number);

		if (result == USB_NOT_READY) {
			control_state = PAUSE;
			pInformation->control_state = control_state;
			return;
		}
	}

	if (result != USB_SUCCESS) {
		control_state = STALLED;
		pInformation->control_state = control_state;
		return;
	}

	/* After no data stage SETUP */
	control_state = WAIT_STATUS_IN;
	_SetEPTxCount(ENDP0, 0);
	ep0_tx_state = EP_TX_VALID;

	pInformation->control_state = control_state;
}

/*******************************************************************************
* Function Name  : Data_Setup0.
* Description    : Proceed the processing of setup request with data stage.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void setup_with_data_process(void)
{
	uint8_t 	*(*CopyRoutine)(uint16_t) = NULL;
	RESULT		Result;
	uint8_t		request_number = pInformation->b_request;
	uint8_t		request_type = pInformation->bm_request_type & (REQUEST_TYPE | RECIPIENT);

	uint32_t	Related_Endpoint;
	uint32_t	Reserved;
	uint32_t	wOffset = 0;
	uint32_t	Status;

	/************************************************/
	/************************************************/
	/*GET DESCRIPTOR*/
	if (request_number == GET_DESCRIPTOR) {

		if (request_type == DEVICE_RECIPIENT) {

			uint8_t wValue1 = pInformation->USBwValue1;
			if (wValue1 == DEVICE_DESCRIPTOR) {
				CopyRoutine = pProperty->GetDeviceDescriptor;
			}

			else if (wValue1 == CONFIG_DESCRIPTOR) {
				CopyRoutine = pProperty->GetConfigDescriptor;
			}

			else if (wValue1 == STRING_DESCRIPTOR) {
				CopyRoutine = pProperty->GetStringDescriptor;
			}
		}
	}

	/************************************************/
	/************************************************/
	/*GET STATUS*/
	else if ((request_number == GET_STATUS) &&
			(pInformation->USBwValue == 0) &&
			(pInformation->USBwLength == 0x0002) &&
			(pInformation->USBwIndex1 == 0)) {

		/************************************************/
		/* GET STATUS for Device*/
		if ((request_type == DEVICE_RECIPIENT) && (pInformation->USBwIndex == 0)) {
			CopyRoutine = Standard_GetStatus;
		}

		/************************************************/
		/* GET STATUS for Interface*/
		else if (Type_Recipient == (STANDARD_REQUEST | INTERFACE_RECIPIENT)) {

			if (((*pProperty->Class_Get_Interface_Setting)(pInformation->USBwIndex0, 0) == USB_SUCCESS) &&
				(pInformation->Current_Configuration != 0)) {

				CopyRoutine = Standard_GetStatus;
			}
		}

		/************************************************/
		/* GET STATUS for EndPoint*/
		else if (request_type == ENDPOINT_RECIPIENT) {

			Related_Endpoint = (pInformation->USBwIndex0 & 0x0f);
			Reserved = pInformation->USBwIndex0 & 0x70;

			if (ValBit(pInformation->USBwIndex0, 7)) {
				/*Get Status of endpoint & stall the request if the related_ENdpoint
				 * is Disabled */
				Status = _GetEPTxStatus(Related_Endpoint);

			} else {
				Status = _GetEPRxStatus(Related_Endpoint);
			}

			if ((Related_Endpoint < Device_Table.Total_Endpoint) && (Reserved == 0) && (Status != 0)) {
				CopyRoutine = Standard_GetStatus;
			}
		}
	}

	/************************************************/
	/************************************************/
	/*GET CONFIGURATION*/
	else if (request_number == GET_CONFIGURATION) {

		if (request_type == (STANDARD_REQUEST | DEVICE_RECIPIENT)) {
			CopyRoutine = Standard_GetConfiguration;
		}
	}

	/************************************************/
	/************************************************/
	/*GET INTERFACE*/
	else if (request_number == GET_INTERFACE) {

		if ((request_type == INTERFACE_RECIPIENT) &&
			(pInformation->Current_Configuration != 0) &&
			(pInformation->USBwValue == 0) &&
			(pInformation->USBwIndex1 == 0) &&
			(pInformation->USBwLength == 0x0001) &&
			((*pProperty->Class_Get_Interface_Setting)(pInformation->USBwIndex0, 0) == USB_SUCCESS)) {

			CopyRoutine = Standard_GetInterface;
		}
	}

	if (CopyRoutine) {

		pInformation->ep0_ctrl_info.data_buffer_offset = wOffset;
		pInformation->ep0_ctrl_info.data_copy = CopyRoutine;
		/* sb in the original the cast to word was directly */
		/* now the cast is made step by step */
		(*CopyRoutine)(0);
		Result = USB_SUCCESS;

	} else {
		Result = (*pProperty->Class_Data_Setup)(pInformation->b_request);
		if (Result == USB_NOT_READY) {
			pInformation->control_state = PAUSE;
			return;
		}
	}

	if (pInformation->ep0_ctrl_info.remaining_data_size == 0xFFFF) {
		/* Data is not ready, wait it */
		pInformation->control_state = PAUSE;
		return;
	}

	if ((Result == USB_UNSUPPORT) || (pInformation->ep0_ctrl_info.remaining_data_size == 0)) {
		/* Unsupported request */
		pInformation->control_state = STALLED;
		return;
	}

	/* Device ==> Host */
	if (pInformation->bm_request_type & (1 << 7)) {
		__IO uint32_t wLength = pInformation->USBwLength;

		/* Restrict the data length to be the one host asks for */
		if (pInformation->ep0_ctrl_info.remaining_data_size > wLength) {
			pInformation->ep0_ctrl_info.remaining_data_size = wLength;

		} else if (pInformation->ep0_ctrl_info.remaining_data_size < pInformation->USBwLength) {

			if (pInformation->ep0_ctrl_info.remaining_data_size < pProperty->MaxPacketSize) {
				multiple_max_packet_size = FALSE;

			} else if ((pInformation->ep0_ctrl_info.remaining_data_size % pProperty->MaxPacketSize) == 0) {
				multiple_max_packet_size = TRUE;
			}
		}

		pInformation->ep0_ctrl_info.single_transfer_size = pProperty->MaxPacketSize;
		ep0_data_stage_in_process();

	} else {
		pInformation->control_state = OUT_DATA;
		ep0_rx_state = EP_RX_VALID; /* enable for next data reception */
	}

}

/*******************************************************************************
* Function Name  : Setup0_Process
* Description    : Get the device request data and dispatch to individual process.
* Input          : None.
* Output         : None.
* Return         : Post0_Process.
*******************************************************************************/
uint8_t ep0_setup_process(void)
{
	struct std_request {
		uint8_t		bmRequestType;
		uint8_t		bRequest;
		uint16_t	wValue;
		uint16_t	wIndex;
		uint16_t	wLength;
	};

	struct std_request *prequest = (struct std_request *)(PMAAddr + (uint8_t *)(_GetEPRxAddr(ENDP0) * 2));

	if (pInformation->control_state != PAUSE) {

		pInformation->bm_request_type = prequest->bmRequestType;

		pInformation->b_request = prequest->bRequest;

		pInformation->USBwValue = ByteSwap(prequest->wValue);

		pInformation->USBwIndex	= ByteSwap(prequest->wIndex);

		pInformation->USBwLength = ByteSwap(prequest->wLength); /*TODO: USB check correctness of the swapping*/
	}

	pInformation->control_state = SETTING_UP;

	if (pInformation->USBwLength == 0) {
		setup_without_data_process();

	} else {
		setup_with_data_process();
	}

	return ep0_finish_processing();


//	union {
//		uint8_t		*b;
//		uint16_t	*w;
//	} pBuf;
//
//	uint16_t offset = 1;
//
//	pBuf.b = PMAAddr + (uint8_t *)(_GetEPRxAddr(ENDP0) * 2); /* *2 for 32 bits addr */
//
//	if (pInformation->control_state != PAUSE) {
//		pInformation->bm_request_type = *pBuf.b++; /* bmRequestType */
//		pInformation->b_request = *pBuf.b++; /* bRequest */
//		pBuf.w += offset;  /* word not accessed because of 32 bits addressing */
//		pInformation->USBwValue = ByteSwap(*pBuf.w++); /* wValue */
//		pBuf.w += offset;  /* word not accessed because of 32 bits addressing */
//		pInformation->USBwIndex  = ByteSwap(*pBuf.w++); /* wIndex */
//		pBuf.w += offset;  /* word not accessed because of 32 bits addressing */
//		pInformation->USBwLength = *pBuf.w; /* wLength */
//	}
//
//	pInformation->control_state = SETTING_UP;
//
//	if (pInformation->USBwLength == 0) {
//
//		NoData_Setup0();
//	} else {
//
//		Data_Setup0();
//	}
//
//	return ep0_finish_processing();
}

/*******************************************************************************
* Function Name  : In0_Process
* Description    : Process the IN token on all default endpoint.
* Input          : None.
* Output         : None.
* Return         : Post0_Process.
*******************************************************************************/
uint8_t ep0_in_process(void)
{
	uint8_t control_state = pInformation->control_state;

	if ((control_state == IN_DATA) || (control_state == LAST_IN_DATA)) {

		ep0_data_stage_in_process();
		/* ControlState may be changed outside the function */
		control_state = pInformation->control_state;

	} else if (control_state == WAIT_STATUS_IN) {

		if ((pInformation->b_request == SET_ADDRESS) &&
			(((pInformation->bm_request_type & (REQUEST_TYPE | RECIPIENT)) == DEVICE_RECIPIENT))) {

			SetDeviceAddress(pInformation->USBwValue0);
			pUser_Standard_Requests->User_SetDeviceAddress();
		}

		(*pProperty->Process_Status_IN)();
		control_state = STALLED;

	} else {
		control_state = STALLED;
	}

	pInformation->control_state = control_state;

	return ep0_finish_processing();
}

/*******************************************************************************
* Function Name  : Out0_Process
* Description    : Process the OUT token on all default endpoint.
* Input          : None.
* Output         : None.
* Return         : Post0_Process.
*******************************************************************************/
uint8_t ep0_out_process(void)
{
	uint32_t control_state = pInformation->control_state;

	if ((control_state == OUT_DATA) || (control_state == LAST_OUT_DATA)) {

		ep0_data_stage_out_process();

		/* may be changed outside the function */
		/* TODO: USB check how is it possible? */
		control_state = pInformation->control_state;

	} else if (control_state == WAIT_STATUS_OUT) {
//		(*pProperty->Process_Status_OUT)();
		/* TODO: USB Is some action needed here? */
		control_state = STALLED;

	} else {
		control_state = STALLED;
	}

	pInformation->control_state = control_state;

	return ep0_finish_processing();
}

/*******************************************************************************
* Function Name  : Post0_Process
* Description    : Stall the Endpoint 0 in case of error.
* Input          : None.
* Output         : None.
* Return         : - 0 if the control State is in PAUSE
*                  - 1 if not.
*******************************************************************************/
uint8_t ep0_finish_processing(void)
{
	_SetEPRxCount(ENDP0, Device_Property.MaxPacketSize);

	if (pInformation->control_state == STALLED) {
		ep0_rx_state = EP_RX_STALL;
		ep0_tx_state = EP_TX_STALL;
	}

	return (pInformation->control_state == PAUSE);
}

/*******************************************************************************
* Function Name  : SetDeviceAddress.
* Description    : Set the device and all the used Endpoints addresses.
* Input          : - Val: device address.
* Output         : None.
* Return         : None.
*******************************************************************************/
void SetDeviceAddress(uint8_t Val)
{
	uint32_t i;
	uint32_t nEP = Device_Table.Total_Endpoint;

	/* set address in every used endpoint */
	for (i = 0; i < nEP; i++) {
		_SetEPAddress((uint8_t)i, (uint8_t)i);
	}

	_SetDADDR(Val | DADDR_EF); /* set device address and enable function */
}

/*******************************************************************************
* Function Name  : NOP_Process
* Description    : No operation function.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void NOP_Process(void)
{
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
