#include <ite8613.h>

#define UART_DEV		PNP_DEV(0x2e, IT8613E_SP1)
#define IT8613E_SP1		0x01
#define PNP_DEV(PORT, FUNC)	(((PORT) << 8) | (FUNC))
#define PNP_IDX_IO0		0x60

/* For details see
 * https://doc.coreboot.org/superio/common/pnp.html
 *
 * coreboot/src/mainboard/cwwk/adl/bootblock.c
 * coreboot/src/superio/ite/it8613e/it8613e.h
 * coreboot/src/include/device/pnp_type.h
 * coreboot/src/include/device/pnp_def.h
 * coreboot/build/config.h
 * coreboot/src/superio/ite/common/early_serial.c
 */
void sio_ite8613_init(unsigned int base) {
    
    unsigned short	dev = UART_DEV;
    unsigned char	port = dev >> 8;
    unsigned char	device = dev & 0xff;
    
    //	enter conf state
    outb(0x87, port);
    outb(0x01, port);
    outb(0x55, port);
    outb((port == 0x4e) ? 0xaa : 0x55, port);

    //	set logical device
    outb(0x07, port);
    outb(device, port + 1);

    //	disable
    outb(0x30, port);
    outb(0x00, port + 1);

    //	set iobase
    outb(PNP_IDX_IO0 + 0, port);
    outb((base >> 8) & 0xff, port + 1);
    outb(PNP_IDX_IO0 + 1, port);
    outb(base & 0xff, port + 1);

    //	enable
    outb(0x30, port);
    outb(0x00, port + 1);

    //	exit conf state
    outb(0x02, port);
    outb(0x02, port + 1);
}