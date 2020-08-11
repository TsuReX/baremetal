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

  if ((usb_device_info->USBwValue0 <=
      Device_Table.Total_Configuration) && (usb_device_info->USBwValue1 == 0)
      && (usb_device_info->USBwIndex == 0)) /*call Back usb spec 2.0*/
  {
    usb_device_info->Current_Configuration = usb_device_info->USBwValue0;
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
  Re = (*usb_device_property->Class_Get_Interface_Setting)(usb_device_info->USBwIndex0, usb_device_info->USBwValue0);

  if (usb_device_info->Current_Configuration != 0)
  {
    if ((Re != USB_SUCCESS) || (usb_device_info->USBwIndex1 != 0)
        || (usb_device_info->USBwValue1 != 0))
    {
      return  USB_UNSUPPORT;
    }
    else if (Re == USB_SUCCESS)
    {
      usb_standard_requests->User_SetInterface();
      usb_device_info->Current_Interface = usb_device_info->USBwIndex0;
      usb_device_info->Current_AlternateSetting = usb_device_info->USBwValue0;
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
    usb_device_info->ep0_ctrl_info.remaining_data_size = 2;
    return 0;
  }

  /* Reset Status Information */
  StatusInfo.w = 0;

  if ((usb_device_info->bm_request_type & (REQUEST_TYPE | REQUEST_RECIPIENT)) == (STANDARD_REQUEST | DEVICE_RECIPIENT))
  {
    /*Get Device Status */
    uint8_t Feature = usb_device_info->Current_Feature;

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
  else if ((usb_device_info->bm_request_type & (REQUEST_TYPE | REQUEST_RECIPIENT)) == (STANDARD_REQUEST | INTERFACE_RECIPIENT))
  {
    return (uint8_t *)&StatusInfo;
  }
  /*Get EndPoint Status*/
  else if ((usb_device_info->bm_request_type & (REQUEST_TYPE | REQUEST_RECIPIENT)) == (STANDARD_REQUEST | ENDPOINT_RECIPIENT))
  {
    uint8_t Related_Endpoint;
    uint8_t wIndex0 = usb_device_info->USBwIndex0;

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
  usb_standard_requests->User_GetStatus();
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
  uint32_t     Type_Rec = (usb_device_info->bm_request_type & (REQUEST_TYPE | REQUEST_RECIPIENT));
  uint32_t     Status;


  if (Type_Rec == (STANDARD_REQUEST | DEVICE_RECIPIENT))
  {/*Device Clear Feature*/
    ClrBit(usb_device_info->Current_Feature, 5);
    return USB_SUCCESS;
  }
  else if (Type_Rec == (STANDARD_REQUEST | ENDPOINT_RECIPIENT))
  {/*EndPoint Clear Feature*/
    DEVICE* pDev;
    uint32_t Related_Endpoint;
    uint32_t wIndex0;
    uint32_t rEP;

    if ((usb_device_info->USBwValue != ENDPOINT_STALL)
        || (usb_device_info->USBwIndex1 != 0))
    {
      return USB_UNSUPPORT;
    }

    pDev = &Device_Table;
    wIndex0 = usb_device_info->USBwIndex0;
    rEP = wIndex0 & ~0x80;
    Related_Endpoint = ENDP0 + rEP;

    if (ValBit(usb_device_info->USBwIndex0, 7))
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
          SetEPRxCount(Related_Endpoint, property.MaxPacketSize);
          _SetEPRxStatus(Related_Endpoint, EP_RX_VALID);
        }
        else
        {
          ClearDTOG_RX(Related_Endpoint);
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

  wIndex0 = usb_device_info->USBwIndex0;
  rEP = wIndex0 & ~0x80;
  Related_Endpoint = ENDP0 + rEP;

  if (ValBit(usb_device_info->USBwIndex0, 7))
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
      || usb_device_info->USBwValue != 0 || Status == 0
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
  if (Length == 0)
  {
    usb_device_info->ep0_ctrl_info.remaining_data_size = pDesc->Descriptor_Size - wOffset;
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
	ENDPOINT_INFO	*ep0_ctrl_info = &usb_device_info->ep0_ctrl_info;
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
		usb_device_info->control_state = OUT_DATA;

	} else {

		if (ep0_ctrl_info->remaining_data_size > 0) {
			usb_device_info->control_state = LAST_OUT_DATA;

		} else if (ep0_ctrl_info->remaining_data_size == 0) {
			usb_device_info->control_state = WAIT_STATUS_IN;
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
	ENDPOINT_INFO	*ep0_ctrl_info = &usb_device_info->ep0_ctrl_info;
	uint8_t			*data_buffer;
	size_t			single_transfer_size;

	if ((ep0_ctrl_info->remaining_data_size == 0) && (usb_device_info->control_state == LAST_IN_DATA)) {

		if(multiple_max_packet_size == TRUE) {
			/* No more data to send and empty packet */
			_SetEPTxCount(ENDP0, 0);
			ep0_tx_state = EP_TX_VALID;

			usb_device_info->control_state = LAST_IN_DATA;
			multiple_max_packet_size = FALSE;

		} else  {
			/* No more data to send so STALL the TX Status*/
			usb_device_info->control_state = WAIT_STATUS_OUT;
			ep0_tx_state = EP_TX_STALL;
		}

		return;
	}

	single_transfer_size = ep0_ctrl_info->single_transfer_size;

	usb_device_info->control_state = (ep0_ctrl_info->remaining_data_size <= single_transfer_size) ? LAST_IN_DATA : IN_DATA;

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
	uint32_t	request_number = usb_device_info->b_request;
	uint8_t		request_type = usb_device_info->bm_request_type & (REQUEST_TYPE | REQUEST_RECIPIENT);

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

			if ((usb_device_info->USBwValue0 > 127) ||
				(usb_device_info->USBwValue1 != 0) ||
				(usb_device_info->USBwIndex != 0) ||
				(usb_device_info->Current_Configuration != 0)) {

				/* Device Address should be 127 or less*/
				control_state = STALLED;
				usb_device_info->control_state = control_state;
				return;

			} else {
				result = USB_SUCCESS;
			}
		}

		/************************************************/
		/*SET FEATURE for Device*/
		else if (request_number == SET_FEATURE) {

			if ((usb_device_info->USBwValue0 == DEVICE_REMOTE_WAKEUP) &&
				(usb_device_info->USBwIndex == 0)) {

				result = Standard_SetDeviceFeature();

			} else {
				result = USB_UNSUPPORT;
			}
		}

		/************************************************/
		/*Clear FEATURE for Device */
		else if (request_number == CLEAR_FEATURE) {

			if (usb_device_info->USBwValue0 == DEVICE_REMOTE_WAKEUP &&
				usb_device_info->USBwIndex == 0 &&
				(usb_device_info->Current_Feature & (1 << 5))) {

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
		result = (*usb_device_property->class_setup_without_data_process)(request_number);

		if (result == USB_NOT_READY) {
			control_state = PAUSE;
			usb_device_info->control_state = control_state;
			return;
		}
	}

	if (result != USB_SUCCESS) {
		control_state = STALLED;
		usb_device_info->control_state = control_state;
		return;
	}

	/* After no data stage SETUP */
	control_state = WAIT_STATUS_IN;
	_SetEPTxCount(ENDP0, 0);
	ep0_tx_state = EP_TX_VALID;

	usb_device_info->control_state = control_state;
}

uint32_t standard_request_process()
{
	uint8_t	request_direction = usb_device_info->bm_request_type & REQUEST_DIRECTION;
	uint8_t	request_recipient = usb_device_info->bm_request_type & REQUEST_RECIPIENT;

	switch (request_recipient) {

		case DEVICE_RECIPIENT_TYPE:

			break;

		case INTERFACE_RECIPIENT_TYPE:

			break;

		case ENDPOINT_RECIPIENT_TYPE:

			break;

		default:
			break;
	}

	return 0;
}

uint32_t class_request_process()
{
	uint8_t	request_direction = usb_device_info->bm_request_type & REQUEST_DIRECTION;
	uint8_t	request_recipient = usb_device_info->bm_request_type & REQUEST_RECIPIENT;
	uint8_t	request_number = usb_device_info->b_request;

	switch (request_recipient) {

		case DEVICE_RECIPIENT_TYPE:

			switch (request_number) {

				case GET_STATUS:

					break;

				case GET_DESCRIPTOR:

					break;

				case GET_CONFIGURATION:

					break;

				default:
					break;
			}

			break;

		case INTERFACE_RECIPIENT_TYPE:
			switch (request_number) {

				case GET_STATUS:

					break;

				case GET_INTERFACE:

					break;

				default:
					break;
			}
			break;

		case ENDPOINT_RECIPIENT_TYPE:
			switch (request_number) {

				case GET_STATUS:

					break;

				default:
					break;
			}
			break;

		default:
			break;
	}

	return 0;
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
	uint8_t		request_number = usb_device_info->b_request;
	uint8_t		request_type = usb_device_info->bm_request_type & (REQUEST_TYPE | REQUEST_RECIPIENT);


	uint32_t	related_endpoint;
	uint32_t	Reserved;
	uint32_t	wOffset = 0;
	uint32_t	Status;

	uint8_t		request_type = usb_device_info->bm_request_type & REQUEST_TYPE;

	switch (request_type) {

		case STANDARD_REQUEST_TYPE:
			standard_request_process();
			break;

		case CLASS_REQUEST_TYPE:
			class_request_process();
			break;

		case VENDOR_REQUEST_TYPE:

			break;

		default: /*RESERVED_REQUEST_TYPE*/
			break;
	}

	/************************************************/
	/************************************************/
	/*GET DESCRIPTOR*/
	if (request_number == GET_DESCRIPTOR) {

		if (request_type == DEVICE_RECIPIENT) {

			uint8_t wValue1 = usb_device_info->USBwValue1;
			if (wValue1 == DEVICE_DESCRIPTOR) {
				CopyRoutine = usb_device_property->GetDeviceDescriptor;
			}

			else if (wValue1 == CONFIG_DESCRIPTOR) {
				CopyRoutine = usb_device_property->GetConfigDescriptor;
			}

			else if (wValue1 == STRING_DESCRIPTOR) {
				CopyRoutine = usb_device_property->GetStringDescriptor;
			}
		}
	}

	/************************************************/
	/************************************************/
	/*GET STATUS*/
	else if ((request_number == GET_STATUS) &&
			(usb_device_info->USBwValue == 0) &&
			(usb_device_info->USBwLength == 0x0002) &&
			(usb_device_info->USBwIndex1 == 0)) {

		/************************************************/
		/* GET STATUS for Device*/
		if ((request_type == DEVICE_RECIPIENT) && (usb_device_info->USBwIndex == 0)) {
			CopyRoutine = Standard_GetStatus;
		}

		/************************************************/
		/* GET STATUS for Interface*/
		else if ((usb_device_info->bm_request_type & (REQUEST_TYPE | REQUEST_RECIPIENT)) == (STANDARD_REQUEST | INTERFACE_RECIPIENT)) {

			if (((*usb_device_property->Class_Get_Interface_Setting)(usb_device_info->USBwIndex0, 0) == USB_SUCCESS) &&
				(usb_device_info->Current_Configuration != 0)) {

				CopyRoutine = Standard_GetStatus;
			}
		}

		/************************************************/
		/* GET STATUS for EndPoint*/
		else if (request_type == ENDPOINT_RECIPIENT) {

			related_endpoint = (usb_device_info->USBwIndex0 & 0x0f);
			Reserved = usb_device_info->USBwIndex0 & 0x70;

			if (ValBit(usb_device_info->USBwIndex0, 7)) {
				/*Get Status of endpoint & stall the request if the related_ENdpoint
				 * is Disabled */
				Status = _GetEPTxStatus(related_endpoint);

			} else {
				Status = _GetEPRxStatus(related_endpoint);
			}

			if ((related_endpoint < Device_Table.Total_Endpoint) && (Reserved == 0) && (Status != 0)) {
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
			(usb_device_info->Current_Configuration != 0) &&
			(usb_device_info->USBwValue == 0) &&
			(usb_device_info->USBwIndex1 == 0) &&
			(usb_device_info->USBwLength == 0x0001) &&
			((*usb_device_property->Class_Get_Interface_Setting)(usb_device_info->USBwIndex0, 0) == USB_SUCCESS)) {

			CopyRoutine = Standard_GetInterface;
		}
	}

	if (CopyRoutine) {

		usb_device_info->ep0_ctrl_info.data_buffer_offset = wOffset;
		usb_device_info->ep0_ctrl_info.data_copy = CopyRoutine;
		/* sb in the original the cast to word was directly */
		/* now the cast is made step by step */
		(*CopyRoutine)(0);
		Result = USB_SUCCESS;

	} else {
		Result = (*usb_device_property->class_setup_with_data_process)(usb_device_info->b_request);
		if (Result == USB_NOT_READY) {
			usb_device_info->control_state = PAUSE;
			return;
		}
	}

	if (usb_device_info->ep0_ctrl_info.remaining_data_size == 0xFFFF) {
		/* Data is not ready, wait it */
		usb_device_info->control_state = PAUSE;
		return;
	}

	if ((Result == USB_UNSUPPORT) || (usb_device_info->ep0_ctrl_info.remaining_data_size == 0)) {
		/* Unsupported request */
		usb_device_info->control_state = STALLED;
		return;
	}

	/* Device ==> Host */
	if (usb_device_info->bm_request_type & (1 << 7)) {
		__IO uint32_t wLength = usb_device_info->USBwLength;

		/* Restrict the data length to be the one host asks for */
		if (usb_device_info->ep0_ctrl_info.remaining_data_size > wLength) {
			usb_device_info->ep0_ctrl_info.remaining_data_size = wLength;

		} else if (usb_device_info->ep0_ctrl_info.remaining_data_size < usb_device_info->USBwLength) {

			if (usb_device_info->ep0_ctrl_info.remaining_data_size < usb_device_property->MaxPacketSize) {
				multiple_max_packet_size = FALSE;

			} else if ((usb_device_info->ep0_ctrl_info.remaining_data_size % usb_device_property->MaxPacketSize) == 0) {
				multiple_max_packet_size = TRUE;
			}
		}

		usb_device_info->ep0_ctrl_info.single_transfer_size = usb_device_property->MaxPacketSize;
		ep0_data_stage_in_process();

	} else {
		usb_device_info->control_state = OUT_DATA;
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

	if (usb_device_info->control_state != PAUSE) {

		usb_device_info->bm_request_type = prequest->bmRequestType;

		usb_device_info->b_request = prequest->bRequest;

		usb_device_info->USBwValue = ByteSwap(prequest->wValue);

		usb_device_info->USBwIndex	= ByteSwap(prequest->wIndex);

		usb_device_info->USBwLength = ByteSwap(prequest->wLength); /*TODO: USB check correctness of the swapping*/
	}

	usb_device_info->control_state = SETTING_UP;

	if (usb_device_info->USBwLength == 0) {
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
	uint8_t control_state = usb_device_info->control_state;

	if ((control_state == IN_DATA) || (control_state == LAST_IN_DATA)) {

		ep0_data_stage_in_process();
		/* ControlState may be changed outside the function */
		control_state = usb_device_info->control_state;

	} else if (control_state == WAIT_STATUS_IN) {

		if ((usb_device_info->b_request == SET_ADDRESS) &&
			(((usb_device_info->bm_request_type & (REQUEST_TYPE | REQUEST_RECIPIENT)) == DEVICE_RECIPIENT))) {

			SetDeviceAddress(usb_device_info->USBwValue0);
			usb_standard_requests->User_SetDeviceAddress();
		}

		(*usb_device_property->status_in_process)();
		control_state = STALLED;

	} else {
		control_state = STALLED;
	}

	usb_device_info->control_state = control_state;

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
	uint32_t control_state = usb_device_info->control_state;

	if ((control_state == OUT_DATA) || (control_state == LAST_OUT_DATA)) {

		ep0_data_stage_out_process();

		/* may be changed outside the function */
		/* TODO: USB check how is it possible? */
		control_state = usb_device_info->control_state;

	} else if (control_state == WAIT_STATUS_OUT) {
//		(*pProperty->Process_Status_OUT)();
		/* TODO: USB Is some action needed here? */
		control_state = STALLED;

	} else {
		control_state = STALLED;
	}

	usb_device_info->control_state = control_state;

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
	_SetEPRxCount(ENDP0, property.MaxPacketSize);

	if (usb_device_info->control_state == STALLED) {
		ep0_rx_state = EP_RX_STALL;
		ep0_tx_state = EP_TX_STALL;
	}

	return (usb_device_info->control_state == PAUSE);
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
