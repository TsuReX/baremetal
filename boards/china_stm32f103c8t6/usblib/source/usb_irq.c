#include "usb_core.h"
#include "usb_prop.h"
#include "usb_endp.h"

#include "console.h"

//__IO uint16_t	usb_irq_flags;  /* ISTR register last read value */
//__IO uint8_t	bIntPackSOF = 0;  /* SOFs received between 2 consecutive packets */
//__IO uint32_t	esof_counter = 0; /* expected SOF counter */
//__IO uint32_t	wCNTR	= 0;

__IO uint16_t ep0_rx_state;
__IO uint16_t ep0_tx_state;

//extern uint8_t	ep_index;
__IO uint32_t	bDeviceState = UNCONNECTED; /* USB device status */
//bool			fSuspendEnabled = FALSE;  /* true when suspend is possible */
//uint32_t		endpoints[8];

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
	 d_print("%s() USB_EP0R: 0x%04X\r\n",  __func__, usb_ep0_register);

	ep0_rx_state = usb_ep0_register;
	ep0_tx_state = ep0_rx_state & EPTX_STAT;
	ep0_rx_state &=  EPRX_STAT;

	_SetEPRxTxStatus(ENDP0, EP_RX_NAK, EP_TX_NAK);

	if ((_GetISTR() & ISTR_DIR) == 0) {
		_ClearEP_CTR_TX(ENDP0);
		ep0_in_process();
		d_print("EP0_TX_COUNT: 0x%04X\r\n", _GetEPTxCount(ENDP0));
	}  else {

		d_print("EP0_RX_COUNT: 0x%04X\r\n", _GetEPRxCount(ENDP0));
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
			d_print("ERROR state r: 0x%04X\r\n",  usb_ep0_register);
		}
	}
//	d_print("ep0_rx_state: 0x%04X, ep0_tx_state: 0x%04X\r\n",  ep0_rx_state, ep0_tx_state);

	_SetEPRxTxStatus(ENDP0, ep0_rx_state, ep0_tx_state);
}

void ep_handle(uint32_t ep_index)
{
	 d_print("%s()\r\n",  __func__);
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
//	d_print("%s()\r\n",  __func__);
	uint16_t usb_irq_flags = _GetISTR();
	uint32_t ep_index;
	while ((usb_irq_flags & ISTR_CTR) != 0) {

		ep_index = (uint32_t)(usb_irq_flags & ISTR_EP_ID);

		if (ep_index == 0) {
			ep0_handle();
		} else {
			ep_handle(ep_index);
		}
		usb_irq_flags = _GetISTR();
	}
}

void hp_ctr_handle(void)
{
//	d_print("%s()\r\n",  __func__);
	uint32_t usb_ep_register = 0;
	uint32_t ep_index;
	uint16_t usb_irq_flags = _GetISTR();

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

void suspend(void)
{
	uint32_t ep_ind = 0;
	uint16_t cntr = 0;
	uint16_t epxreg[EP_COUNT];

	cntr = _GetCNTR();

	/*Store endpoints registers status */
	for (ep_ind = 0; ep_ind < EP_COUNT; ep_ind++)
		epxreg[ep_ind] = _GetENDPOINT(ep_ind);

	/* unmask RESET flag */
	cntr |= CNTR_RESETM;
	_SetCNTR(cntr);

	cntr &= ~CNTR_RESETM;
	_SetCNTR(cntr);

	/*apply FRES */
	cntr |= CNTR_FRES;
	_SetCNTR(cntr);

	/*clear FRES*/
	cntr &= ~CNTR_FRES;
	_SetCNTR(cntr);

	/*poll for RESET flag in ISTR*/
	while ((_GetISTR() & ISTR_RESET) == 0);

	/* clear RESET flag in ISTR */
	_SetISTR((uint16_t)CLR_RESET);

	/*restore Enpoints*/
	for (ep_ind = 0; ep_ind < EP_COUNT; ep_ind++)
		_SetENDPOINT(ep_ind, epxreg[ep_ind]);

	/* Now it is safe to enter macrocell in suspend mode */
	cntr |= CNTR_FSUSP;
	_SetCNTR(cntr);

	/* force low-power mode in the macrocell */
	cntr = _GetCNTR();
	cntr |= CNTR_LPMODE;
	_SetCNTR(cntr);

	/* enter system in STOP mode, only when wakeup flag in not set */
	if((_GetISTR() & ISTR_WKUP) == 0) {
//		__WFI();

	} else {
		/* Clear Wakeup flag */
		_SetISTR(CLR_WKUP);
		/* clear FSUSP to abort entry in suspend mode  */
		cntr = _GetCNTR();
		cntr &= ~CNTR_FSUSP;
		_SetCNTR(cntr);
	}
}

void resume(void)
{
	uint16_t cntr = _GetCNTR();

	cntr &= (~CNTR_LPMODE);
	_SetCNTR(cntr);

	_SetCNTR(_GetCNTR() & ~CNTR_FSUSP);
}

void usb_lp_can1_rx0_handle(void)
{
	uint16_t usb_irq_flags = _GetISTR();
//	d_print("\r\n%s() begin ISTR: 0x%04X\r\n", __func__, usb_irq_flags);

//	if (usb_irq_flags & ISTR_SOF) {
////		 d_print("ISTR_SOF\r\n");
//		_SetISTR(CLR_SOF);
//	}

	if (usb_irq_flags & ISTR_CTR) {
		d_print("ISTR_CTR\r\n");
		lp_ctr_handle();
		_SetISTR(CLR_CTR);
	}

	if (usb_irq_flags & ISTR_RESET) {
		_SetISTR(CLR_RESET);
//		if ((_GetFNR() & (FNR_RXDM | FNR_RXDP)) == 0x0) {
			LL_GPIO_SetOutputPin(GPIOA, LL_GPIO_PIN_7);
//			d_print("ISTR_RESET\r\n");
			d_print("ISTR_RESET USB_FNR: 0x%04X\r\n", _GetFNR() & (FNR_RXDM | FNR_RXDP));
			reset();
//			LL_mDelay(1);
			LL_GPIO_ResetOutputPin(GPIOA, LL_GPIO_PIN_7);
//		} else {
//		}
	}

//	if (usb_irq_flags & ISTR_DOVR) {
//		_SetISTR(CLR_DOVR);
//	}
//
//	if (usb_irq_flags & ISTR_ERR) {
//		_SetISTR(CLR_ERR);
//	}
//
//	if (usb_irq_flags & ISTR_WKUP) {
//		uint16_t usb_fnr = _GetFNR();
//		if ((usb_fnr & (FNR_RXDM | FNR_RXDP)) == (FNR_RXDM)) {
//			d_print("ISTR_WKUP\r\n");
//			d_print("USB_FNR: 0x%04X\r\n", usb_fnr);
//			resume();
//		}
//		_SetISTR(CLR_WKUP);
//	}
//
//	if (usb_irq_flags & ISTR_ESOF) {
////		 d_print("ISTR_ESOF\r\n");
//		_SetISTR(CLR_ESOF);
//	}
//
//	if (usb_irq_flags & ISTR_SUSP) {
////		 d_print("ISTR_SUSP\r\n");
////		 suspend();
//		_SetISTR(CLR_SUSP);
//	}
//
//	d_print("%s() end ISTR: 0x%04X\r\n", __func__, _GetISTR());
}

void usbwakeup_handle(void)
{
	d_print("usbwakeup_handle() begin ISTR: 0x%04X\r\n", _GetISTR());
	LL_EXTI_ClearFlag_0_31(LL_EXTI_LINE_18);
//	LL_EXTI_DisableEvent_0_31(LL_EXTI_LINE_18);
//	LL_EXTI_DisableIT_0_31(LL_EXTI_LINE_18);
	d_print("usbwakeup_handle() end ISTR: 0x%04X\r\n", _GetISTR());
}
