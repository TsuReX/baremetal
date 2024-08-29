#include <ast2500.h>
#include <io.h>
/*
static unsigned int pci_io_encode_addr(unsigned int dev, unsigned short reg) {
    
    unsigned int addr = 1U << 31;

    addr |= dev >> 4;
    addr |= reg & 0xfc;

    if (CONFIG(PCI_IO_CFG_EXT))
	addr |= (reg & 0xf00) << 16;

    return addr;
}
*/
static void p2sb_init(void) {

#define	PCH_DEV_SLOT_LPC			0x1f
#define	PCI_DEV(_bus, _dev, _fn)	(0x80000000 | (unsigned int)(_bus << 16) | (unsigned int)(_dev << 11) | (unsigned int)(_fn << 8))
#define	_PCH_DEV(slot, func)		PCI_DEV(0, PCH_DEV_SLOT_ ## slot, func)
#define	PCH_DEV_P2SB				_PCH_DEV(LPC, 1)

#define	PCI_COMMAND				0x04	/* 16 bits */
#define	PCI_COMMAND_MEMORY		0x02	/* Enable response in Memory space */
#define	PCI_BASE_ADDRESS_0		0x10	/* 32 bits */
#define CONFIG_PCR_BASE_ADDRESS	0xFD000000

//PCI_DEV(0, 0x1f, 1) == PCH_DEV_P2SB

    /* Set up P2SB BAR. This is needed for PCR to work */
//	PCI_CF8_ADDR(Bus, Dev, Func, Off)
//	PCI_CF8_ADDR(0, 0x1F, 1, Off)
    //unsigned char p2sb_cmd = pci_s_read_config8(PCH_DEV_P2SB, PCI_COMMAND);
    outl(PCH_DEV_P2SB, R_PCH_IOPORT_PCI_INDEX);
    unsigned char p2sb_cmd = inb(R_PCH_IOPORT_PCI_DATA);
    
    //pci_s_write_config8(PCH_DEV_P2SB, PCI_COMMAND, p2sb_cmd | PCI_COMMAND_MEMORY);
    outl(PCI_CF8_ADDR(0, 0x1F, 1, PCI_COMMAND), R_PCH_IOPORT_PCI_INDEX);
    outb(p2sb_cmd | PCI_COMMAND_MEMORY, R_PCH_IOPORT_PCI_DATA);
    
    //pci_s_write_config32(PCH_DEV_P2SB, PCI_BASE_ADDRESS_0, CONFIG_PCR_BASE_ADDRESS);
    outl(PCI_CF8_ADDR(0, 0x1F, 1, PCI_BASE_ADDRESS_0), R_PCH_IOPORT_PCI_INDEX);
    outl(CONFIG_PCR_BASE_ADDRESS, R_PCH_IOPORT_PCI_DATA);
}

void sio_ast2500_init (unsigned int base) {

    unsigned short  lpciod;
    unsigned short  lpcioe;

    p2sb_init();

    // Enable LPC decode
    // Set COMA/COMB base
    lpciod =  (V_LPC_CFG_IOD_COMA_3F8 << N_LPC_CFG_IOD_COMA) | (V_LPC_CFG_IOD_COMB_2F8 << N_LPC_CFG_IOD_COMB);
    lpcioe =  (B_LPC_CFG_IOE_SE | B_LPC_CFG_IOE_CBE | B_LPC_CFG_IOE_CAE);

    outl((PCH_LPC_CF8_ADDR (R_LPC_CFG_IOD)), R_PCH_IOPORT_PCI_INDEX);
    outw(lpciod, R_PCH_IOPORT_PCI_DATA);

    outl((PCH_LPC_CF8_ADDR (R_LPC_CFG_IOE)), R_PCH_IOPORT_PCI_INDEX);
    outw(lpcioe, R_PCH_IOPORT_PCI_DATA);

    MmioWrite16 (PCH_PCR_ADDRESS(PID_DMI, R_PCH_DMI_PCR_LPCIOD), (unsigned short)lpciod);
    MmioWrite16 (PCH_PCR_ADDRESS(PID_DMI, R_PCH_DMI_PCR_LPCIOE), (unsigned short)lpcioe);

    // Unlock SIO
    outb (ASPEED2500_SIO_UNLOCK, ASPEED2500_SIO_INDEX_PORT);
    outb (ASPEED2500_SIO_UNLOCK, ASPEED2500_SIO_INDEX_PORT);

    // COM1
    outb (REG_LOGICAL_DEVICE, ASPEED2500_SIO_INDEX_PORT);
    outb (ASPEED2500_SIO_UART1, ASPEED2500_SIO_DATA_PORT);

    outb (ACTIVATE, ASPEED2500_SIO_INDEX_PORT);
    outb (0x00, ASPEED2500_SIO_DATA_PORT);

#define SUART_BASEADDR_HIGH 0x60
    outb(SUART_BASEADDR_HIGH, ASPEED2500_SIO_INDEX_PORT);
    outb((base >> 8) & 0xff, ASPEED2500_SIO_DATA_PORT);

#define SUART_BASEADDR_LOW 0x61
    outb(SUART_BASEADDR_LOW, ASPEED2500_SIO_INDEX_PORT);
    outb(base & 0xff, ASPEED2500_SIO_DATA_PORT);

//	outb (PRIMARY_INTERRUPT_SELECT, ASPEED2500_SIO_INDEX_PORT);
//	outb (0x04, ASPEED2500_SIO_DATA_PORT);

//	outb (INTERRUPT_TYPE, ASPEED2500_SIO_INDEX_PORT);
//	outb (0x01, ASPEED2500_SIO_DATA_PORT);

    outb (ACTIVATE, ASPEED2500_SIO_INDEX_PORT);
    outb (0x01, ASPEED2500_SIO_DATA_PORT);

    // COM2
    outb (REG_LOGICAL_DEVICE, ASPEED2500_SIO_INDEX_PORT);
    outb (ASPEED2500_SIO_UART2, ASPEED2500_SIO_DATA_PORT);

    outb (ACTIVATE, ASPEED2500_SIO_INDEX_PORT);
    outb (0x00, ASPEED2500_SIO_DATA_PORT);

    outb(SUART_BASEADDR_HIGH, ASPEED2500_SIO_INDEX_PORT);
    outb((base >> 8) & 0xff, ASPEED2500_SIO_DATA_PORT);

    outb(SUART_BASEADDR_LOW, ASPEED2500_SIO_INDEX_PORT);
    outb(base & 0xff, ASPEED2500_SIO_DATA_PORT);

//	outb (PRIMARY_INTERRUPT_SELECT, ASPEED2500_SIO_INDEX_PORT);
//	outb (0x03, ASPEED2500_SIO_DATA_PORT);

//	outb (INTERRUPT_TYPE, ASPEED2500_SIO_INDEX_PORT);
//	outb (0x01, ASPEED2500_SIO_DATA_PORT);

    outb (ACTIVATE, ASPEED2500_SIO_INDEX_PORT);
    outb (0x01, ASPEED2500_SIO_DATA_PORT);

    // Lock SIO
    outb (ASPEED2500_SIO_LOCK, ASPEED2500_SIO_INDEX_PORT);
}