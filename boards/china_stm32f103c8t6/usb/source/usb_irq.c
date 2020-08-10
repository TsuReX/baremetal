#include <usb_endp.h>
#include <usb_lib.h>
#include <usb_prop.h>
#include <usb_pwr.h>

__IO uint16_t usb_irq_flags;  /* ISTR register last read value */
__IO uint8_t bIntPackSOF = 0;  /* SOFs received between 2 consecutive packets */
__IO uint32_t esof_counter = 0; /* expected SOF counter */
__IO uint32_t wCNTR	= 0;

void USB_LP_CAN1_RX0_IRQHandler(void)
{
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
//	EXTI_ClearITPendingBit(EXTI_Line18);
	/* TODO: USB implement pending flag clearing */
}
