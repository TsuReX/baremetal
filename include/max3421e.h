/*
 * max3421e.h
 *
 *  Created on: Oct 2, 2020
 *      Author: user
 */

#ifndef MAX3421E_H_
#define MAX3421E_H_


uint8_t usbirq_read(void);

void usbirq_write(uint8_t usbirq);

uint8_t usbctl_read(void);

void usbctl_write(uint8_t usbctl);

uint8_t pinctl_read(void);

void pinctl_write(uint8_t pinctl);

uint8_t iopins1_read(void);

void iopins1_write(uint8_t iopins1);

uint8_t iopins2_read(void);

void iopins2_write(uint8_t iopins1);

uint8_t revision_read(void);

uint8_t hirq_read(void);

void hirq_write(uint8_t hirq);

uint8_t mode_read(void);

void mode_write(uint8_t mode);

uint8_t peraddr_read(void);

void peraddr_write(uint8_t peraddr);

uint8_t hctl_read(void);

void hctl_write(uint8_t hctl);

uint8_t hxfr_read(void);

void hxfr_write(uint8_t hxfr);

uint8_t hrsl_read(void);

uint8_t rcvfifo_write(uint8_t *src_buf, size_t data_size);

uint8_t rcvfifo_read(uint8_t *dst_buf, size_t data_size);

uint8_t sndfifo_write(uint8_t *src_buf, size_t data_size);

uint8_t sndfifo_read(uint8_t *dst_buf, size_t data_size);

uint8_t sudfifo_write(uint8_t *src_buf, size_t data_size);

uint8_t sudfifo_read(uint8_t *dst_buf, size_t data_size);

void rcvbc_write(uint8_t size);

uint8_t rcvbc_read(void);

void sndbc_write(uint8_t size);

uint8_t sndbc_read(void);

void kb_usb_fullduplex_spi_set(void);

void kb_usb_revision_read(void);

void kb_usb_chip_reset(void);

void kb_usb_power_enable(void);

void kb_usb_mode_set(void);

void kb_usb_bus_reset(void);

void kb_usb_device_detection_cycle(void);

void kb_usb_device_detect(void);

void kb_usb_setup_set_address(void);

void kb_usb_setup_get_dev_descr(void);


#endif /* MAX3421E_H_ */