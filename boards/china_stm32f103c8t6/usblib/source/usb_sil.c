#include "usb_lib.h"

extern uint16_t	wInterrupt_Mask;

/*******************************************************************************
* Function Name  : USB_SIL_Init
* Description    : Initialize the USB Device IP and the Endpoint 0.
* Input          : None.
* Output         : None.
* Return         : Status.
*******************************************************************************/
uint32_t USB_SIL_Init(void)
{
	_SetISTR(0);
	_SetCNTR(CNTR_CTRM | CNTR_WKUPM | CNTR_SUSPM | CNTR_ERRM | CNTR_SOFM | CNTR_ESOFM | CNTR_RESETM);
	return 0;
}

/*******************************************************************************
* Function Name  : USB_SIL_Write
* Description    : Write a buffer of data to a selected endpoint.
* Input          : - bEpAddr: The address of the non control endpoint.
*                  - pBufferPointer: The pointer to the buffer of data to be written
*                    to the endpoint.
*                  - wBufferSize: Number of data to be written (in bytes).
* Output         : None.
* Return         : Status.
*******************************************************************************/
uint32_t USB_SIL_Write(uint8_t bEpAddr, uint8_t* pBufferPointer, uint32_t wBufferSize)
{
  /* Use the memory interface function to write to the selected endpoint */
  copy_to_usb(pBufferPointer, _GetEPTxAddr(bEpAddr & 0x7F), wBufferSize);

  /* Update the data length in the control register */
  _SetEPTxCount((bEpAddr & 0x7F), wBufferSize);
  
  return 0;
}

/*******************************************************************************
* Function Name  : USB_SIL_Read
* Description    : Write a buffer of data to a selected endpoint.
* Input          : - bEpAddr: The address of the non control endpoint.
*                  - pBufferPointer: The pointer to which will be saved the 
*                     received data buffer.
* Output         : None.
* Return         : Number of received data (in Bytes).
*******************************************************************************/
uint32_t USB_SIL_Read(uint8_t bEpAddr, uint8_t* pBufferPointer)
{
  uint32_t DataLength = 0;

  /* Get the number of received data on the selected Endpoint */
  DataLength = _GetEPRxCount(bEpAddr & 0x7F);
  
  /* Use the memory interface function to write to the selected endpoint */
  copy_from_usb(pBufferPointer, _GetEPRxAddr(bEpAddr & 0x7F), DataLength);

  /* Return the number of received data */
  return DataLength;
}
