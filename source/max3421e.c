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

#define USB_EP0					0x00
#define USB_EP1					0x01

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

#define HIRQ_RCVDAVIRQ	0x04
#define HIRQ_CONDETIRQ	0x20
#define HIRQ_FRAMEIRQ	0x40
#define HIRQ_HXFRDNIRQ	0x80

#define HCTL_BUSRST		0x01
#define HCTL_SAMPLEBUS	0x04
#define HCTL_RCVTOG0	0x10
#define HCTL_RCVTOG1	0x20
#define HCTL_SNDTOG0	0x40
#define HCTL_SNDTOG1	0x80

#define MODE_HOST		0x01
#define MODE_LOWSPEED	0x02
#define MODE_SOFKAENAB	0x08
#define MODE_DMPULLDN	0x40
#define MODE_DPPULLDN	0x80

#define HRSL_KSTATUS	0x40
#define HRSL_JSTATUS	0x80

#define HXFR_BULKIN		0x00
#define HXFR_SETUP		0x10
#define HXFR_BULK_OUT	0x20
#define HXFR_HSIN		0x80
#define HXFR_HSOUT		0xA0

#define IOPINS1_GPOUT1 0x1

#define USBCTL_CHIPRES	0x10

#define USBIRQ_OSCOKIRQ	0x01

#define PINCTL_GPXA		0x01
#define PINCTL_GPXB		0x02
#define PINCTL_POSINT	0x04
#define PINCTL_INTLEVEL 0x08
#define PINCTL_FDUPSPI	0x10

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

	pinctl_write(PINCTL_FDUPSPI | PINCTL_INTLEVEL | PINCTL_POSINT | PINCTL_GPXB | PINCTL_GPXA);
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
	uint8_t usbctl = usbctl_read();
	spi_chip_deactivate();
	d_print("USBCTL: 0x%02X\r\n", usbctl);

	d_print("Reset chip\r\n");
	spi_chip_activate();

	usbctl_write(usbctl | USBCTL_CHIPRES);
	spi_chip_deactivate();
	mdelay(1);

	spi_chip_activate();
	usbctl_write(usbctl);
	spi_chip_deactivate();

	spi_chip_activate();
	d_print("USBCTL: 0x%02X\r\n", usbctl_read());
	spi_chip_deactivate();

	spi_chip_activate();
	d_print("USBIRQ: 0x%02X\r\n", usbirq_read());
	spi_chip_deactivate();

	d_print("Waiting the oscillator stabilization\r\n");

	spi_chip_activate();
	while((usbirq_read() & USBIRQ_OSCOKIRQ) != USBIRQ_OSCOKIRQ) {
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
	usbirq_write(USBIRQ_OSCOKIRQ);
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
	iopins1_write(iopins1 | IOPINS1_GPOUT1);
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

	mode_write(mode | MODE_DPPULLDN | MODE_DMPULLDN |MODE_HOST);
	spi_chip_deactivate();

	d_print("HOST mode, DPPULLDN and DNPULLDN are set\r\n");

	spi_chip_activate();
	d_print("MODE: 0x%02X\r\n", mode_read());
	spi_chip_deactivate();
}

void kb_usb_sof_stop(void)
{
	spi_chip_activate();
	uint8_t mode = mode_read();
	spi_chip_deactivate();

	/* Start SOF */
	spi_chip_activate();
	mode_write(mode & ~MODE_SOFKAENAB);
	spi_chip_deactivate();

	d_print("SOF stopped\r\n");
}

void kb_usb_sof_start(void)
{
	spi_chip_activate();
	uint8_t mode = mode_read();
	spi_chip_deactivate();

	/* Start SOF */
	spi_chip_activate();
	mode_write(mode | MODE_SOFKAENAB);
	spi_chip_deactivate();

	/* Wait first SOF */
	spi_chip_activate();
	while((hirq_read() & HIRQ_FRAMEIRQ) != HIRQ_FRAMEIRQ) {
		spi_chip_deactivate();
		mdelay(1);
		spi_chip_activate();
	}
	spi_chip_deactivate();

	d_print("SOF started\r\n");
}

void kb_usb_bus_reset(void)
{
	spi_chip_activate();
	uint8_t hctl = hctl_read();
	spi_chip_deactivate();

	/* Start bus reset */
	spi_chip_activate();
	hctl_write(hctl | HCTL_BUSRST);
	spi_chip_deactivate();

	/* Wait end of bus reset */
	spi_chip_activate();
	while((hctl_read() & HCTL_BUSRST) == HCTL_BUSRST) {
		spi_chip_deactivate();
		mdelay(1);
		spi_chip_activate();
	}
	spi_chip_deactivate();

	mdelay(200);

	d_print("USB bus was reset\r\n");
}

void kb_usb_device_poll_detection_cycle(void)
{
	while(1) {
		spi_chip_activate();
		uint8_t hctl = hctl_read();
		spi_chip_deactivate();

		/* Update bus state */
		spi_chip_activate();
		hctl_write(hctl | HCTL_SAMPLEBUS);
		spi_chip_deactivate();

		spi_chip_activate();
		uint16_t hrsl = hrsl_read();
		spi_chip_deactivate();

		switch(hrsl & (HRSL_JSTATUS | HRSL_KSTATUS)){
		case HRSL_KSTATUS:
				d_print("Low speed device connected\r\n");
				spi_chip_activate();
				uint8_t mode = mode_read();
				spi_chip_deactivate();

				spi_chip_activate();
				mode_write(mode | MODE_LOWSPEED);
				spi_chip_deactivate();
				break;
		case HRSL_JSTATUS:
				d_print("Full speed device connected\r\n");
				break;
		case (HRSL_JSTATUS | HRSL_KSTATUS):
				d_print("Bus illegal state\r\n");
				break;
		case 0x00:
				d_print("Device not connected\r\n");
				break;
		}

		mdelay(1000);
	}
}

void kb_usb_device_irq_detection_cycle(void)
{
	while(1) {
		spi_chip_activate();
		uint16_t hirq = hirq_read();
		while((hirq & HIRQ_CONDETIRQ) == 0x0) {
			spi_chip_deactivate();

			mdelay(1000);

			spi_chip_activate();
			hirq = hirq_read();
		}
		spi_chip_deactivate();

		/* Deassert CONDETIRQ */
		spi_chip_activate();
		hirq_write(HIRQ_CONDETIRQ);
		spi_chip_deactivate();

		spi_chip_activate();
		uint16_t hrsl = hrsl_read();
		spi_chip_deactivate();

		switch(hrsl & (HRSL_JSTATUS | HRSL_KSTATUS)){
		case HRSL_KSTATUS:
				d_print("Low speed device connected\r\n");
				spi_chip_activate();
				uint8_t mode = mode_read();
				spi_chip_deactivate();

				spi_chip_activate();
				mode_write(mode | MODE_LOWSPEED);
				spi_chip_deactivate();
				break;
		case HRSL_JSTATUS:
				d_print("Full speed device connected\r\n");
				break;
		case (HRSL_JSTATUS | HRSL_KSTATUS):
				d_print("Bus illegal state\r\n");
				break;
		case 0x00:
				d_print("Device not connected\r\n");
				break;
		}

		mdelay(1000);
	}
}

void kb_usb_device_detect(void)
{
	spi_chip_activate();
	uint8_t hctl = hctl_read();
	spi_chip_deactivate();

	spi_chip_activate();
	hctl_write(hctl | HCTL_SAMPLEBUS);
	spi_chip_deactivate();

	spi_chip_activate();
	uint8_t hrsl = hrsl_read();
	spi_chip_deactivate();
	/*
		(J,K)
		(0,0) - Single Ended Zero
		(0,1) - Low Speed
		(1,0) - Full Speed
		(1,1) - Single Ended One (Illegal state)
	*/

	switch(hrsl & (HRSL_JSTATUS | HRSL_KSTATUS)){
	case HRSL_KSTATUS:
			d_print("Low speed device connected\r\n");
			spi_chip_activate();
			uint8_t mode = mode_read();
			spi_chip_deactivate();

			spi_chip_activate();
			mode_write(mode | MODE_LOWSPEED);
			spi_chip_deactivate();
			break;
	case HRSL_JSTATUS:
			d_print("Full speed device connected\r\n");
			break;
	case (HRSL_JSTATUS | HRSL_KSTATUS):
			d_print("Bus illegal state\r\n");
			break;
	case 0x00:
			d_print("Device not connected\r\n");
			break;
	}
}

void kb_usb_setup_set_address(uint8_t dev_addr)
{
	d_print("Setting up device address\r\n");
	struct std_request {
		uint8_t		bm_request_type;
		uint8_t		b_request;
		uint16_t	w_value;
		uint16_t	w_index;
		uint16_t	w_length;
	};
	struct std_request set_addr = {0x0, 0x5, dev_addr, 0x0, 0x0};

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
//	LL_GPIO_SetOutputPin(GPIOB, LL_GPIO_PIN_1);

	spi_chip_activate();
	hxfr_write(HXFR_SETUP);
	spi_chip_deactivate();

	spi_chip_activate();
	while((hirq_read() & HIRQ_HXFRDNIRQ) != HIRQ_HXFRDNIRQ) {
		spi_chip_deactivate();
		u100delay(1);
		spi_chip_activate();
	}
	spi_chip_deactivate();

	spi_chip_activate();
	hirq_write(HIRQ_HXFRDNIRQ);
	spi_chip_deactivate();

	spi_chip_activate();
	d_print("HRSLT: 0x%01X\r\n", hrsl_read() & 0x0F);
	spi_chip_deactivate();

//	LL_GPIO_ResetOutputPin(GPIOB, LL_GPIO_PIN_1);
//	PORT_ResetBits(MDR_PORTB, PORT_Pin_6);
}

int16_t kb_usb_hs_in_send(uint8_t dev_addr)
{
	spi_chip_activate();
	peraddr_write(dev_addr);
	spi_chip_deactivate();

	spi_chip_activate();
	hxfr_write(HXFR_HSIN);
	spi_chip_deactivate();

	spi_chip_activate();
	while((hirq_read() & HIRQ_HXFRDNIRQ) != HIRQ_HXFRDNIRQ) {
		spi_chip_deactivate();
		u100delay(1);
		spi_chip_activate();
	}
	spi_chip_deactivate();

	spi_chip_activate();
	hirq_write(HIRQ_HXFRDNIRQ);
	spi_chip_deactivate();

	spi_chip_activate();
	uint8_t hrslt = hrsl_read() & 0x0F;
	spi_chip_deactivate();

	return -1 * hrslt;

}

int16_t kb_usb_hs_out_send(uint8_t dev_addr)
{
	spi_chip_activate();
	peraddr_write(dev_addr);
	spi_chip_deactivate();

	spi_chip_activate();
	hxfr_write(HXFR_HSOUT);
	spi_chip_deactivate();

	spi_chip_activate();
	while((hirq_read() & HIRQ_HXFRDNIRQ) != HIRQ_HXFRDNIRQ) {
		spi_chip_deactivate();
		u100delay(1);
		spi_chip_activate();
	}
	spi_chip_deactivate();

	spi_chip_activate();
	hirq_write(HIRQ_HXFRDNIRQ);
	spi_chip_deactivate();

	spi_chip_activate();
	uint8_t hrslt = hrsl_read() & 0x0F;
	spi_chip_deactivate();

	return -1 * hrslt;
}

int16_t kb_usb_setup_send(uint8_t dev_addr, struct std_request* request)
{
	/* Write data into SETUP FIFO */
	spi_chip_activate();
	sudfifo_write((uint8_t*)request, 8);
	spi_chip_deactivate();

	/* Set device address */
	spi_chip_activate();
	peraddr_write(dev_addr);
	spi_chip_deactivate();

	/* Initiate SETUP package transmission */
	spi_chip_activate();
	hxfr_write(HXFR_SETUP);
	spi_chip_deactivate();

	/* Check ending of the package transmission */
	spi_chip_activate();
	while((hirq_read() & HIRQ_HXFRDNIRQ) != HIRQ_HXFRDNIRQ) {
		spi_chip_deactivate();
		u100delay(1);
		spi_chip_activate();
	}
	spi_chip_deactivate();

	/* Clear flag of the transmission ending */
	spi_chip_activate();
	hirq_write(HIRQ_HXFRDNIRQ);
	spi_chip_deactivate();

	uint8_t hrsl = 0;
	/* Check result of the SETUP transmission */
	spi_chip_activate();
	hrsl = hrsl_read() & 0x0F;
	spi_chip_deactivate();

	return -1 * (hrsl & 0x0F);
}

void kb_usb_recv_tog_set(uint32_t recv_tog)
{
	/* Read HCTL register */
	spi_chip_activate();
	uint8_t hctl = hctl_read();
	spi_chip_deactivate();

	spi_chip_activate();
	if (recv_tog == 0){
		/* Notice master to wait DATA0 */
		hctl_write(hctl | HCTL_RCVTOG0);
	} else {
		/* Notice master to wait DATA1 */
		hctl_write(hctl | HCTL_RCVTOG1);
	}
	spi_chip_deactivate();
}

void kb_usb_send_tog_set(uint32_t send_tog)
{
	/* Read HCTL register */
	spi_chip_activate();
	uint8_t hctl = hctl_read();
	spi_chip_deactivate();

	spi_chip_activate();
	if (send_tog == 0){
		/* Notice master to send DATA0 */
		hctl_write(hctl | HCTL_SNDTOG0);
	} else {
		/* Notice master to send DATA1 */
		hctl_write(hctl | HCTL_SNDTOG1);
	}
	spi_chip_deactivate();
}

int16_t kb_usb_bulk_receive(uint8_t dev_addr, uint8_t ep_addr, void* dst_buf, size_t buf_size)
{
	/* Set device address */
	spi_chip_activate();
	peraddr_write(dev_addr);
	spi_chip_deactivate();

	size_t size_to_recv = buf_size;

	do {
//		d_print("Size to receive %d\r\n", size_to_recv);
		/* Initiate BULK-IN transaction */
		spi_chip_activate();
		hxfr_write(HXFR_BULKIN | ep_addr);
		spi_chip_deactivate();

		/* Check ending of the package transmission */
		spi_chip_activate();
		while((hirq_read() & HIRQ_HXFRDNIRQ) != HIRQ_HXFRDNIRQ) {
			spi_chip_deactivate();
			u100delay(1);
			spi_chip_activate();
		}
		spi_chip_deactivate();

		/* Clear flag of the transmission ending */
		spi_chip_activate();
		hirq_write(HIRQ_HXFRDNIRQ);
		spi_chip_deactivate();

		/* Check result of the BULK-IN transmission */
		spi_chip_activate();
		uint8_t hrslt = hrsl_read() & 0x0F;
		spi_chip_deactivate();

		if (hrslt != 0){
	//		d_print("BULK-IN transmission error. HRSLT: 0x%01X\r\n", hrslt);
			return -1 * hrslt;
		}

		/* Check flag of successful data reception */
		spi_chip_activate();
		while((hirq_read() & HIRQ_RCVDAVIRQ) != HIRQ_RCVDAVIRQ) {
			spi_chip_deactivate();
			u100delay(1);
			spi_chip_activate();
		}
		spi_chip_deactivate();

		uint8_t rcvbc = 0x00;

		/* Read data form reception byte counter */
		spi_chip_activate();
		rcvbc = rcvbc_read();
		spi_chip_deactivate();

	//	d_print("rcvbc: 0x%02X\r\n", rcvbc);

		/* Read data from reception FIFO */
		spi_chip_activate();
		if (size_to_recv >= rcvbc) {
			rcvfifo_read((uint8_t*)(dst_buf + buf_size - size_to_recv), rcvbc);
			size_to_recv -= rcvbc;
		} else {
			rcvfifo_read((uint8_t*)(dst_buf + buf_size - size_to_recv), size_to_recv);
			size_to_recv -= size_to_recv;
		}
		spi_chip_deactivate();


		/* Clear flag of successful reception */
		spi_chip_activate();
		hirq_write(HIRQ_RCVDAVIRQ);
		spi_chip_deactivate();

	} while(size_to_recv > 0);

	return buf_size;
}

void kb_usb_setup_get_dev_descr(uint8_t dev_addr)
{
	d_print("Getting device descriptor\r\n");

	struct std_request get_dev_descr = {0x80, 0x06, 0x0100, 0x0000, 0x0008};

//	d_print("bm_request_type: 0x%02X\r\n", get_dev_descr.bm_request_type);
//	d_print("b_request: 0x%02X\r\n", get_dev_descr.b_request);
//	d_print("w_value: 0x%04X\r\n", get_dev_descr.w_value);
//	d_print("w_index: 0x%04X\r\n", get_dev_descr.w_index);
//	d_print("w_length: 0x%04X\r\n", get_dev_descr.w_length);
//	mdelay(10);

	int16_t hrslt = kb_usb_setup_send(dev_addr, &get_dev_descr);

	if (hrslt < 0) {
		d_print("SETUP transmission error. HRSLT: 0x%01X\r\n", -1 * hrslt);
		return;
	}

	/* It needs time to process request */
	mdelay(20);

	d_print("The first reading of the descriptor\r\n");

	kb_usb_recv_tog_set(1);

	struct device_descriptor dev_descr = {0x1,0x2,0x3,0x4,0x5,0x6,0x7,0x8,0x9,0xA,0xB,0xC,0xD,0xE};
	hrslt = kb_usb_bulk_receive(dev_addr, 0, &dev_descr, 8);

	kb_usb_hs_out_send(0x34);

	if (hrslt < 0) {
		d_print("BULK-IN transmission error. HRSLT: 0x%01X\r\n",  -1 * hrslt);
		return;
	}

	d_print("b_length: 0x%02X\r\n", dev_descr.b_length);
	d_print("b_descriptor_type: 0x%02X\r\n", dev_descr.b_descriptor_type);
	d_print("bcd_usb: 0x%04X\r\n", dev_descr.bcd_usb);
	d_print("b_device_class: 0x%02X\r\n", dev_descr.b_device_class);
	d_print("b_device_sub_class: 0x%02X\r\n", dev_descr.b_device_sub_class);
	d_print("b_device_protocol: 0x%02X\r\n", dev_descr.b_device_protocol);
	d_print("b_max_packet_size: 0x%02X\r\n", dev_descr.b_max_packet_size);

	mdelay(1000);
	d_print("\n\n\n");
	d_print("The second reading of the descriptor\r\n");

	get_dev_descr.w_length = 0x12;
	hrslt = kb_usb_setup_send(dev_addr, &get_dev_descr);

	if (hrslt < 0) {
		d_print("SETUP transmission error. HRSLT: 0x%01X\r\n", -1 * hrslt);
		return;
	}

	/* It needs time to process request */
	mdelay(20);

	kb_usb_recv_tog_set(1);

	struct device_descriptor dev_descr1 = {0xF1,0xF2,0xF3,0xF4,0xF5,0xF6,0xF7,0xF8,0xF9,0xFA,0xFB,0xFC,0xFD,0xFE};
	hrslt = kb_usb_bulk_receive(dev_addr, 0, &dev_descr1, sizeof(dev_descr1));

	kb_usb_hs_out_send(0x34);

	if (hrslt < 0) {
		d_print("BULK-IN transmission error. HRSLT: 0x%01X\r\n",  -1 * hrslt);
		return;
	}

	d_print("b_length: 0x%02X\r\n", dev_descr1.b_length);
	d_print("b_descriptor_type: 0x%02X\r\n", dev_descr1.b_descriptor_type);
	d_print("bcd_usb: 0x%04X\r\n", dev_descr1.bcd_usb);
	d_print("b_device_class: 0x%02X\r\n", dev_descr1.b_device_class);
	d_print("b_device_sub_class: 0x%02X\r\n", dev_descr1.b_device_sub_class);
	d_print("b_device_protocol: 0x%02X\r\n", dev_descr1.b_device_protocol);
	d_print("b_max_packet_size: 0x%02X\r\n", dev_descr1.b_max_packet_size);
	d_print("id_vendor: 0x%04X\r\n", dev_descr1.id_vendor);
	d_print("id_product: 0x%04X\r\n", dev_descr1.id_product);
	d_print("bcd_device: 0x%04X\r\n", dev_descr1.bcd_device);
	d_print("i_manufacturer: 0x%02X\r\n", dev_descr1.i_manufacturer);
	d_print("i_product: 0x%02X\r\n", dev_descr1.i_product);
	d_print("i_serial_number: 0x%02X\r\n", dev_descr1.i_serial_number);
	d_print("b_num_configurations: 0x%02X\r\n", dev_descr1.b_num_configurations);
}

void kb_usb_setup_get_conf_descr(uint8_t dev_addr)
{
	d_print("\n\n\n");
	d_print("Getting configuration descriptor\r\n");

	struct configuration_descriptor conf_descr = {0x1,0x2,0x3,0x4,0x5,0x6,0x7,0x8};
	struct std_request get_dev_descr = {0x80, 0x06, 0x0200, 0x0000, sizeof(conf_descr)};

//	d_print("bm_request_type: 0x%02X\r\n", get_dev_descr.bm_request_type);
//	d_print("b_request: 0x%02X\r\n", get_dev_descr.b_request);
//	d_print("w_value: 0x%04X\r\n", get_dev_descr.w_value);
//	d_print("w_index: 0x%04X\r\n", get_dev_descr.w_index);
//	d_print("w_length: 0x%04X\r\n", get_dev_descr.w_length);
//	mdelay(10);

	int16_t hrslt = kb_usb_setup_send(dev_addr, &get_dev_descr);

	if (hrslt < 0) {
		d_print("SETUP transmission error. HRSLT: 0x%01X\r\n", -1 * hrslt);
		return;
	}

	/* It needs time to process request */
	mdelay(20);



	kb_usb_recv_tog_set(1);

	hrslt = kb_usb_bulk_receive(dev_addr, 0, &conf_descr, sizeof(conf_descr));

	kb_usb_hs_out_send(0x34);

	if (hrslt < 0) {
		d_print("BULK-IN transmission error. HRSLT: 0x%01X\r\n",  -1 * hrslt);
		return;
	}

	d_print("b_length: 0x%02X\r\n", conf_descr.b_length);
	d_print("b_descriptor_type: 0x%02X\r\n", conf_descr.b_descriptor_type);
	d_print("w_total_length: 0x%04X\r\n", conf_descr.w_total_length);
	d_print("b_num_interfaces: 0x%02X\r\n", conf_descr.b_num_interfaces);
	d_print("b_configuration_value: 0x%02X\r\n", conf_descr.b_configuration_value);
	d_print("i_configuration: 0x%02X\r\n", conf_descr.i_configuration);
	d_print("bm_attributes: 0x%02X\r\n", conf_descr.bm_attributes);
	d_print("b_max_power: 0x%02X\r\n", conf_descr.b_max_power);

}

void kb_usb_int_receive(uint8_t dev_addr)
{
	d_print("Getting interrupt data\r\n");

	spi_chip_activate();
	peraddr_write(dev_addr);
	spi_chip_deactivate();

//	spi_chip_activate();
//	uint8_t hctl = hctl_read();
//	spi_chip_deactivate();
//
//	spi_chip_activate();
//	hctl_write(hctl | HCTL_RCVTOG1);
//	spi_chip_deactivate();

	d_print("Send BULK/INTERRUPT IN\r\n");

	spi_chip_activate();
	hxfr_write(HXFR_BULKIN | USB_EP1);
	spi_chip_deactivate();

	spi_chip_activate();
	while((hirq_read() & HIRQ_HXFRDNIRQ) != HIRQ_HXFRDNIRQ) {
		spi_chip_deactivate();
		u100delay(1);
		spi_chip_activate();
	}
	spi_chip_deactivate();

	spi_chip_activate();
	hirq_write(HIRQ_HXFRDNIRQ);
	spi_chip_deactivate();

	spi_chip_activate();
	d_print("HRSLT: 0x%01X\r\n", hrsl_read() & 0x0F);
	spi_chip_deactivate();

	spi_chip_activate();
	while((hirq_read() & HIRQ_RCVDAVIRQ) != HIRQ_RCVDAVIRQ) {
		spi_chip_deactivate();
		u100delay(1);
		spi_chip_activate();
	}
	spi_chip_deactivate();

	uint8_t rcvbc = 0x00;

	spi_chip_activate();
	rcvbc = rcvbc_read();
	spi_chip_deactivate();

	d_print("rcvbc: 0x%02X\r\n", rcvbc);

	uint8_t array[8];

	spi_chip_activate();
	rcvfifo_read(array, sizeof(array));
	spi_chip_deactivate();

	d_print("Data: ");
	size_t idx = 0;
	for (; idx < sizeof(array); ++idx)
		d_print("%02X ", array[idx]);
	d_print("\r\n");

	spi_chip_activate();
	hirq_write(HIRQ_RCVDAVIRQ);
	spi_chip_deactivate();
}
