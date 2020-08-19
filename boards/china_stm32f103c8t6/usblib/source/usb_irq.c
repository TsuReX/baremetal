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

/*******************************************************************************
* Function Name  : PowerOn
* Description    :
* Input          : None.
* Output         : None.
* Return         : USB_SUCCESS.
*******************************************************************************/
RESULT PowerOn(void)
{
	// print("%s()\r\n",  __func__);

	_SetISTR(0);

//	_SetCNTR(CNTR_RESETM | CNTR_SUSPM | CNTR_WKUPM | CNTR_PDWN);

	return USB_SUCCESS;
}

/*******************************************************************************
* Function Name  : PowerOff
* Description    : handles switch-off conditions
* Input          : None.
* Output         : None.
* Return         : USB_SUCCESS.
*******************************************************************************/
RESULT PowerOff()
{
	// print("%s()\r\n",  __func__);

	_SetCNTR(CNTR_FRES);

	_SetISTR(0);

	_SetCNTR(CNTR_FRES + CNTR_PDWN);

	return USB_SUCCESS;
}

/*******************************************************************************
* Function Name  : Suspend
* Description    : sets suspend mode operating conditions
* Input          : None.
* Output         : None.
* Return         : USB_SUCCESS.
*******************************************************************************/
void suspend(void)
{
	// print("%s()\r\n",  __func__);
	uint32_t ep_ind = 0;
	uint16_t w_cntr = 0;
	uint32_t tmp_pwr_cr = 0;
	uint32_t pwr_cr = 0;

	/*Store CNTR value */
	w_cntr = _GetCNTR();

	/*Store endpoints registers status */
	for (ep_ind = 0; ep_ind < EP_COUNT; ep_ind++)
		endpoints[ep_ind] = _GetENDPOINT(ep_ind);

	/* unmask RESET flag */
	w_cntr |= CNTR_RESETM;
	_SetCNTR(w_cntr);

	/*apply FRES */
	w_cntr |= CNTR_FRES;
	_SetCNTR(w_cntr);

	/*clear FRES*/
	w_cntr &= ~CNTR_FRES;
	_SetCNTR(w_cntr);

	/*poll for RESET flag in ISTR*/
	while((_GetISTR()&ISTR_RESET) == 0);

	/* clear RESET flag in ISTR */
	_SetISTR(CLR_RESET);

	/*restore Enpoints*/
	for (ep_ind = 0; ep_ind < EP_COUNT; ep_ind++)
		_SetENDPOINT(ep_ind, endpoints[ep_ind]);

	/* Now it is safe to enter macrocell in suspend mode */
	w_cntr |= CNTR_FSUSP;
	_SetCNTR(w_cntr);

	/* force low-power mode in the macrocell */
//	w_cntr = _GetCNTR();
	w_cntr |= CNTR_LPMODE;
	_SetCNTR(w_cntr);

	/*prepare entry in low power mode (STOP mode)*/
	/* Select the regulator state in STOP mode*/
	pwr_cr = PWR->CR;
	tmp_pwr_cr = PWR->CR;
	/* Clear PDDS and LPDS bits */
	tmp_pwr_cr &= ((uint32_t)0xFFFFFFFC);
	/* Set LPDS bit according to PWR_Regulator value */
	tmp_pwr_cr |= PWR_CR_LPDS;
	/* Store the new value */
	PWR->CR = tmp_pwr_cr;
	/* Set SLEEPDEEP bit of Cortex System Control Register */
        SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;

	/* enter system in STOP mode, only when wakeup flag in not set */
	if((_GetISTR() & ISTR_WKUP) == 0) {
		__WFI();
		/* Reset SLEEPDEEP bit of Cortex System Control Register */
		SCB->SCR &= (uint32_t)~((uint32_t)SCB_SCR_SLEEPDEEP_Msk);
	} else {
		/* Clear Wakeup flag */
		_SetISTR(CLR_WKUP);
		/* clear FSUSP to abort entry in suspend mode  */
        w_cntr = _GetCNTR();
        w_cntr &= ~CNTR_FSUSP;
        _SetCNTR(w_cntr);

		/*restore sleep mode configuration */
		/* restore Power regulator config in sleep mode*/
		PWR->CR = pwr_cr;

		/* Reset SLEEPDEEP bit of Cortex System Control Register */
		SCB->SCR &= (uint32_t)~((uint32_t)SCB_SCR_SLEEPDEEP_Msk);
    }
}

/*******************************************************************************
* Function Name  : Leave_LowPowerMode
* Description    : Restores system clocks and power while exiting suspend mode
* Input          : None.
* Return         : None.
*******************************************************************************/
void Leave_LowPowerMode(void)
{
	// print("%s()\r\n",  __func__);
	DEVICE_INFO *pInfo = &device_info;

	/* Set the device state to the correct state */
	if (pInfo->Current_Configuration != 0) {
		/* Device configured */
		bDeviceState = CONFIGURED;
	} else {
			bDeviceState = ATTACHED;
	}
	/*Enable SystemCoreClock*/
	SystemInit();
}

/*******************************************************************************
* Function Name  : Resume_Init
* Description    : Handles wake-up restoring normal operations
* Input          : None.
* Output         : None.
* Return         : USB_SUCCESS.
*******************************************************************************/
void Resume_Init(void)
{
	// print("%s()\r\n",  __func__);
	uint16_t wCNTR;

	/* CNTR_LPMODE = 0 */
	wCNTR = _GetCNTR();
	wCNTR &= (~CNTR_LPMODE);
	_SetCNTR(wCNTR);

	/* restore full power */
	/* ... on connected devices */
	Leave_LowPowerMode();

	/* reset FSUSP bit */
	_SetCNTR(IMR_MSK);

}

/*******************************************************************************
* Function Name  : Resume
* Description    : This is the state machine handling resume operations and
*                 timing sequence. The control is based on the Resume structure
*                 variables and on the ESOF interrupt calling this subroutine
*                 without changing machine state.
* Input          : a state machine value (RESUME_STATE)
*                  RESUME_ESOF doesn't change ResumeS.eState allowing
*                  decrementing of the ESOF counter in different states.
* Output         : None.
* Return         : None.
*******************************************************************************/
void resume(RESUME_STATE eResumeSetVal)
{
//	// print("%s(0x%04X)\r\n",  __func__, eResumeSetVal);
	uint16_t wCNTR;

	if (eResumeSetVal != RESUME_ESOF)
		ResumeS.eState = eResumeSetVal;

	switch (ResumeS.eState) {

		case RESUME_EXTERNAL:
			if (remotewakeupon ==0) {
				Resume_Init();
				ResumeS.eState = RESUME_OFF;
			} else {/* RESUME detected during the RemoteWAkeup signalling => keep RemoteWakeup handling*/
				ResumeS.eState = RESUME_ON;
			}
			break;

		case RESUME_INTERNAL:
			Resume_Init();
			ResumeS.eState = RESUME_START;
			remotewakeupon = 1;
			break;

		case RESUME_LATER:
			ResumeS.bESOFcnt = 2;
			ResumeS.eState = RESUME_WAIT;
			break;

		case RESUME_WAIT:
			ResumeS.bESOFcnt--;
			if (ResumeS.bESOFcnt == 0)
				ResumeS.eState = RESUME_START;
			break;

		case RESUME_START:
			wCNTR = _GetCNTR();
			wCNTR |= CNTR_RESUME;
			_SetCNTR(wCNTR);
			ResumeS.eState = RESUME_ON;
			ResumeS.bESOFcnt = 10;
			break;

		case RESUME_ON:
			ResumeS.bESOFcnt--;
			if (ResumeS.bESOFcnt == 0) {
				wCNTR = _GetCNTR();
				wCNTR &= (~CNTR_RESUME);
				_SetCNTR(wCNTR);
				ResumeS.eState = RESUME_OFF;
				remotewakeupon = 0;
			}
			break;

		case RESUME_OFF:
		case RESUME_ESOF:
		default:
			ResumeS.eState = RESUME_OFF;
			break;
	}
}

void ep0_handle(void)
{
	// print("%s()\r\n",  __func__);
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

		} else {

//			_ClearEP_CTR_TX(ENDP0);
//			ep0_in_process();
			// print("Error! usb_ep0_register: 0x%04X\r\n",  usb_ep0_register);
		}
	}

	_SetEPRxTxStatus(ENDP0, ep0_rx_state, ep0_tx_state);
}

void ep_handle(void)
{
	// print("%s()\r\n",  __func__);
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
//	// print("%s()\r\n",  __func__);
	while ((usb_irq_flags & ISTR_CTR) != 0) {
	usb_irq_flags = _GetISTR();
//	// print("ISTR: 0x%04X\r\n", usb_irq_flags);

		ep_index = (uint8_t)(usb_irq_flags & ISTR_EP_ID);

		if (ep_index == 0) {
			ep0_handle();
//			// print("ISTR: 0x%04X\r\n", _GetISTR());
		} else {
			ep_handle(/*TODO: add index argument, remove index var from the global space.*/);
		}
	}
}

void hp_ctr_handle(void)
{
	// print("%s()\r\n",  __func__);
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

void esof_handle()
{
	uint32_t ep_ind = 0;
	uint32_t endpoints[EP_COUNT];
	uint16_t w_fnr = _GetFNR();

//	// print("%s() FNR: 0x%04X\r\n",  __func__, w_fnr);

	if ((w_fnr & FNR_RXDP) != 0) {
		/* increment ESOF counter */
		esof_counter++;

		/* test if we enter in ESOF more than 3 times with FSUSP =0 and RXDP =1=>> possible missing SUSP flag*/
		if ((esof_counter > 3) && ((_GetCNTR() & CNTR_FSUSP) == 0)) {
			/* this a sequence to apply a force RESET*/
			/*Store CNTR value */
			wCNTR = _GetCNTR();

			/*Store endpoints registers status */
			for (ep_ind = 0; ep_ind < 8; ep_ind++) {
				endpoints[ep_ind] = _GetENDPOINT(ep_ind);
			}

			/*apply FRES */
			wCNTR |= CNTR_FRES;
			_SetCNTR(wCNTR);

			/*clear FRES*/
			wCNTR &= ~CNTR_FRES;
			_SetCNTR(wCNTR);

			/*poll for RESET flag in ISTR*/
			while ((_GetISTR() & ISTR_RESET) == 0);

			/* clear RESET flag in ISTR */
			_SetISTR((uint16_t)CLR_RESET);

			/*restore Enpoints*/
			for (ep_ind = 0; ep_ind < 8; ep_ind++) {
				_SetENDPOINT(ep_ind, endpoints[ep_ind]);
			}
			esof_counter = 0;
		}
	} else { /* if ((_GetFNR() & FNR_RXDP)!=0) */
		esof_counter = 0;
	}
	/* resume handling timing is made with ESOFs */
	resume(RESUME_ESOF); /* request without change of the machine state */
}

void usb_lp_can1_rx0_handle(void)
{
	usb_irq_flags = _GetISTR();
//	// print("\r\n%s() begin ISTR: 0x%04X\r\n", __func__, usb_irq_flags);

	if (usb_irq_flags & ISTR_SOF) {
//		// print("ISTR_SOF\r\n");
		_SetISTR(CLR_SOF);
	}

	if (usb_irq_flags & ISTR_CTR) {
		 print("ISTR_CTR\r\n");
		lp_ctr_handle();
	}

	if (usb_irq_flags & ISTR_RESET) {
		_SetISTR(CLR_RESET);
		 print("ISTR_RESET: ISTR 0x%04X\r\n", _GetISTR());
		property.reset();
	}

	if (usb_irq_flags & ISTR_DOVR) {
		_SetISTR(CLR_DOVR);
	}

	if (usb_irq_flags & ISTR_ERR) {
		_SetISTR(CLR_ERR);
	}

	if (usb_irq_flags & ISTR_WKUP) {
		 print("ISTR_WKUP\r\n");
//		resume(RESUME_EXTERNAL);

		uint16_t cntr = _GetCNTR();
		cntr &= (~CNTR_LPMODE);
		_SetCNTR(cntr);

		/* reset FSUSP bit */
//		_SetCNTR(IMR_MSK);
		cntr = _GetCNTR();
		cntr &= (~CNTR_FSUSP);
		_SetCNTR(cntr);

		_SetISTR(CLR_WKUP);
	}

	if (usb_irq_flags & ISTR_ESOF) {
		 print("ISTR_ESOF\r\n");
		_SetISTR(CLR_ESOF);
//		esof_handle();
	}

	if (usb_irq_flags & ISTR_SUSP) {
		 print("ISTR_SUSP\r\n");
		_SetCNTR(CNTR_FSUSP);
		_SetISTR(CLR_SUSP);
	}

//	// print("%s() end ISTR: 0x%04X\r\n", __func__, _GetISTR());
}

void usbwakeup_handle(void)
{
	 print("%s()\r\n",  __func__);
	LL_EXTI_ClearFlag_0_31(LL_EXTI_LINE_18);
}
