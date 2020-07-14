#include <usb_lib.h>
#include <usb_endp.h>

__IO uint16_t ep0_rx_state;
__IO uint16_t ep0_tx_state;

/* function pointers to non-control endpoints service routines */
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

void ep0_handle(void) {
	__IO uint16_t ep0_val = 0;

	/* Decode and service control endpoint interrupt */
	/* calling related service routine */
	/* (Setup0_Process, In0_Process, Out0_Process) */

	/* save RX & TX status */
	/* and set both to NAK */

	ep0_rx_state = _GetENDPOINT(ENDP0);
	ep0_tx_state = ep0_rx_state & EPTX_STAT;
	ep0_rx_state &=  EPRX_STAT;

	_SetEPRxTxStatus(ENDP0,EP_RX_NAK,EP_TX_NAK);

	/* DIR bit = origin of the interrupt */

	if ((wIstr & ISTR_DIR) == 0) {
		/* DIR = 0 */

		/* DIR = 0      => IN  int */
		/* DIR = 0 implies that (EP_CTR_TX = 1) always  */

		_ClearEP_CTR_TX(ENDP0);
		In0_Process();

		/* before terminate set Tx & Rx status */

		_SetEPRxTxStatus(ENDP0,ep0_rx_state,ep0_tx_state);
		return;

	}  else {
		/* DIR = 1 */

		/* DIR = 1 & CTR_RX       => SETUP or OUT int */
		/* DIR = 1 & (CTR_TX | CTR_RX) => 2 int pending */

		ep0_val = _GetENDPOINT(ENDP0);

		if ((ep0_val &EP_SETUP) != 0) {
			_ClearEP_CTR_RX(ENDP0); /* SETUP bit kept frozen while CTR_RX = 1 */
			Setup0_Process();
			/* before terminate set Tx & Rx status */

			_SetEPRxTxStatus(ENDP0,ep0_rx_state,ep0_tx_state);
			return;
		} else if ((ep0_val & EP_CTR_RX) != 0) {
			_ClearEP_CTR_RX(ENDP0);
			Out0_Process();
			/* before terminate set Tx & Rx status */

			_SetEPRxTxStatus(ENDP0,ep0_rx_state,ep0_tx_state);
			return;
		}
	}
}

void ep_handle(void) {

	__IO uint16_t ep_val = 0;
	/* Decode and service non control endpoints interrupt  */
	/* process related endpoint register */
	ep_val = _GetENDPOINT(ep_index);
	if ((ep_val & EP_CTR_RX) != 0) {
		/* clear int flag */
		_ClearEP_CTR_RX(ep_index);

		/* call OUT service function */
		(*ep_out[ep_index - 1])();

	} /* if((wEPVal & EP_CTR_RX) */

	if ((ep_val & EP_CTR_TX) != 0) {
		/* clear int flag */
		_ClearEP_CTR_TX(ep_index);

		/* call IN service function */
		(*ep_in[ep_index - 1])();
	} /* if((wEPVal & EP_CTR_TX) != 0) */

}

void lp_ctr_handle( void)
{

	/* stay in loop while pending interrupts */
	while (((wIstr = _GetISTR()) & ISTR_CTR) != 0) {
		/* extract highest priority endpoint number */
		ep_index = (uint8_t)(wIstr & ISTR_EP_ID);

		if (ep_index == 0) {
			ep0_handle();

		} else {
			ep_handle(/*TODO: add index argument, remove index var from the global space.*/);

		}

	}
}

void hp_ctr_handle(void)
{
	uint32_t ep_val = 0;

	while (((wIstr = _GetISTR()) & ISTR_CTR) != 0) {
		_SetISTR((uint16_t)CLR_CTR); /* clear CTR flag */
		/* extract highest priority endpoint number */
		ep_index = (uint8_t)(wIstr & ISTR_EP_ID);
		/* process related endpoint register */
		ep_val = _GetENDPOINT(ep_index);

		if ((ep_val & EP_CTR_RX) != 0) {
			/* clear int flag */
			_ClearEP_CTR_RX(ep_index);

			/* call OUT service function */
			(*ep_out[ep_index-1])();

		} else if ((ep_val & EP_CTR_TX) != 0) {
			/* clear int flag */
			_ClearEP_CTR_TX(ep_index);

			/* call IN service function */
			(*ep_in[ep_index - 1])();

		}
	}
}
