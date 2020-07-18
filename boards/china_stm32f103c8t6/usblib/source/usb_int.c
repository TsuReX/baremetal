#include <usb_lib.h>
#include <usb_endp.h>

__IO uint16_t ep0_rx_state;
__IO uint16_t ep0_tx_state;

void (*ep_in[7])(void) = {
	ep_in_handle,
	ep_in_handle,
	ep_in_handle,
	ep_in_handle,
	ep_in_handle,
	ep_in_handle,
	ep_in_handle,
};

void (*ep_out[7])(void) = {
	ep_out_handle,
	ep_out_handle,
	ep_out_handle,
	ep_out_handle,
	ep_out_handle,
	ep_out_handle,
	ep_out_handle,
};

void ep0_handle(void)
{
	__IO uint16_t usb_ep0_register = 0;

	ep0_rx_state = _GetENDPOINT(ENDP0);
	ep0_tx_state = ep0_rx_state & EPTX_STAT;
	ep0_rx_state &=  EPRX_STAT;

	_SetEPRxTxStatus(ENDP0, EP_RX_NAK, EP_TX_NAK);

	if ((usb_irq_flags & ISTR_DIR) == 0) {

		_ClearEP_CTR_TX(ENDP0);

		ep0_in_process();

	}  else {

		usb_ep0_register = _GetENDPOINT(ENDP0);

		if ((usb_ep0_register & EP_SETUP) != 0) {

			_ClearEP_CTR_RX(ENDP0);

			ep0_setup_process();

		} else if ((usb_ep0_register & EP_CTR_RX) != 0) {

			_ClearEP_CTR_RX(ENDP0);

			ep0_out_process();
		}
	}

	_SetEPRxTxStatus(ENDP0, ep0_rx_state, ep0_tx_state);
}

void ep_handle(void)
{
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
	while (((usb_irq_flags = _GetISTR()) & ISTR_CTR) != 0) {
		ep_index = (uint8_t)(usb_irq_flags & ISTR_EP_ID);

		if (ep_index == 0) {
			ep0_handle();

		} else {
			ep_handle(/*TODO: add index argument, remove index var from the global space.*/);
		}
	}
}

void hp_ctr_handle(void)
{
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
