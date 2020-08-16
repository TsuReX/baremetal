#include "usb_core.h"
#include "usb_lib.h"
#include "console.h"

#include <stddef.h>

bool multiple_max_packet_size = FALSE;
extern DEVICE_INFO *usb_device_info;
extern USER_STANDARD_REQUESTS  *usb_standard_requests;
extern DEVICE_PROP *usb_device_property;

static void ep0_data_stage_out_process(void);
static void ep0_data_stage_in_process(void);
static void setup_without_data_process(void);
static void setup_with_data_process(void);
static void standard_request_process(void);

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
	uint8_t			*src_data_buffer;
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

	src_data_buffer = (*ep0_ctrl_info->data_copy)(single_transfer_size);

	copy_to_usb(src_data_buffer, _GetEPTxAddr(ENDP0), single_transfer_size);

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
	uint8_t request_type = usb_device_info->bm_request_type & REQUEST_TYPE;

	switch (request_type) {

		case STANDARD_REQUEST_TYPE:
			standard_request_process();
			break;

		case CLASS_REQUEST_TYPE:
			if ((*usb_device_property->class_setup_without_data_process)(usb_device_info->b_request) == USB_NOT_READY) {
				usb_device_info->control_state = PAUSE;
				return;
			}
//			class_request_process();
			break;

		case VENDOR_REQUEST_TYPE:

			break;

		default: /*RESERVED_REQUEST_TYPE*/
			break;
	}

//
///********************************************************************************************************************/
//
//	RESULT		result = USB_UNSUPPORT;
//	uint32_t	control_state;
//	uint32_t	request_number = usb_device_info->b_request;
//	uint8_t		request_type = usb_device_info->bm_request_type & (REQUEST_TYPE | REQUEST_RECIPIENT);
//
//	/************************************************/
//	/************************************************/
//
//	//if (request_type == (STANDARD_REQUEST | DEVICE_RECIPIENT)) {
//	if (request_type == DEVICE_RECIPIENT) {
//	/* Device Request*/
//
//		/************************************************/
//		/* SET_CONFIGURATION*/
//		if (request_number == SET_CONFIGURATION) {
//			result = Standard_SetConfiguration();
//		}
//
//		/************************************************/
//		/*SET ADDRESS*/
//		else if (request_number == SET_ADDRESS) {
//
//			if ((usb_device_info->w_value & 0xFF > 127) ||
//				(usb_device_info->USBwValue1 != 0) ||
//				(usb_device_info->USBwIndex != 0) ||
//				(usb_device_info->Current_Configuration != 0)) {
//
//				/* Device Address should be 127 or less*/
//				control_state = STALLED;
//				usb_device_info->control_state = control_state;
//				return;
//
//			} else {
//				result = USB_SUCCESS;
//			}
//		}
//
//		/************************************************/
//		/*SET FEATURE for Device*/
//		else if (request_number == SET_FEATURE) {
//
//			if ((usb_device_info->w_value & 0xFF == DEVICE_REMOTE_WAKEUP) &&
//				(usb_device_info->USBwIndex == 0)) {
//
//				result = Standard_SetDeviceFeature();
//
//			} else {
//				result = USB_UNSUPPORT;
//			}
//		}
//
//		/************************************************/
//		/*Clear FEATURE for Device */
//		else if (request_number == CLEAR_FEATURE) {
//
//			if (usb_device_info->w_value & 0xFF == DEVICE_REMOTE_WAKEUP &&
//				usb_device_info->USBwIndex == 0 &&
//				(usb_device_info->Current_Feature & (1 << 5))) {
//
//				result = Standard_ClearFeature();
//
//			} else {
//				result = USB_UNSUPPORT;
//			}
//		}
//	}
//
//	/************************************************/
//	/************************************************/
//	/* Interface Request*/
//	else if (request_type == INTERFACE_RECIPIENT) {
//
//		/************************************************/
//		/*SET INTERFACE*/
//		if (request_number == SET_INTERFACE) {
//			result = Standard_SetInterface();
//		}
//	}
//
//	/************************************************/
//	/************************************************/
//	/* EndPoint Request*/
//	else if (request_type == ENDPOINT_RECIPIENT) {
//
//		/************************************************/
//		/*CLEAR FEATURE for EndPoint*/
//		if (request_number == CLEAR_FEATURE) {
//			result = Standard_ClearFeature();
//		}
//		/************************************************/
//		/* SET FEATURE for EndPoint*/
//		else if (request_number == SET_FEATURE) {
//			result = Standard_SetEndPointFeature();
//		}
//	}
//
//	/************************************************/
//	/************************************************/
//	else {
//		result = USB_UNSUPPORT;
//	}
//
//	if (result != USB_SUCCESS) {
//		result = (*usb_device_property->class_setup_without_data_process)(request_number);
//
//		if (result == USB_NOT_READY) {
//			control_state = PAUSE;
//			usb_device_info->control_state = control_state;
//			return;
//		}
//	}
//
//	if (result != USB_SUCCESS) {
//		control_state = STALLED;
//		usb_device_info->control_state = control_state;
//		return;
//	}
//
//	/* After no data stage SETUP */
//	control_state = WAIT_STATUS_IN;
//	_SetEPTxCount(ENDP0, 0);
//	ep0_tx_state = EP_TX_VALID;
//
//	usb_device_info->control_state = control_state;
}

uint32_t class_request_process()
{
//	uint8_t	request_direction = usb_device_info->bm_request_type & REQUEST_DIRECTION;
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

void standard_request_process()
{
	uint8_t	request_direction = usb_device_info->bm_request_type & REQUEST_DIRECTION;
	uint8_t	request_recipient = usb_device_info->bm_request_type & REQUEST_RECIPIENT;
	uint8_t	request_number = usb_device_info->b_request;

	uint8_t *(*copy_routine)(uint16_t) = NULL;
	uint8_t wValue1 = 0;
	uint32_t status;
	uint32_t related_endpoint = 0;
	uint32_t reserved = 0;

	switch (request_recipient) {

		case DEVICE_RECIPIENT_TYPE:

			switch (request_number) {

				case GET_STATUS:
					if (usb_device_info->w_index == 0) {
						copy_routine = Standard_GetStatus;
					}
					break;

				case GET_DESCRIPTOR:
					wValue1 = (usb_device_info->w_value >> 8);
					if (wValue1 == DEVICE_DESCRIPTOR) {
						copy_routine = usb_device_property->GetDeviceDescriptor;

					} else if (wValue1 == CONFIG_DESCRIPTOR) {
						copy_routine = usb_device_property->GetConfigDescriptor;

					} else if (wValue1 == STRING_DESCRIPTOR) {
						copy_routine = usb_device_property->GetStringDescriptor;
					}
					break;

				case GET_CONFIGURATION:
					copy_routine = Standard_GetConfiguration;
					break;

				case SET_CONFIGURATION:
					if (Standard_SetConfiguration() != USB_SUCCESS) {
						usb_device_info->control_state = STALLED;
						return;
					}
					break;

				case SET_ADDRESS:
					if (((usb_device_info->w_value & 0xFF) > 127) ||
						((usb_device_info->w_value >> 8) != 0) ||
						(usb_device_info->w_index != 0) ||
						(usb_device_info->Current_Configuration != 0)) {

						usb_device_info->control_state = STALLED;
						return;
					}
					break;

				case SET_FEATURE:
					if (((usb_device_info->w_value & 0xFF) == DEVICE_REMOTE_WAKEUP) &&
						(usb_device_info->w_index == 0)) {

						if (Standard_SetDeviceFeature() != USB_SUCCESS) {
							usb_device_info->control_state = STALLED;
							return;
						}

					} else {
						usb_device_info->control_state = STALLED;
						return;
					}
					break;

				case CLEAR_FEATURE:
					if ((usb_device_info->w_value & 0xFF) == DEVICE_REMOTE_WAKEUP &&
						usb_device_info->w_index == 0 &&
						(usb_device_info->Current_Feature & (1 << 5))) {

						if (Standard_ClearFeature() != USB_SUCCESS) {
							usb_device_info->control_state = STALLED;
							return;
						}

					} else {
						usb_device_info->control_state = STALLED;
						return;
					}
					break;

				default:
					break;
			}

			break;

		case INTERFACE_RECIPIENT_TYPE:
			switch (request_number) {

				case GET_STATUS:
					if (((*usb_device_property->Class_Get_Interface_Setting)(usb_device_info->w_index & 0xFF, 0) == USB_SUCCESS) &&
						(usb_device_info->Current_Configuration != 0)) {

						copy_routine = Standard_GetStatus;
					}
					break;

				case GET_INTERFACE:
					if ((usb_device_info->Current_Configuration != 0) &&
						(usb_device_info->w_value == 0) &&
						((usb_device_info->w_index >> 8) == 0) &&
						(usb_device_info->w_length == 0x0001) &&
						((*usb_device_property->Class_Get_Interface_Setting)(usb_device_info->w_index & 0xFF, 0) == USB_SUCCESS)) {

						copy_routine = Standard_GetInterface;
					}
					break;
				case SET_INTERFACE:
					if (Standard_SetInterface() != USB_SUCCESS) {
						usb_device_info->control_state = STALLED;
						return;
					}
					break;

				default:
					break;
			}
			break;

		case ENDPOINT_RECIPIENT_TYPE:
			switch (request_number) {

				case GET_STATUS:
					related_endpoint = (usb_device_info->w_index & 0xFF & 0x0f);
					reserved = usb_device_info->w_index & 0xFF & 0x70;

					if ((usb_device_info->w_index & 0xFF) & (1 << 7)) {
						/*Get status of endpoint & stall the request if the related_ENdpoint
						 * is Disabled */
						status = _GetEPTxStatus(related_endpoint);

					} else {
						status = _GetEPRxStatus(related_endpoint);
					}

					if ((related_endpoint < Device_Table.Total_Endpoint) && (reserved == 0) && (status != 0)) {
						copy_routine = Standard_GetStatus;
					}
					break;

				case SET_FEATURE:
					if (Standard_SetEndPointFeature() != USB_SUCCESS) {
						usb_device_info->control_state = STALLED;
						return;
					}
					break;

				case CLEAR_FEATURE:
					if (Standard_ClearFeature() != USB_SUCCESS) {
						usb_device_info->control_state = STALLED;
						return;
					}
					break;

				default:
					break;
			}
			break;

		default:
			break;
	}

	/* Setup without data stage. */
	if (usb_device_info->w_length == 0) {

		_SetEPTxCount(ENDP0, 0);
		ep0_tx_state = EP_TX_VALID;
		usb_device_info->control_state = WAIT_STATUS_IN;

	} else { /* Setup with data stage. */

		usb_device_info->ep0_ctrl_info.data_buffer_offset = 0;
		usb_device_info->ep0_ctrl_info.data_copy = copy_routine;

		(*copy_routine)(0);

		/* TODO: USB Check a purpose of the construction below.*/
		if (usb_device_info->ep0_ctrl_info.remaining_data_size == 0xFFFF) {
			/* Data is not ready, wait it */
			usb_device_info->control_state = PAUSE;
			return;
		}

		/* TODO: USB Check a purpose of the construction below.*/
		if ((usb_device_info->ep0_ctrl_info.remaining_data_size == 0)) {
			/* Unsupported request */
			usb_device_info->control_state = STALLED;
			return;
		}

		if (request_direction == TO_HOST_DIRECTION_TYPE) {

			/* Restrict the data length to be the one host asks for */
			if (usb_device_info->ep0_ctrl_info.remaining_data_size > usb_device_info->w_length) {
				/* TODO: USB Check a correctness of the construction below. */
				usb_device_info->ep0_ctrl_info.remaining_data_size = usb_device_info->w_length;
			}

			if (usb_device_info->ep0_ctrl_info.remaining_data_size < usb_device_property->MaxPacketSize) {
				multiple_max_packet_size = FALSE;

			} else if ((usb_device_info->ep0_ctrl_info.remaining_data_size % usb_device_property->MaxPacketSize) == 0) {
				multiple_max_packet_size = TRUE;
			}

			usb_device_info->ep0_ctrl_info.single_transfer_size = usb_device_property->MaxPacketSize;

			ep0_data_stage_in_process();

		} else { /* TO_DEVICE_DIRECTION_TYPE */
			usb_device_info->control_state = OUT_DATA;
			ep0_rx_state = EP_RX_VALID; /* enable for next data reception */
		}
	}
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
	uint8_t	request_type = usb_device_info->bm_request_type & REQUEST_TYPE;

	switch (request_type) {

		case STANDARD_REQUEST_TYPE:
			standard_request_process();
			break;

		case CLASS_REQUEST_TYPE:
			if ((*usb_device_property->class_setup_with_data_process)(usb_device_info->b_request) == USB_NOT_READY) {
				usb_device_info->control_state = PAUSE;
				return;
			}
//			class_request_process();
			break;

		case VENDOR_REQUEST_TYPE:

			break;

		default: /*RESERVED_REQUEST_TYPE*/
			break;
	}

//	uint8_t 	*(*CopyRoutine)(uint16_t) = NULL;
//	RESULT		Result;
//	uint8_t		request_number = usb_device_info->b_request;
//	uint8_t		request_type = usb_device_info->bm_request_type & (REQUEST_TYPE | REQUEST_RECIPIENT);
//
//
//	uint32_t	related_endpoint;
//	uint32_t	Reserved;
//	uint32_t	wOffset = 0;
//	uint32_t	Status;
//
//	/************************************************/
//	/************************************************/
//	/*GET DESCRIPTOR*/
//	if (request_number == GET_DESCRIPTOR) {
//
//		if (request_type == DEVICE_RECIPIENT) {
//
//			uint8_t wValue1 = usb_device_info->USBwValue1;
//			if (wValue1 == DEVICE_DESCRIPTOR) {
//				CopyRoutine = usb_device_property->GetDeviceDescriptor;
//			}
//
//			else if (wValue1 == CONFIG_DESCRIPTOR) {
//				CopyRoutine = usb_device_property->GetConfigDescriptor;
//			}
//
//			else if (wValue1 == STRING_DESCRIPTOR) {
//				CopyRoutine = usb_device_property->GetStringDescriptor;
//			}
//		}
//	}
//
//	/************************************************/
//	/************************************************/
//	/*GET STATUS*/
//	else if ((request_number == GET_STATUS) &&
//			(usb_device_info->USBwValue == 0) &&
//			(usb_device_info->USBwLength == 0x0002) &&
//			(usb_device_info->USBwIndex1 == 0)) {
//
//		/************************************************/
//		/* GET STATUS for Device*/
//		if ((request_type == DEVICE_RECIPIENT) && (usb_device_info->USBwIndex == 0)) {
//			CopyRoutine = Standard_GetStatus;
//		}
//
//		/************************************************/
//		/* GET STATUS for Interface*/
//		else if ((usb_device_info->bm_request_type & (REQUEST_TYPE | REQUEST_RECIPIENT)) == (STANDARD_REQUEST | INTERFACE_RECIPIENT)) {
//
//			if (((*usb_device_property->Class_Get_Interface_Setting)(usb_device_info->USBwIndex0, 0) == USB_SUCCESS) &&
//				(usb_device_info->Current_Configuration != 0)) {
//
//				CopyRoutine = Standard_GetStatus;
//			}
//		}
//
//		/************************************************/
//		/* GET STATUS for EndPoint*/
//		else if (request_type == ENDPOINT_RECIPIENT) {
//
//			related_endpoint = (usb_device_info->USBwIndex0 & 0x0f);
//			Reserved = usb_device_info->USBwIndex0 & 0x70;
//
//			if (ValBit(usb_device_info->USBwIndex0, 7)) {
//				/*Get Status of endpoint & stall the request if the related_ENdpoint
//				 * is Disabled */
//				Status = _GetEPTxStatus(related_endpoint);
//
//			} else {
//				Status = _GetEPRxStatus(related_endpoint);
//			}
//
//			if ((related_endpoint < Device_Table.Total_Endpoint) && (Reserved == 0) && (Status != 0)) {
//				CopyRoutine = Standard_GetStatus;
//			}
//		}
//	}
//
//	/************************************************/
//	/************************************************/
//	/*GET CONFIGURATION*/
//	else if (request_number == GET_CONFIGURATION) {
//
//		if (request_type == (STANDARD_REQUEST | DEVICE_RECIPIENT)) {
//			CopyRoutine = Standard_GetConfiguration;
//		}
//	}
//
//	/************************************************/
//	/************************************************/
//	/*GET INTERFACE*/
//	else if (request_number == GET_INTERFACE) {
//
//		if ((request_type == INTERFACE_RECIPIENT) &&
//			(usb_device_info->Current_Configuration != 0) &&
//			(usb_device_info->USBwValue == 0) &&
//			(usb_device_info->USBwIndex1 == 0) &&
//			(usb_device_info->USBwLength == 0x0001) &&
//			((*usb_device_property->Class_Get_Interface_Setting)(usb_device_info->USBwIndex0, 0) == USB_SUCCESS)) {
//
//			CopyRoutine = Standard_GetInterface;
//		}
//	}
//
//	if (CopyRoutine) {
//
//		usb_device_info->ep0_ctrl_info.data_buffer_offset = wOffset;
//		usb_device_info->ep0_ctrl_info.data_copy = CopyRoutine;
//		/* sb in the original the cast to word was directly */
//		/* now the cast is made step by step */
//		(*CopyRoutine)(0);
//		Result = USB_SUCCESS;
//
//	} else {
//		Result = (*usb_device_property->class_setup_with_data_process)(usb_device_info->b_request);
//		if (Result == USB_NOT_READY) {
//			usb_device_info->control_state = PAUSE;
//			return;
//		}
//	}
//
//	if (usb_device_info->ep0_ctrl_info.remaining_data_size == 0xFFFF) {
//		/* Data is not ready, wait it */
//		usb_device_info->control_state = PAUSE;
//		return;
//	}
//
//	if ((Result == USB_UNSUPPORT) || (usb_device_info->ep0_ctrl_info.remaining_data_size == 0)) {
//		/* Unsupported request */
//		usb_device_info->control_state = STALLED;
//		return;
//	}
//
//	uint8_t	request_direction = usb_device_info->bm_request_type & REQUEST_DIRECTION;
//	if (request_direction == TO_HOST_DIRECTION_TYPE) {
//
//		uint32_t wLength = usb_device_info->USBwLength;
//
//		/* Restrict the data length to be the one host asks for */
//		if (usb_device_info->ep0_ctrl_info.remaining_data_size > wLength) {
//			usb_device_info->ep0_ctrl_info.remaining_data_size = wLength;
//
//		} else if (usb_device_info->ep0_ctrl_info.remaining_data_size < usb_device_info->USBwLength) {
//
//			if (usb_device_info->ep0_ctrl_info.remaining_data_size < usb_device_property->MaxPacketSize) {
//				multiple_max_packet_size = FALSE;
//
//			} else if ((usb_device_info->ep0_ctrl_info.remaining_data_size % usb_device_property->MaxPacketSize) == 0) {
//				multiple_max_packet_size = TRUE;
//			}
//		}
//
//		usb_device_info->ep0_ctrl_info.single_transfer_size = usb_device_property->MaxPacketSize;
//		ep0_data_stage_in_process();
//
//	} else {
//		usb_device_info->control_state = OUT_DATA;
//		ep0_rx_state = EP_RX_VALID; /* enable for next data reception */
//	}

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
	d_print("%s()\r\n",  __func__);
	struct std_request {
		uint8_t		bm_request_type;
		uint8_t		b_request;
		uint16_t	w_value;
		uint16_t	w_index;
		uint16_t	w_length;
	};
	struct std_request prequest;

	/* TODO: USB check correctness of the swapping*/
	copy_from_usb((uint8_t*)&prequest, _GetEPRxAddr(ENDP0), sizeof(struct std_request));

	d_print("bm_request_type: 0x%02X\r\n", prequest.bm_request_type);
	d_print("b_request: 0x%02X\r\n", prequest.b_request);
	d_print("w_value: 0x%04X\r\n", prequest.w_value);
	d_print("w_value: 0x%04X\r\n", prequest.w_value);
	d_print("w_length: 0x%04X\r\n", prequest.w_length);

	if (usb_device_info->control_state != PAUSE) {

		usb_device_info->bm_request_type = prequest.bm_request_type;
		usb_device_info->b_request = prequest.b_request;
		usb_device_info->w_value = prequest.w_value;
		usb_device_info->w_index = prequest.w_index;
		usb_device_info->w_length = prequest.w_length;
	}

	usb_device_info->control_state = SETTING_UP;

	if (usb_device_info->w_length == 0) {
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
	d_print("%s()\r\n",  __func__);
	uint8_t control_state = usb_device_info->control_state;

	if ((control_state == IN_DATA) || (control_state == LAST_IN_DATA)) {

		ep0_data_stage_in_process();
		/* ControlState may be changed outside the function */
		control_state = usb_device_info->control_state;

	} else if (control_state == WAIT_STATUS_IN) {

		if ((usb_device_info->b_request == SET_ADDRESS) &&
			(((usb_device_info->bm_request_type & (REQUEST_TYPE | REQUEST_RECIPIENT)) == DEVICE_RECIPIENT))) {

			SetDeviceAddress(usb_device_info->w_value & 0xFF);
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
	d_print("%s()\r\n",  __func__);
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
	d_print("%s()\r\n",  __func__);
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
