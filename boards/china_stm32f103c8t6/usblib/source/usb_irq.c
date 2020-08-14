#include <usb_endp.h>
#include <usb_lib.h>
#include <usb_prop.h>
#include <usb_lib.h>
#include <usb_endp.h>

#include <console.h>


__IO uint16_t usb_irq_flags;  /* ISTR register last read value */
__IO uint8_t bIntPackSOF = 0;  /* SOFs received between 2 consecutive packets */
__IO uint32_t esof_counter = 0; /* expected SOF counter */
__IO uint32_t wCNTR	= 0;

__IO uint16_t ep0_rx_state;
__IO uint16_t ep0_tx_state;

/*  The number of current endpoint, it will be used to specify an endpoint */
extern uint8_t	ep_index;
extern uint16_t	wInterrupt_Mask;
__IO uint32_t bDeviceState = UNCONNECTED; /* USB device status */
__IO bool fSuspendEnabled = TRUE;  /* true when suspend is possible */
__IO uint32_t EP[8];
extern uint16_t	wInterrupt_Mask;

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


struct {
	__IO RESUME_STATE eState;
	__IO uint8_t bESOFcnt;
} ResumeS;

__IO uint32_t remotewakeupon=0;

/*******************************************************************************
* Function Name  : USB_Cable_Config
* Description    : Software Connection/Disconnection of USB Cable
* Input          : None.
* Return         : Status
*******************************************************************************/
void USB_Cable_Config (FunctionalState NewState)
{
}

/*******************************************************************************
* Function Name  : PowerOn
* Description    :
* Input          : None.
* Output         : None.
* Return         : USB_SUCCESS.
*******************************************************************************/
RESULT PowerOn(void)
{
	d_print("%s()\r\n",  __func__);
  uint16_t wRegVal;

  /*** cable plugged-in ? ***/
  USB_Cable_Config(ENABLE);

  /*** CNTR_PWDN = 0 ***/
  wRegVal = CNTR_FRES;
  _SetCNTR(wRegVal);

  /*** CNTR_FRES = 0 ***/
//  wInterrupt_Mask = 0;
//  _SetCNTR(wInterrupt_Mask);
  /*** Clear pending interrupts ***/
  _SetISTR(0);
  /*** Set interrupt mask ***/
  wInterrupt_Mask = CNTR_RESETM | CNTR_SUSPM | CNTR_WKUPM | CNTR_PDWN;
  _SetCNTR(wInterrupt_Mask);

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
	d_print("%s()\r\n",  __func__);
  /* disable all interrupts and force USB reset */
  _SetCNTR(CNTR_FRES);
  /* clear interrupt status register */
  _SetISTR(0);
  /* Disable the Pull-Up*/
  USB_Cable_Config(DISABLE);
  /* switch-off device */
  _SetCNTR(CNTR_FRES + CNTR_PDWN);
  /* sw variables reset */
  /* ... */

  return USB_SUCCESS;
}

/*******************************************************************************
* Function Name  : Suspend
* Description    : sets suspend mode operating conditions
* Input          : None.
* Output         : None.
* Return         : USB_SUCCESS.
*******************************************************************************/
void Suspend(void)
{
	d_print("%s()\r\n",  __func__);
	uint32_t i =0;
	uint16_t wCNTR;
	uint32_t tmpreg = 0;
	__IO uint32_t savePWR_CR=0;
	/* suspend preparation */
	/* ... */

	/*Store CNTR value */
	wCNTR = _GetCNTR();

	/* This a sequence to apply a force RESET to handle a robustness case */

	/*Store endpoints registers status */
	for (i = 0; i < 8; i++)
		EP[i] = _GetENDPOINT(i);

	/* unmask RESET flag */
	wCNTR |= CNTR_RESETM;
	_SetCNTR(wCNTR);

	/*apply FRES */
	wCNTR |= CNTR_FRES;
	_SetCNTR(wCNTR);

	/*clear FRES*/
	wCNTR &= ~CNTR_FRES;
	_SetCNTR(wCNTR);

	/*poll for RESET flag in ISTR*/
	while((_GetISTR()&ISTR_RESET) == 0);

	d_print("%s() 2\r\n",  __func__);
	/* clear RESET flag in ISTR */
	_SetISTR((uint16_t)CLR_RESET);

	/*restore Enpoints*/
	for (i = 0; i < 8; i++)
		_SetENDPOINT(i, EP[i]);

	/* Now it is safe to enter macrocell in suspend mode */
	wCNTR |= CNTR_FSUSP;
	_SetCNTR(wCNTR);

	/* force low-power mode in the macrocell */
	wCNTR = _GetCNTR();
	wCNTR |= CNTR_LPMODE;
	_SetCNTR(wCNTR);

	/*prepare entry in low power mode (STOP mode)*/
	/* Select the regulator state in STOP mode*/
	savePWR_CR = PWR->CR;
	tmpreg = PWR->CR;
	/* Clear PDDS and LPDS bits */
	tmpreg &= ((uint32_t)0xFFFFFFFC);
	/* Set LPDS bit according to PWR_Regulator value */
	tmpreg |= PWR_CR_LPDS;
	/* Store the new value */
	PWR->CR = tmpreg;
	/* Set SLEEPDEEP bit of Cortex System Control Register */
#if defined (STM32F30X) || defined (STM32F37X)
        SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;
#else
        SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;
#endif

	/* enter system in STOP mode, only when wakeup flag in not set */
	if((_GetISTR()&ISTR_WKUP)==0) {
		__WFI();
		/* Reset SLEEPDEEP bit of Cortex System Control Register */
#if defined (STM32F30X) || defined (STM32F37X)
		SCB->SCR &= (uint32_t)~((uint32_t)SCB_SCR_SLEEPDEEP_Msk);
#else
		SCB->SCR &= (uint32_t)~((uint32_t)SCB_SCR_SLEEPDEEP_Msk);
#endif
	} else {
		/* Clear Wakeup flag */
		_SetISTR(CLR_WKUP);
		/* clear FSUSP to abort entry in suspend mode  */
        wCNTR = _GetCNTR();
        wCNTR &= ~CNTR_FSUSP;
        _SetCNTR(wCNTR);

		/*restore sleep mode configuration */
		/* restore Power regulator config in sleep mode*/
		PWR->CR = savePWR_CR;

		/* Reset SLEEPDEEP bit of Cortex System Control Register */
#if defined (STM32F30X) || defined (STM32F37X)
		SCB->SCR &= (uint32_t)~((uint32_t)SCB_SCR_SLEEPDEEP_Msk);
#else
		SCB->SCR &= (uint32_t)~((uint32_t)SCB_SCR_SLEEPDEEP_Msk);
#endif
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
	d_print("%s()\r\n",  __func__);
  DEVICE_INFO *pInfo = &device_info;

  /* Set the device state to the correct state */
  if (pInfo->Current_Configuration != 0)
  {
    /* Device configured */
    bDeviceState = CONFIGURED;
  }
  else
  {
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
	d_print("%s()\r\n",  __func__);
  uint16_t wCNTR;

  /* ------------------ ONLY WITH BUS-POWERED DEVICES ---------------------- */
  /* restart the clocks */
  /* ...  */

  /* CNTR_LPMODE = 0 */
  wCNTR = _GetCNTR();
  wCNTR &= (~CNTR_LPMODE);
  _SetCNTR(wCNTR);

  /* restore full power */
  /* ... on connected devices */
  Leave_LowPowerMode();

  /* reset FSUSP bit */
  _SetCNTR(IMR_MSK);

  /* reverse suspend preparation */
  /* ... */

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
void Resume(RESUME_STATE eResumeSetVal)
{
	d_print("%s()\r\n",  __func__);
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
	d_print("%s()\r\n",  __func__);
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
	d_print("%s()\r\n",  __func__);
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
	d_print("%s()\r\n",  __func__);
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

void USB_LP_CAN1_RX0_IRQHandler(void)
{
	d_print("%s()\r\n",  __func__);
	uint32_t i=0;
	__IO uint32_t EP[8];

	usb_irq_flags = _GetISTR();

/***********************************************************************/
	if (usb_irq_flags & ISTR_SOF & wInterrupt_Mask) {
		_SetISTR((uint16_t)CLR_SOF);
//		bIntPackSOF++;
	}

/***********************************************************************/
  
	if (usb_irq_flags & ISTR_CTR & wInterrupt_Mask) {
		/* servicing of the endpoint correct transfer interrupt */
		/* clear of the CTR flag into the sub */
		lp_ctr_handle();
	}

/***********************************************************************/

	if (usb_irq_flags & ISTR_RESET & wInterrupt_Mask) {
		_SetISTR((uint16_t)CLR_RESET);
		property.reset();
	}

/***********************************************************************/

	if (usb_irq_flags & ISTR_DOVR & wInterrupt_Mask) {
		_SetISTR((uint16_t)CLR_DOVR);
	}

/***********************************************************************/

	if (usb_irq_flags & ISTR_ERR & wInterrupt_Mask) {
		_SetISTR((uint16_t)CLR_ERR);
	}

/***********************************************************************/

	if (usb_irq_flags & ISTR_WKUP & wInterrupt_Mask) {
		_SetISTR((uint16_t)CLR_WKUP);
		Resume(RESUME_EXTERNAL);
	}

/***********************************************************************/

	if (usb_irq_flags & ISTR_SUSP & wInterrupt_Mask) {

		/* check if SUSPEND is possible */
		if (fSuspendEnabled) {
			Suspend();
		} else {
			/* if not possible then resume after xx ms */
			Resume(RESUME_LATER);
		}
		/* clear of the ISTR bit must be done after setting of CNTR_FSUSP */
		_SetISTR((uint16_t)CLR_SUSP);
	}

/***********************************************************************/

	if (usb_irq_flags & ISTR_ESOF & wInterrupt_Mask) {
		/* clear ESOF flag in ISTR */
		_SetISTR((uint16_t)CLR_ESOF);

		if ((_GetFNR() & FNR_RXDP) != 0) {
			/* increment ESOF counter */
			esof_counter++;

			/* test if we enter in ESOF more than 3 times with FSUSP =0 and RXDP =1=>> possible missing SUSP flag*/
			if ((esof_counter > 3) && ((_GetCNTR() & CNTR_FSUSP) == 0)) {
				/* this a sequence to apply a force RESET*/
				/*Store CNTR value */
				wCNTR = _GetCNTR();

				/*Store endpoints registers status */
				for (i = 0; i < 8; i++) {
					EP[i] = _GetENDPOINT(i);
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
				for (i = 0; i < 8; i++) {
					_SetENDPOINT(i, EP[i]);
				}
				esof_counter = 0;
			}
		} else { /* if ((_GetFNR() & FNR_RXDP)!=0) */
			esof_counter = 0;
		}
		/* resume handling timing is made with ESOFs */
		Resume(RESUME_ESOF); /* request without change of the machine state */
	}
}

void USBWakeUp_IRQHandler(void)
{
	d_print("%s()\r\n",  __func__);
//	EXTI_ClearITPendingBit(EXTI_Line18);
	/* TODO: USB implement pending flag clearing */
}
