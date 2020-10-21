/*
 * max3421e.h
 *
 *  Created on: Oct 2, 2020
 *      Author: user
 */

#ifndef MAX3421E_H_
#define MAX3421E_H_

__PACKED_STRUCT std_request {
		uint8_t		bm_request_type;
		uint8_t		b_request;
		uint16_t	w_value;
		uint16_t	w_index;
		uint16_t	w_length;
	};

__PACKED_STRUCT device_descriptor {
	uint8_t		b_length;
	uint8_t		b_descriptor_type;
	uint16_t	bcd_usb;
	uint8_t		b_device_class;
	uint8_t		b_device_sub_class;
	uint8_t		b_device_protocol;
	uint8_t		b_max_packet_size;
	uint16_t	id_vendor;
	uint16_t	id_product;
	uint16_t	bcd_device;
	uint8_t		i_manufacturer;
	uint8_t		i_product;
	uint8_t		i_serial_number;
	uint8_t		b_num_configurations;
};

__PACKED_STRUCT configuration_descriptor {
	uint8_t		b_length;
	uint8_t		b_descriptor_type;
	uint16_t	w_total_length;
	uint8_t		b_num_interfaces;
	uint8_t		b_configuration_value;
	uint8_t		i_configuration;
	uint8_t		bm_attributes;
	uint8_t		b_max_power;
};

__PACKED_STRUCT interface_descriptor {
	uint8_t		b_length;
	uint8_t		b_descriptor_type;
	uint8_t		b_interface_number;
	uint8_t		b_alternate_setting;
	uint8_t		b_num_endpoints;
	uint8_t		b_interface_class;
	uint8_t		b_interface_subclass;
	uint8_t		b_interface_protocol;
	uint8_t		i_interface;
};

__PACKED_STRUCT endpoint_descriptor {
	uint8_t		b_length;
	uint8_t		b_descriptor_type;
	uint8_t		b_endpoint_address;
	uint8_t		bm_attributes;
	uint16_t	w_max_packet_size;
	uint8_t		b_interval;
};

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

void kb_usb_sof_start(void);

void kb_usb_sof_stop(void);

void kb_usb_bus_reset(void);

void kb_usb_device_poll_detection_cycle(void);

void kb_usb_device_irq_detection_cycle(void);

void kb_usb_device_detect(void);

void kb_usb_setup_set_address(uint8_t dev_addr);

void kb_usb_setup_get_dev_descr(uint8_t dev_addr);

void kb_usb_setup_get_conf_descr(uint8_t dev_addr);

void kb_usb_setup_get_ep_status(uint8_t dev_addr, uint8_t ep_addr);

void kb_usb_setup_get_full_conf(uint8_t dev_addr);

void kb_usb_data_read(uint8_t dev_addr, uint8_t ep_addr);

int16_t kb_usb_hs_out_send(uint8_t dev_addr);

int16_t kb_usb_setup_send(uint8_t dev_addr, struct std_request* request);

int16_t kb_usb_hs_in_send(uint8_t dev_addr);

#endif /* MAX3421E_H_ */
