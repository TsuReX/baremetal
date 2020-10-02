/*
 * max3421e.c
 *
 *  Created on: Oct 2, 2020
 *      Author: user
 */

#include <stddef.h>

#include "drivers.h"
#include "max3421e.h"
#include "spi.h"
#include "utils.h"
#include "console.h"

#define SPI_USB_REGNUMOFF		0x03
#define SPI_USB_WROP			0x02
#define SPI_USB_RDOP			0x00

#define SPI_USB_RCVFIFOREG		0x01
#define SPI_USB_SNDFIFOREG		0x02
#define SPI_USB_SUDFIFOREG		0x04
#define SPI_USB_RCVBCREG		0x06
#define SPI_USB_SNDBCREG		0x07

#define SPI_USB_USBIRQREG		0x0D
#define SPI_USB_USBCTLREG		0x0F
#define SPI_USB_PINCTLREG		0x11
#define SPI_USB_REVREG			0x12
#define SPI_USB_IOPINS1REG		0x14
#define SPI_USB_IOPINS2REG		0x15
#define SPI_USB_HIRQREG			0x19
#define SPI_USB_MODEREG			0x1B
#define SPI_USB_PERADDRREG		0x1C
#define SPI_USB_HCTLREG			0x1D
#define SPI_USB_HXFRREG			0x1E
#define SPI_USB_HRSLREG			0x1F

//#define SPI_USB_HIENREG			0x1A

#define SPI_CMD_RDRCVFIFO	((SPI_USB_RCVFIFOREG << SPI_USB_REGNUMOFF) | SPI_USB_RDOP)
#define SPI_CMD_WRRCVFIFO	((SPI_USB_RCVFIFOREG << SPI_USB_REGNUMOFF) | SPI_USB_WROP)

#define SPI_CMD_RDSNDFIFO	((SPI_USB_SNDFIFOREG << SPI_USB_REGNUMOFF) | SPI_USB_RDOP)
#define SPI_CMD_WRSNDFIFO	((SPI_USB_SNDFIFOREG << SPI_USB_REGNUMOFF) | SPI_USB_WROP)

#define SPI_CMD_RDSUDFIFO	((SPI_USB_SUDFIFOREG << SPI_USB_REGNUMOFF) | SPI_USB_RDOP)
#define SPI_CMD_WRSUDFIFO	((SPI_USB_SUDFIFOREG << SPI_USB_REGNUMOFF) | SPI_USB_WROP)

#define SPI_CMD_RDRCVBC		((SPI_USB_RCVBCREG << SPI_USB_REGNUMOFF) | SPI_USB_RDOP)
#define SPI_CMD_WRRCVBC		((SPI_USB_RCVBCREG << SPI_USB_REGNUMOFF) | SPI_USB_WROP)

#define SPI_CMD_RDSNDBC		((SPI_USB_SNDBCREG << SPI_USB_REGNUMOFF) | SPI_USB_RDOP)
#define SPI_CMD_WRSNDBC		((SPI_USB_SNDBCREG << SPI_USB_REGNUMOFF) | SPI_USB_WROP)

#define SPI_CMD_RDREV		((SPI_USB_REVREG << SPI_USB_REGNUMOFF) | SPI_USB_RDOP)

#define SPI_CMD_WRPINCTL	((SPI_USB_PINCTLREG << SPI_USB_REGNUMOFF) | SPI_USB_WROP)
#define SPI_CMD_RDPINCTL	((SPI_USB_PINCTLREG << SPI_USB_REGNUMOFF) | SPI_USB_RDOP)

#define SPI_CMD_WRUSBIRQ	((SPI_USB_USBIRQREG << SPI_USB_REGNUMOFF) | SPI_USB_WROP)
#define SPI_CMD_RDUSBIRQ	((SPI_USB_USBIRQREG << SPI_USB_REGNUMOFF) | SPI_USB_RDOP)

#define SPI_CMD_WRUSBCTL	((SPI_USB_USBCTLREG << SPI_USB_REGNUMOFF) | SPI_USB_WROP)
#define SPI_CMD_RDUSBCTL	((SPI_USB_USBCTLREG << SPI_USB_REGNUMOFF) | SPI_USB_RDOP)

#define SPI_CMD_WRIOPINS1	((SPI_USB_IOPINS1REG << SPI_USB_REGNUMOFF) | SPI_USB_WROP)
#define SPI_CMD_RDIOPINS1	((SPI_USB_IOPINS1REG << SPI_USB_REGNUMOFF) | SPI_USB_RDOP)

#define SPI_CMD_WRIOPINS2	((SPI_USB_IOPINS2REG << SPI_USB_REGNUMOFF) | SPI_USB_WROP)
#define SPI_CMD_RDIOPINS2	((SPI_USB_IOPINS2REG << SPI_USB_REGNUMOFF) | SPI_USB_RDOP)

#define SPI_CMD_WRHIRQ	((SPI_USB_HIRQREG << SPI_USB_REGNUMOFF) | SPI_USB_WROP)
#define SPI_CMD_RDHIRQ	((SPI_USB_HIRQREG << SPI_USB_REGNUMOFF) | SPI_USB_RDOP)

#define SPI_CMD_WRMODE	((SPI_USB_MODEREG << SPI_USB_REGNUMOFF) | SPI_USB_WROP)
#define SPI_CMD_RDMODE	((SPI_USB_MODEREG << SPI_USB_REGNUMOFF) | SPI_USB_RDOP)

#define SPI_CMD_WRPERADDR	((SPI_USB_PERADDRREG << SPI_USB_REGNUMOFF) | SPI_USB_WROP)
#define SPI_CMD_RDPERADDR	((SPI_USB_PERADDRREG << SPI_USB_REGNUMOFF) | SPI_USB_RDOP)

#define SPI_CMD_WRHCTL	((SPI_USB_HCTLREG << SPI_USB_REGNUMOFF) | SPI_USB_WROP)
#define SPI_CMD_RDHCTL	((SPI_USB_HCTLREG << SPI_USB_REGNUMOFF) | SPI_USB_RDOP)

#define SPI_CMD_WRHXFR	((SPI_USB_HXFRREG << SPI_USB_REGNUMOFF) | SPI_USB_WROP)
#define SPI_CMD_RDHXFR	((SPI_USB_HXFRREG << SPI_USB_REGNUMOFF) | SPI_USB_RDOP)

#define SPI_CMD_RDHRSL	((SPI_USB_HRSLREG << SPI_USB_REGNUMOFF) | SPI_USB_RDOP)


uint8_t usbirq_read(void)
{
	uint8_t	buffer[2] = {SPI_CMD_RDUSBIRQ, 0x00};
	spi_data_xfer(buffer, buffer, sizeof(buffer) / sizeof(buffer[0]));
	return buffer[1];
}

void usbirq_write(uint8_t usbirq)
{
	uint8_t	buffer[2] = {SPI_CMD_WRUSBIRQ, usbirq};
	spi_data_xfer(buffer, buffer, sizeof(buffer) / sizeof(buffer[0]));
}

uint8_t usbctl_read(void)
{
	uint8_t	buffer[2] = {SPI_CMD_RDUSBCTL, 0x00};
	spi_data_xfer(buffer, buffer, sizeof(buffer) / sizeof(buffer[0]));
	return buffer[1];
}

void usbctl_write(uint8_t usbctl)
{
	uint8_t	buffer[2] = {SPI_CMD_WRUSBCTL, usbctl};
	spi_data_xfer(buffer, buffer, sizeof(buffer) / sizeof(buffer[0]));
}

uint8_t pinctl_read(void)
{
	uint8_t	buffer[2] = {SPI_CMD_RDPINCTL, 0x00};
	spi_data_xfer(buffer, buffer, sizeof(buffer) / sizeof(buffer[0]));
	return buffer[1];
}

void pinctl_write(uint8_t pinctl)
{
	uint8_t	buffer[2] = {SPI_CMD_WRPINCTL, pinctl};
	spi_data_xfer(buffer, buffer, sizeof(buffer) / sizeof(buffer[0]));
}

uint8_t iopins1_read(void)
{
	uint8_t	buffer[2] = {SPI_CMD_RDIOPINS1, 0x00};
	spi_data_xfer(buffer, buffer, sizeof(buffer) / sizeof(buffer[0]));
	return buffer[1];
}

void iopins1_write(uint8_t iopins1)
{
	uint8_t	buffer[2] = {SPI_CMD_WRIOPINS1, iopins1};
	spi_data_xfer(buffer, buffer, sizeof(buffer) / sizeof(buffer[0]));
}

uint8_t iopins2_read(void)
{
	uint8_t	buffer[2] = {SPI_CMD_RDIOPINS2, 0x00};
	spi_data_xfer(buffer, buffer, sizeof(buffer) / sizeof(buffer[0]));
	return buffer[1];
}

void iopins2_write(uint8_t iopins1)
{
	uint8_t	buffer[2] = {SPI_CMD_WRIOPINS2, iopins1};
	spi_data_xfer(buffer, buffer, sizeof(buffer) / sizeof(buffer[0]));
}

uint8_t revision_read(void)
{
	uint8_t	buffer[2] = {SPI_CMD_RDREV, 0x00};
	spi_data_xfer(buffer, buffer, sizeof(buffer) / sizeof(buffer[0]));
	return buffer[1];
}

uint8_t hirq_read(void)
{
	uint8_t	buffer[2] = {SPI_CMD_RDHIRQ, 0x00};
	spi_data_xfer(buffer, buffer, sizeof(buffer) / sizeof(buffer[0]));
	return buffer[1];
}

void hirq_write(uint8_t hirq)
{
	uint8_t	buffer[2] = {SPI_CMD_WRHIRQ, hirq};
	spi_data_xfer(buffer, buffer, sizeof(buffer) / sizeof(buffer[0]));
}

uint8_t mode_read(void)
{
	uint8_t	buffer[2] = {SPI_CMD_RDMODE, 0x00};
	spi_data_xfer(buffer, buffer, sizeof(buffer) / sizeof(buffer[0]));
	return buffer[1];
}

void mode_write(uint8_t mode)
{
	uint8_t	buffer[2] = {SPI_CMD_WRMODE, mode};
	spi_data_xfer(buffer, buffer, sizeof(buffer) / sizeof(buffer[0]));
}

uint8_t peraddr_read(void)
{
	uint8_t	buffer[2] = {SPI_CMD_RDPERADDR, 0x00};
	spi_data_xfer(buffer, buffer, sizeof(buffer) / sizeof(buffer[0]));
	return buffer[1];
}

void peraddr_write(uint8_t peraddr)
{
	uint8_t	buffer[2] = {SPI_CMD_WRPERADDR, peraddr};
	spi_data_xfer(buffer, buffer, sizeof(buffer) / sizeof(buffer[0]));
}

uint8_t hctl_read(void)
{
	uint8_t	buffer[2] = {SPI_CMD_RDHCTL, 0x00};
	spi_data_xfer(buffer, buffer, sizeof(buffer) / sizeof(buffer[0]));
	return buffer[1];
}

void hctl_write(uint8_t hctl)
{
	uint8_t	buffer[2] = {SPI_CMD_WRHCTL, hctl};
	spi_data_xfer(buffer, buffer, sizeof(buffer) / sizeof(buffer[0]));
}

uint8_t hxfr_read(void)
{
	uint8_t	buffer[2] = {SPI_CMD_RDHXFR, 0x00};
	spi_data_xfer(buffer, buffer, sizeof(buffer) / sizeof(buffer[0]));
	return buffer[1];
}

void hxfr_write(uint8_t hxfr)
{
	uint8_t	buffer[2] = {SPI_CMD_WRHXFR, hxfr};
	spi_data_xfer(buffer, buffer, sizeof(buffer) / sizeof(buffer[0]));
}

uint8_t hrsl_read(void)
{
	uint8_t	buffer[2] = {SPI_CMD_RDHRSL, 0x00};
	spi_data_xfer(buffer, buffer, sizeof(buffer) / sizeof(buffer[0]));
	return buffer[1];
}

uint8_t rcvfifo_write(uint8_t *src_buf, size_t data_size)
{
	uint8_t	cmd = SPI_CMD_WRRCVFIFO;
	spi_data_xfer(&cmd, NULL, 1);
	size_t size = spi_data_xfer(src_buf, NULL, data_size);
	return size;
}

uint8_t rcvfifo_read(uint8_t *dst_buf, size_t data_size)
{
	uint8_t	cmd = SPI_CMD_RDRCVFIFO;
	spi_data_xfer(&cmd, NULL, 1);
	size_t size = spi_data_xfer(NULL, dst_buf, data_size);
	return size;
}

uint8_t sndfifo_write(uint8_t *src_buf, size_t data_size)
{
	uint8_t	cmd = SPI_CMD_WRSNDFIFO;
	spi_data_xfer(&cmd, NULL, 1);
	size_t size = spi_data_xfer(src_buf, NULL, data_size);
	return size;
}

uint8_t sndfifo_read(uint8_t *dst_buf, size_t data_size)
{
	uint8_t	cmd = SPI_CMD_RDSNDFIFO;
	spi_data_xfer(&cmd, NULL, 1);
	size_t size = spi_data_xfer(NULL, dst_buf, data_size);
	return size;
}

uint8_t sudfifo_write(uint8_t *src_buf, size_t data_size)
{
	uint8_t	cmd = SPI_CMD_WRSUDFIFO;
	spi_data_xfer(&cmd, NULL, 1);
	size_t size = spi_data_xfer(src_buf, NULL, data_size);
	return size;
}

uint8_t sudfifo_read(uint8_t *dst_buf, size_t data_size)
{
	uint8_t	cmd = SPI_CMD_RDSUDFIFO;
	spi_data_xfer(&cmd, NULL, 1);
	size_t size = spi_data_xfer(NULL, dst_buf, data_size);
	return size;
}

void rcvbc_write(uint8_t size)
{
	uint8_t	buffer[2] = {SPI_CMD_WRRCVBC, size};
	spi_data_xfer(buffer, buffer, sizeof(buffer) / sizeof(buffer[0]));
}

uint8_t rcvbc_read(void)
{
	uint8_t	buffer[2] = {SPI_CMD_RDRCVBC, 0x00};
	spi_data_xfer(buffer, buffer, sizeof(buffer) / sizeof(buffer[0]));
	return buffer[1];
}

void sndbc_write(uint8_t size)
{
	uint8_t	buffer[2] = {SPI_CMD_WRSNDBC, size};
	spi_data_xfer(buffer, buffer, sizeof(buffer) / sizeof(buffer[0]));
}

uint8_t sndbc_read(void)
{
	uint8_t	buffer[2] = {SPI_CMD_RDSNDBC, 0x00};
	spi_data_xfer(buffer, buffer, sizeof(buffer) / sizeof(buffer[0]));
	return buffer[1];
}

void kb_usb_fullduplex_spi_set(void)
{
	d_print("Set full duplex SPI transmission\r\n");
	spi_chip_activate();
	pinctl_write(0x1E);
	spi_chip_deactivate();
}

void kb_usb_revision_read(void)
{
	spi_chip_activate();
	d_print("REVISION 0x%02X\r\n", revision_read());
	spi_chip_deactivate();
}

void kb_usb_chip_reset(void)
{
	spi_chip_activate();
	d_print("USBIRQ: 0x%02X\r\n", usbirq_read());
	spi_chip_deactivate();

	spi_chip_activate();
	uint8_t val = usbctl_read();
	spi_chip_deactivate();
	d_print("USBCTL: 0x%02X\r\n", val);

	d_print("Reset chip\r\n");
	spi_chip_activate();
	usbctl_write(val | (0x1 << 5));
	spi_chip_deactivate();
	mdelay(1);

	spi_chip_activate();
	usbctl_write(val);
	spi_chip_deactivate();

	spi_chip_activate();
	d_print("USBCTL: 0x%02X\r\n", usbctl_read());
	spi_chip_deactivate();

	spi_chip_activate();
	d_print("USBIRQ: 0x%02X\r\n", usbirq_read());
	spi_chip_deactivate();

	d_print("Waiting the oscillator stabilization\r\n");

	spi_chip_activate();
	while((usbirq_read() & 0x1) != 0x1) {
		spi_chip_deactivate();
		mdelay(1);
		spi_chip_activate();
	}
	spi_chip_deactivate();

	spi_chip_activate();
	d_print("USBCTL: 0x%02X\r\n", usbctl_read());
	spi_chip_deactivate();

	spi_chip_activate();
	d_print("USBIRQ: 0x%02X\r\n", usbirq_read());
	spi_chip_deactivate();

	spi_chip_activate();
	usbirq_write(0x1);
	spi_chip_deactivate();

	spi_chip_activate();
	d_print("USBIRQ: 0x%02X\r\n", usbirq_read());
	spi_chip_deactivate();

}

void kb_usb_power_enable(void)
{
	spi_chip_activate();
	uint8_t iopins1 = iopins1_read();
	spi_chip_deactivate();

	d_print("IOPINS1: 0x%02X\r\n", iopins1);

	spi_chip_activate();
	iopins1_write(iopins1 | 0x1);
	spi_chip_deactivate();

	d_print("USB power is enabled\r\n");

	spi_chip_activate();
	iopins1 = iopins1_read();
	spi_chip_deactivate();
	d_print("IOPINS1: 0x%02X\r\n", iopins1);
}

void kb_usb_mode_set(void)
{
	spi_chip_activate();
	uint8_t mode = mode_read();
	spi_chip_deactivate();

	d_print("MODE: 0x%02X\r\n", mode);

	spi_chip_activate();
	mode_write(mode |0x80 | 0x40 |0x1);
	spi_chip_deactivate();

	d_print("HOST mode, DPPULLDN and DNPULLDN are set\r\n");

	spi_chip_activate();
	d_print("MODE: 0x%02X\r\n", mode_read());
	spi_chip_deactivate();
}

void kb_usb_bus_reset(void)
{
	spi_chip_activate();
	uint8_t hctl = hctl_read();
	spi_chip_deactivate();
	d_print("HCTL: 0x%02X\r\n", hctl);

	spi_chip_activate();
	mode_write(hctl |0x1);
	spi_chip_deactivate();

	spi_chip_activate();
	d_print("HCTL: 0x%02X\r\n", hctl_read());
	spi_chip_deactivate();

	spi_chip_activate();
	while((hctl_read() & 0x1) == 0x1) {
		spi_chip_deactivate();
		mdelay(1);
		spi_chip_activate();
	}
	spi_chip_deactivate();

	spi_chip_activate();
	uint8_t mode = mode_read();
	spi_chip_deactivate();
	d_print("MODE: 0x%02X\r\n", mode);

	spi_chip_activate();
	mode_write(mode |0x08);
	spi_chip_deactivate();

	spi_chip_activate();
	d_print("MODE: 0x%02X\r\n", mode_read());
	spi_chip_deactivate();

	spi_chip_activate();
	while((hirq_read() & 0x40) != 0x40) {
		spi_chip_deactivate();
		mdelay(1);
		spi_chip_activate();
	}
	spi_chip_deactivate();

//	d_print("USB bus was reset\r\n");
}

void kb_usb_device_detection_cycle(void)
{

	while(1) {
		d_print("----------------------\r\n");
		spi_chip_activate();
		while((hirq_read() & 0x20) == 0x0) {
			spi_chip_deactivate();
			mdelay(1000);
			spi_chip_activate();
		}
		spi_chip_deactivate();

		spi_chip_activate();
		d_print("HIRQ: 0x%02X\r\n", hirq_read());
		spi_chip_deactivate();

		spi_chip_activate();
		hirq_write(0x20);
		spi_chip_deactivate();

		spi_chip_activate();
		d_print("HIRQ: 0x%02X\r\n", hirq_read());
		spi_chip_deactivate();

		spi_chip_activate();
		d_print("HRSL: 0x%02X\r\n", hrsl_read());
		spi_chip_deactivate();

		spi_chip_activate();
		d_print("HIRQ: 0x%02X\r\n", hirq_read());
		spi_chip_deactivate();

		mdelay(1000);

		spi_chip_activate();
		d_print("HIRQ: 0x%02X\r\n", hirq_read());
		spi_chip_deactivate();
	}
}

void kb_usb_device_detect(void)
{
//	spi_chip_activate();
//	uint8_t hirq = hirq_read();
//	spi_chip_deactivate();
//	d_print("HIRQ: 0x%02X\r\n", hirq);

	spi_chip_activate();
	uint8_t hctl = hctl_read();
	spi_chip_deactivate();

	spi_chip_activate();
//	d_print("HRSL: 0x%02X\r\n", hrsl_read());
	spi_chip_deactivate();

	spi_chip_activate();
	hctl_write(hctl | 0x04);
	spi_chip_deactivate();

	spi_chip_activate();
	uint8_t hrsl = hrsl_read();
//	d_print("HRSL: 0x%02X\r\n", hrsl);
	spi_chip_deactivate();
	/*
		(J,K)
		(0,0) - Single Ended Zero
		(0,1) - Low Speed
		(1,0) - Full Speed
		(1,1) - Single Ended One (Illegal state)
	*/
	switch(hrsl & 0xD0){
	case 0x40:
			d_print("Low speed device connected\r\n");
			spi_chip_activate();
			uint8_t mode = mode_read();
			spi_chip_deactivate();

			spi_chip_activate();
			mode_write(mode | 0x02);
			spi_chip_deactivate();
			break;
	case 0x80:
			d_print("Full speed device connected\r\n");
			break;
	case 0xD0:
			d_print("Bus illegal state\r\n");
			break;
	case 0x00:
			d_print("Device not connected\r\n");
			break;
	}
}

void kb_usb_setup_set_address(void)
{
	d_print("Setting up device address\r\n");
	struct std_request {
		uint8_t		bm_request_type;
		uint8_t		b_request;
		uint16_t	w_value;
		uint16_t	w_index;
		uint16_t	w_length;
	};
	struct std_request set_addr = {0x0, 0x5, 0x34, 0x0, 0x0};

	d_print("bm_request_type: 0x%02X\r\n", set_addr.bm_request_type);
	d_print("b_request: 0x%02X\r\n", set_addr.b_request);
	d_print("w_value: 0x%04X\r\n", set_addr.w_value);
	d_print("w_index: 0x%04X\r\n", set_addr.w_index);
	d_print("w_length: 0x%04X\r\n", set_addr.w_length);

	spi_chip_activate();
	sudfifo_write((uint8_t*)&set_addr, sizeof(set_addr));
	spi_chip_deactivate();

	spi_chip_activate();
	peraddr_write(0x00);
	spi_chip_deactivate();

//	PORT_SetBits(MDR_PORTB, PORT_Pin_6);
	LL_GPIO_SetOutputPin(GPIOB, LL_GPIO_PIN_1);

	size_t counter = 1;
	for (; counter > 0; --counter) {

		spi_chip_activate();
		hxfr_write(0x10);
		spi_chip_deactivate();

		spi_chip_activate();
		while((hirq_read() & 0x80) != 0x80) {
			spi_chip_deactivate();
			u100delay(1);
			spi_chip_activate();
		}
		spi_chip_deactivate();

		spi_chip_activate();
		d_print("HRSLT: 0x%01X\r\n", hrsl_read() & 0x0F);
		spi_chip_deactivate();

//		mdelay(20);
	}
	LL_GPIO_ResetOutputPin(GPIOB, LL_GPIO_PIN_1);
//	PORT_ResetBits(MDR_PORTB, PORT_Pin_6);
}

void kb_usb_setup_get_dev_descr(void)
{
	d_print("Getting device descriptor\r\n");
	struct std_request {
		uint8_t		bm_request_type;
		uint8_t		b_request;
		uint16_t	w_value;
		uint16_t	w_index;
		uint16_t	w_length;
	};
	struct std_request set_addr = {0x80, 0x6, 0x01, 0x0, 0x0};

	d_print("bm_request_type: 0x%02X\r\n", set_addr.bm_request_type);
	d_print("b_request: 0x%02X\r\n", set_addr.b_request);
	d_print("w_value: 0x%04X\r\n", set_addr.w_value);
	d_print("w_index: 0x%04X\r\n", set_addr.w_index);
	d_print("w_length: 0x%04X\r\n", set_addr.w_length);

	spi_chip_activate();
	sudfifo_write((uint8_t*)&set_addr, sizeof(set_addr));
	spi_chip_deactivate();

	spi_chip_activate();
	peraddr_write(0x00);
	spi_chip_deactivate();

//	PORT_SetBits(MDR_PORTB, PORT_Pin_6);

	size_t counter = 3;
	for (; counter > 0; --counter) {

		spi_chip_activate();
		hxfr_write(0x10);
		spi_chip_deactivate();

		spi_chip_activate();
		while((hirq_read() & 0x80) != 0x80) {
			spi_chip_deactivate();
			u100delay(1);
			spi_chip_activate();
		}
		spi_chip_deactivate();

		spi_chip_activate();
		d_print("HRSLT: 0x%01X\r\n", hrsl_read() & 0x0F);
		spi_chip_deactivate();

		mdelay(20);
	}
}
