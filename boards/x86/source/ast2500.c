#include <pch.h>
#include <ast2500.h>
#include <io.h>

static void p2sb_init(void) {

    // Set up P2SB BAR. This is needed for PCR to work 
    outl(P2SB_PCI_CF8_ADDR(R_P2SB_CFG_CMD), PCI_IOPORT_INDEX);
    unsigned char p2sb_cmd = inb(PCI_IOPORT_DATA);

    outl(P2SB_PCI_CF8_ADDR(R_P2SB_CFG_CMD), PCI_IOPORT_INDEX);
    outb(p2sb_cmd | B_P2SB_CFG_CMD_MSE, PCI_IOPORT_DATA);

    outl(P2SB_PCI_CF8_ADDR(R_P2SB_CFG_BAR0), PCI_IOPORT_INDEX);
    outl(PCH_PCR_BASE_ADDRESS, PCI_IOPORT_DATA);
}

void sio_ast2500_init (unsigned int base) {

    unsigned short  lpciod;
    unsigned short  lpcioe;

    p2sb_init();

    // Enable LPC decode
    // Set COMA/COMB base
    lpciod =  (V_LPC_CFG_IOD_COMA_3F8 << N_LPC_CFG_IOD_COMA) | (V_LPC_CFG_IOD_COMB_2F8 << N_LPC_CFG_IOD_COMB);
    lpcioe =  (B_LPC_CFG_IOE_SE | B_LPC_CFG_IOE_CBE | B_LPC_CFG_IOE_CAE);

    // To allow the I/O cycles and memory mapped cycles to go to the LPC interface, the PCH
    // includes several decoders. During configuration, the PCH must be programmed with the
    // same decode ranges as the peripheral. The decoders are programmed using the D
    // 31:F0 configuration space.
    outl((LPC_PCI_CF8_ADDR (R_LPC_CFG_IOD)), PCI_IOPORT_INDEX);
    outw(lpciod, PCI_IOPORT_DATA);

    outl((LPC_PCI_CF8_ADDR (R_LPC_CFG_IOE)), PCI_IOPORT_INDEX);
    outw(lpcioe, PCI_IOPORT_DATA);

    // Setting up PCH PCR register to allow I/O translations via LPC
    MmioWrite16 (PCH_PCR_ADDRESS(PID_DMI, R_PCH_DMI_PCR_LPCIOD), (unsigned short)lpciod);
    MmioWrite16 (PCH_PCR_ADDRESS(PID_DMI, R_PCH_DMI_PCR_LPCIOE), (unsigned short)lpcioe);


    // Setting up SIO via LPC interface
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