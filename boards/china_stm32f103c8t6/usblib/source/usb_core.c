#include "usb_core.h"
#include "usb_lib.h"
#include "usb_prop.h"
#include "usb_desc.h"
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
* Function Name  : HID_init.
* Description    : HID init routine.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void init(void)
{
//	d_print("%s()\r\n",  __func__);
	_SetCNTR(CNTR_FRES | CNTR_PDWN);

	LL_mDelay(10);

	_SetCNTR(CNTR_FRES);

	LL_mDelay(1);

	_SetISTR(0);
	_SetCNTR(CNTR_CTRM | /*CNTR_WKUPM | CNTR_SUSPM | CNTR_ERRM | CNTR_SOFM | CNTR_ESOFM |*/ CNTR_RESETM);

	usb_device_info->Current_Configuration = 0;
	bDeviceState = UNCONNECTED;
}

/*******************************************************************************
* Function Name  : HID_Reset.
* Description    : HID reset routine.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void reset(void)
{
//	LL_GPIO_ResetOutputPin(GPIOA, LL_GPIO_PIN_7);
//	d_print("%s()\r\n",  __func__);
//	pma_init();
	usb_device_info->Current_Configuration = 0;
	usb_device_info->Current_Interface = 0;/*the default Interface*/
	usb_device_info->Current_Feature = rhid_configuration_descriptor[7];

	_SetCNTR(0);
	_SetCNTR(CNTR_CTRM | CNTR_RESETM);

	_SetISTR(0);

	_SetDADDR(DADDR_EF);

	_SetENDPOINT(ENDP0, EP_CONTROL | EP_TX_STALL | EP_RX_VALID);

	_SetEPRxAddr(ENDP0, ENDP0_RXADDR);
	_SetEPTxAddr(ENDP0, ENDP0_TXADDR);
	_SetEPRxCount(ENDP0, EP0_MAX_PACKET_SIZE);

	_SetBTABLE(BTABLE_ADDRESS);

	/* Set this device to response on default address */
	SetDeviceAddress(0);
//	bDeviceState = ATTACHED;
//	LL_GPIO_SetOutputPin(GPIOA, LL_GPIO_PIN_7);
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
	ep0_rx_state = EP_RX_VALID;
//	d_print("single_transfer_size: 0x%04X\r\n", single_transfer_size);
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
//	d_print("%s()\r\n",  __func__);
	switch (request_type) {

		case STANDARD_REQUEST_TYPE:
//			d_print("STANDARD_REQUEST_TYPE\r\n");
			standard_request_process();
			break;

		case CLASS_REQUEST_TYPE:
//			d_print("CLASS_REQUEST_TYPE\r\n");
			if ((*usb_device_property->class_setup_without_data_process)(usb_device_info->b_request) == USB_NOT_READY) {
				usb_device_info->control_state = PAUSE;
				return;
			}
//			class_request_process();
			break;

		case VENDOR_REQUEST_TYPE:
//			d_print("VENDOR_REQUEST_TYPE\r\n");
			break;

		default: /*RESERVED_REQUEST_TYPE*/
//			d_print("RESERVED_REQUEST_TYPE\r\n");
			break;
	}

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
//			d_print("DEVICE_RECIPIENT_TYPE\r\n");
			switch (request_number) {

				case GET_STATUS:
//					d_print("GET_STATUS\r\n");
					if (usb_device_info->w_index == 0) {
						copy_routine = Standard_GetStatus;
					}
					break;

				case GET_DESCRIPTOR:
//					d_print("GET_DESCRIPTOR\r\n");
					wValue1 = (usb_device_info->w_value >> 8);
					if (wValue1 == DEVICE_DESCRIPTOR) {
						copy_routine = usb_device_property->GetDeviceDescriptor;
//						d_print("DEVICE_DESCRIPTOR\r\n");

					} else if (wValue1 == CONFIG_DESCRIPTOR) {
						copy_routine = usb_device_property->GetConfigDescriptor;
//						d_print("CONFIG_DESCRIPTOR\r\n");

					} else if (wValue1 == STRING_DESCRIPTOR) {
						copy_routine = usb_device_property->GetStringDescriptor;
//						d_print("STRING_DESCRIPTOR\r\n");

					} else if (wValue1 == DEVICE_QUALIFIER) {
						copy_routine = HID_GetDeviceDescriptorQualifier;
//						d_print("DEVICE_QUALIFIER\r\n");

					}
					break;

				case GET_CONFIGURATION:
//					d_print("GET_CONFIGURATION\r\n");
					copy_routine = Standard_GetConfiguration;
					break;

				case SET_CONFIGURATION:
//					d_print("SET_CONFIGURATION\r\n");
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
						// d_print("SET_ADDRESS ERR\r\n");
						return;
					}
//					d_print("SET_ADDRESS\r\n");
					break;

				case SET_FEATURE:
//					d_print("SET_FEATURE\r\n");
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
//					d_print("CLEAR_FEATURE\r\n");
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
					d_print("DEVICE_RECIPIENT_TYPE UNKNOWN_REQUEST\r\n");
					break;
			}

			break;

		case INTERFACE_RECIPIENT_TYPE:
//			// d_print("INTERFACE_RECIPIENT_TYPE\r\n");
			switch (request_number) {
				case GET_DESCRIPTOR:
					d_print("GET_DESCRIPTOR\r\n");
					wValue1 = (usb_device_info->w_value >> 8);
					if (wValue1 == HID_REPORT_DESCRIPTOR_TYPE) {
						copy_routine = HID_GetReportDescriptor;

					} else if (wValue1 == HID_DESCRIPTOR_TYPE) {
						copy_routine = HID_GetHIDDescriptor;
					}
					break;
				case GET_STATUS:
//					d_print("GET_STATUS\r\n");
					if (((*usb_device_property->Class_Get_Interface_Setting)(usb_device_info->w_index & 0xFF, 0) == USB_SUCCESS) &&
						(usb_device_info->Current_Configuration != 0)) {

						copy_routine = Standard_GetStatus;
					}
					break;

				case GET_INTERFACE:
//					d_print("GET_INTERFACE\r\n");
					if ((usb_device_info->Current_Configuration != 0) &&
						(usb_device_info->w_value == 0) &&
						((usb_device_info->w_index >> 8) == 0) &&
						(usb_device_info->w_length == 0x0001) &&
						((*usb_device_property->Class_Get_Interface_Setting)(usb_device_info->w_index & 0xFF, 0) == USB_SUCCESS)) {

						copy_routine = Standard_GetInterface;
					}
					break;
				case SET_INTERFACE:
//					d_print("SET_INTERFACE\r\n");
					if (Standard_SetInterface() != USB_SUCCESS) {
						usb_device_info->control_state = STALLED;
						return;
					}
					break;

				default:
					d_print("INTERFACE_RECIPIENT_TYPE UNKNOWN_REQUEST\r\n");
					break;
			}
			break;

		case ENDPOINT_RECIPIENT_TYPE:
//			// d_print("ENDPOINT_RECIPIENT_TYPE\r\n");
			switch (request_number) {

				case GET_STATUS:
//					d_print("GET_STATUS\r\n");
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
//					d_print("SET_FEATURE\r\n");
					if (Standard_SetEndPointFeature() != USB_SUCCESS) {
						usb_device_info->control_state = STALLED;
						return;
					}
					break;

				case CLEAR_FEATURE:
//					d_print("CLEAR_FEATURE\r\n");
					if (Standard_ClearFeature() != USB_SUCCESS) {
						usb_device_info->control_state = STALLED;
						return;
					}
					break;

				default:
					 d_print("ENDPOINT_RECIPIENT_TYPE UNKNOWN_REQUEST\r\n");
					break;
			}
			break;

		default:
			 d_print("UNKNOWN_RECIPIENT_TYPE\r\n");
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

		if (copy_routine == 0) {
			d_print("ERROR bm_request_type: 0x%02X, b_request: 0x%02X, w_value: 0x%04X\r\n",
					usb_device_info->bm_request_type, usb_device_info->b_request, usb_device_info->w_value);
		} else {
			copy_routine(0);
		}

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
//			d_print("TO_HOST_DIRECTION_TYPE\r\n");
//			d_print("usb_device_info->w_length: 0x%04X\r\n", usb_device_info->w_length);
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
//			d_print("TO_DEVICE_DIRECTION_TYPE\r\n");
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
//	d_print("%s()\r\n",  __func__);
	switch (request_type) {

		case STANDARD_REQUEST_TYPE:
//			d_print("STANDARD_REQUEST_TYPE\r\n");
			standard_request_process();
			break;

		case CLASS_REQUEST_TYPE:
//			d_print("CLASS_REQUEST_TYPE\r\n");
			if ((*usb_device_property->class_setup_with_data_process)(usb_device_info->b_request) == USB_NOT_READY) {
				usb_device_info->control_state = PAUSE;
				return;
			}
//			class_request_process();
			break;

		case VENDOR_REQUEST_TYPE:
			 d_print("RESERVED_REQUEST_TYPE\r\n");
			break;

		default: /*RESERVED_REQUEST_TYPE*/
			 d_print("RESERVED_REQUEST_TYPE\r\n");
			break;
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
//	d_print("%s()\r\n",  __func__);
	struct std_request {
		uint8_t		bm_request_type;
		uint8_t		b_request;
		uint16_t	w_value;
		uint16_t	w_index;
		uint16_t	w_length;
	};
	struct std_request prequest;

	uint32_t ep0_rx_addr = _GetEPRxAddr(ENDP0);
	copy_from_usb((uint8_t*)&prequest, ep0_rx_addr, sizeof(struct std_request));
//
//	 d_print("bm_request_type: 0x%02X\r\n", prequest.bm_request_type);
//	 d_print("b_request: 0x%02X\r\n", prequest.b_request);
//	 d_print("w_value: 0x%04X\r\n", prequest.w_value);
//	 d_print("w_index: 0x%04X\r\n", prequest.w_index);
//	 d_print("w_length: 0x%04X\r\n", prequest.w_length);

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
//	d_print("%s()\r\n",  __func__);
	uint8_t control_state = usb_device_info->control_state;

	if ((control_state == IN_DATA) || (control_state == LAST_IN_DATA)) {
//		d_print("IN_DATA || LAST_IN_DATA\r\n");
		ep0_data_stage_in_process();
		/* ControlState may be changed outside the function */
		control_state = usb_device_info->control_state;

	} else if (control_state == WAIT_STATUS_IN) {
//		d_print("WAIT_STATUS_IN\r\n");
		if ((usb_device_info->b_request == SET_ADDRESS) &&
			(((usb_device_info->bm_request_type & (REQUEST_TYPE | REQUEST_RECIPIENT)) == DEVICE_RECIPIENT))) {
//			d_print("SET_ADDRESS: 0x%04X\r\n", usb_device_info->w_value & 0x7F);
			SetDeviceAddress(usb_device_info->w_value & 0xFF);
			usb_standard_requests->User_SetDeviceAddress();
		}

		(*usb_device_property->status_in_process)();
		control_state = STALLED;

	} else {
		d_print("ep0_in_process ERROR control_state: 0x%02X\r\n",  control_state);
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
//	d_print("%s()\r\n",  __func__);
//	d_print("EP0_RX_COUNT: 0x%03X\r\n", _GetEPRxCount(ENDP0));
	uint8_t control_state = usb_device_info->control_state;

	if ((control_state == OUT_DATA) || (control_state == LAST_OUT_DATA)) {
//		d_print("OUT_DATA\r\n");

		ep0_data_stage_out_process();

		control_state = usb_device_info->control_state;

	} else if (control_state == WAIT_STATUS_OUT) {
//		d_print("WAIT_STATUS_OUT\r\n");
		usb_device_property->status_out_process();
		control_state = STALLED;

	} else {
		d_print("ep0_out_process ERROR control_state: 0x%02X\r\n",  control_state);
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
//	 d_print("%s()\r\n",  __func__);
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
void SetDeviceAddress(uint8_t usb_addr)
{
//	d_print("%s(0x%02X)\r\n",  __func__, usb_addr);
	uint32_t i;

	for (i = 0; i < EP_COUNT; i++) {
		_SetEPAddress((uint8_t)i, (uint8_t)i);
	}

	_SetDADDR(usb_addr | DADDR_EF); /* set device address and enable function */
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
