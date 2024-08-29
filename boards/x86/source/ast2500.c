#include <ast2500.h>
#include <io.h>

void sio_ast2500_init (unsigned int base) {

    unsigned short  lpciod;
    unsigned short  lpcioe;

    // Enable LPC decode
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
