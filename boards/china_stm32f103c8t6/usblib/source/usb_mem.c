#include "usb_mem.h"
#include "usb_regs.h"

/*******************************************************************************
* Function Name  : UserToPMABufferCopy
* Description    : Copy a buffer from user memory area to packet memory area (PMA)
* Input          : - pbUsrBuf: pointer to user memory area.
*                  - wPMABufAddr: address into PMA.
*                  - wNBytes: no. of bytes to be copied.
* Output         : None.
* Return         : None	.
*******************************************************************************/
void copy_to_usb(uint8_t *src_buffer, uint16_t usb_buffer_addr, uint16_t data_size)
{
	uint16_t *src_word_buffer = (uint16_t *)src_buffer;
	uint32_t byte_count = data_size & 0x1;
	uint32_t word_count = data_size >> 1;
	uint32_t word_num;

	uint16_t *usb_word_buffer = (uint16_t *)(usb_buffer_addr * 2 + PMAAddr);

	for (word_num = 0; word_num < word_count; ++word_num) {

		usb_word_buffer[word_num] = src_word_buffer[word_num];
	}

	if (byte_count != 0) {
		uint16_t byte = *((uint8_t*)src_word_buffer + word_num * 2 + 1);
		usb_word_buffer[word_num + 1] = byte;
	}
}

/*******************************************************************************
* Function Name  : PMAToUserBufferCopy
* Description    : Copy a buffer from user memory area to packet memory area (PMA)
* Input          : - pbUsrBuf    = pointer to user memory area.
*                  - wPMABufAddr = address into PMA.
*                  - wNBytes     = no. of bytes to be copied.
* Output         : None.
* Return         : None.
*******************************************************************************/
void copy_from_usb(uint8_t *dst_buffer, uint16_t usb_buffer_addr, uint16_t data_size)
{
	uint16_t *dst_word_buffer = (uint16_t *)dst_buffer;
	uint32_t byte_count = data_size & 0x1;
	uint32_t word_count = data_size >> 1;   /* n = (wNBytes + 1) / 2 */
	uint32_t word_num;

	uint16_t *usb_word_buffer = (uint16_t *)(usb_buffer_addr * 2 + PMAAddr);

	for (word_num = 0; word_num < word_count; ++word_num) {

		dst_word_buffer[word_num] = usb_word_buffer[word_num];
	}

	if (byte_count != 0) {
		uint16_t word = usb_word_buffer[word_num + 1];
		*((uint8_t*)dst_word_buffer + word_num * 2 + 1) = (uint8_t)word;
	}
}

