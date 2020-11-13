#ifndef __USB_MEM_H
#define __USB_MEM_H

#include <stdint.h>

void copy_to_usb(uint8_t *src_buffer, uint16_t usb_buffer_addr, uint16_t data_size);
void copy_from_usb(uint8_t *dst_buffer, uint16_t usb_buffer_addr, uint16_t data_size);

#endif  /*__USB_MEM_H*/
