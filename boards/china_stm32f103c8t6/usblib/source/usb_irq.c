#include "usb_core.h"
#include "usb_prop.h"
#include "usb_endp.h"

#include "console.h"

__IO uint16_t	usb_irq_flags;  /* ISTR register last read value */
__IO uint8_t	bIntPackSOF = 0;  /* SOFs received between 2 consecutive packets */
__IO uint32_t	esof_counter = 0; /* expected SOF counter */
__IO uint32_t	wCNTR	= 0;

__IO uint16_t ep0_rx_state;
__IO uint16_t ep0_tx_state;

extern uint8_t	ep_index;
__IO uint32_t	bDeviceState = UNCONNECTED; /* USB device status */
bool			fSuspendEnabled = FALSE;  /* true when suspend is possible */
uint32_t		endpoints[8];

void (*ep_in[EP_COUNT])(void) = {
	ep_in_handle,
	ep_in_handle
};

void (*ep_out[EP_COUNT])(void) = {
	ep_out_handle,
	ep_out_handle
};

struct {
	__IO RESUME_STATE eState;
	__IO uint8_t bESOFcnt;
} ResumeS;

__IO uint32_t remotewakeupon=0;

void ep0_handle(void)
{
	__IO uint16_t usb_ep0_register = _GetENDPOINT(ENDP0);
	 print("\r\n%s() USB_EP0R: 0x%04X\r\n",  __func__, usb_ep0_register);

	ep0_rx_state = usb_ep0_register;
	ep0_tx_state = ep0_rx_state & EPTX_STAT;
	ep0_rx_state &=  EPRX_STAT;

	_SetEPRxTxStatus(ENDP0, EP_RX_NAK, EP_TX_NAK);

	if ((usb_irq_flags & ISTR_DIR) == 0) {
		_ClearEP_CTR_TX(ENDP0);
		ep0_in_process();
		print("EP0_TX_COUNT: 0x%04X\r\n", _GetEPTxCount(ENDP0));
	}  else {

		print("EP0_RX_COUNT: 0x%04X\r\n", _GetEPRxCount(ENDP0));
//		usb_ep0_register = _GetENDPOINT(ENDP0);
		if ((usb_ep0_register & EP_SETUP) != 0) {

			_ClearEP_CTR_RX(ENDP0);
			ep0_setup_process();

		} else if ((usb_ep0_register & EP_CTR_RX) != 0) {

			_ClearEP_CTR_RX(ENDP0);
			ep0_out_process();

		} else {

//			_ClearEP_CTR_TX(ENDP0);
//			ep0_in_process();
			print("ERROR state r: 0x%04X\r\n",  usb_ep0_register);
		}
	}
//	print("ep0_rx_state: 0x%04X, ep0_tx_state: 0x%04X\r\n",  ep0_rx_state, ep0_tx_state);

	_SetEPRxTxStatus(ENDP0, ep0_rx_state, ep0_tx_state);
}

void ep_handle(void)
{
	 print("%s()\r\n",  __func__);
	__IO uint16_t usb_ep_register = 0;

	usb_ep_register = _GetENDPOINT(ep_index);

	if ((usb_ep_register & EP_CTR_RX) != 0) {

		_ClearEP_CTR_RX(ep_index);

		(*ep_out[ep_index - 1])();
	}

	if ((usb_ep_register & EP_CTR_TX) != 0) {

		_ClearEP_CTR_TX(ep_index);

		(*ep_in[ep_index - 1])();
	}
}

void lp_ctr_handle( void)
{
//	print("%s()\r\n",  __func__);
	usb_irq_flags = _GetISTR();
	while ((usb_irq_flags & ISTR_CTR) != 0) {

		ep_index = (uint8_t)(usb_irq_flags & ISTR_EP_ID);

		if (ep_index == 0) {
			ep0_handle();
		} else {
			ep_handle(/*TODO: add index argument, remove index var from the global space.*/);
		}
		usb_irq_flags = _GetISTR();
	}
}

void hp_ctr_handle(void)
{
//	print("%s()\r\n",  __func__);
	uint32_t usb_ep_register = 0;

	usb_irq_flags = _GetISTR();

	while ((usb_irq_flags & ISTR_CTR) != 0) {

		_SetISTR((uint16_t)CLR_CTR);

		ep_index = (uint8_t)(usb_irq_flags & ISTR_EP_ID);

		usb_ep_register = _GetENDPOINT(ep_index);

		if ((usb_ep_register & EP_CTR_RX) != 0) {

			_ClearEP_CTR_RX(ep_index);

			(*ep_out[ep_index-1])();

		} else if ((usb_ep_register & EP_CTR_TX) != 0) {

			_ClearEP_CTR_TX(ep_index);

			(*ep_in[ep_index - 1])();
		}
	}
}

void usb_lp_can1_rx0_handle(void)
{
	usb_irq_flags = _GetISTR();
//	print("\r\n%s() begin ISTR: 0x%04X\r\n", __func__, usb_irq_flags);

	if (usb_irq_flags & ISTR_SOF) {
//		 print("ISTR_SOF\r\n");
		_SetISTR(CLR_SOF);
	}

	if (usb_irq_flags & ISTR_CTR) {
//		print("ISTR_CTR\r\n");
		lp_ctr_handle();
	}

	if (usb_irq_flags & ISTR_RESET) {
		_SetISTR(CLR_RESET);
//		print("ISTR_RESET: ISTR 0x%04X\r\n", _GetISTR());
		property.reset();
	}

	if (usb_irq_flags & ISTR_DOVR) {
		_SetISTR(CLR_DOVR);
	}

	if (usb_irq_flags & ISTR_ERR) {
		_SetISTR(CLR_ERR);
	}

	if (usb_irq_flags & ISTR_WKUP) {
//		print("ISTR_WKUP\r\n");

		uint16_t cntr = _GetCNTR();
		cntr &= (~CNTR_LPMODE);
		_SetCNTR(cntr);

		cntr = _GetCNTR();
		cntr &= (~CNTR_FSUSP);
		_SetCNTR(cntr);

		_SetISTR(CLR_WKUP);
	}

	if (usb_irq_flags & ISTR_ESOF) {
//		 print("ISTR_ESOF\r\n");
		_SetISTR(CLR_ESOF);
	}

	if (usb_irq_flags & ISTR_SUSP) {
//		 print("ISTR_SUSP\r\n");
		_SetCNTR(CNTR_FSUSP);
		_SetISTR(CLR_SUSP);
	}

//	print("%s() end ISTR: 0x%04X\r\n", __func__, _GetISTR());
}

void usbwakeup_handle(void)
{
	print("%s()\r\n",  __func__);
	LL_EXTI_ClearFlag_0_31(LL_EXTI_LINE_18);
}
