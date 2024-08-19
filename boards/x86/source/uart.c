
#include <io.h>
#include <helpers.h>
#include <stdint.h>
#include "uart8250reg.h"

/* Should support 8250, 16450, 16550, 16550A type UARTs */

/* Expected character delay at 1200bps is 9ms for a working UART
 * and no flow-control. Assume UART as stuck if shift register
 * or FIFO takes more than 50ms per character to appear empty.
 *
 * Estimated that inb() from UART takes 1 microsecond.
 */
#define SINGLE_CHAR_TIMEOUT	(50 * 1000)
#define FIFO_TIMEOUT		(16 * SINGLE_CHAR_TIMEOUT)
#define CONFIG_TTYS0_LCS 3

static const unsigned int bases[] = { 0x3F8, 0x2F8, 0x3E8, 0x2E8 };

uintptr_t uart_platform_base(unsigned int idx)
{
	if (idx < ARRAY_SIZE(bases))
		return bases[idx];
	return 0;
}

#include <io.h>
#include <uart.h>
#include <uart16550reg.h>

typedef struct {
    unsigned char   Index;
    unsigned char   Data;
} SIO_REG_TABLE;

static  SIO_REG_TABLE  mASPEED2500Table [] = {
    { REG_LOGICAL_DEVICE,       ASPEED2500_SIO_UART1 },
    { ACTIVATE,                 0x01 },
    { PRIMARY_INTERRUPT_SELECT, 0x04 },    // COMA IRQ routing
    { INTERRUPT_TYPE,                   0x01 },    // COMA Interrupt Type
    { REG_LOGICAL_DEVICE,       ASPEED2500_SIO_UART2 },
    { ACTIVATE,                 0x01 },
    { PRIMARY_INTERRUPT_SELECT, 0x03 },    // COMB IRQ routing
    { INTERRUPT_TYPE,                   0x01 }     // COMB Interrupt Type
};

unsigned int is_aspeed_present(void){
    unsigned int PresenceStatus = 0;

    //
    //ASPEED AST2500/AST2600
    //
    outb(ASPEED2500_SIO_UNLOCK, ASPEED2500_SIO_INDEX_PORT);
    outb(ASPEED2500_SIO_UNLOCK, ASPEED2500_SIO_INDEX_PORT);
    outb(REG_LOGICAL_DEVICE, ASPEED2500_SIO_INDEX_PORT);
    outb(ASPEED2500_SIO_UART1, ASPEED2500_SIO_DATA_PORT);

    if (inb(ASPEED2500_SIO_DATA_PORT) == ASPEED2500_SIO_UART1) {
        //
        //There is a Aspeed card need to support as well. it's type is AST2500 A1 EVB.
        //
        //AST2300-A0 0x01000003
        //AST2300-A1 0x01010303
        //AST1300-A1 0x01010003
        //AST1050-A1 0x01010203
        //AST2400-A0 0x02000303
        //AST2400-A1 0x02010303
        //AST1400-A1 0x02010103
        //AST1250-A1 0x02010303
        //AST2500-A0 0x04000303
        //AST2510-A0 0x04000103
        //AST2520-A0 0x04000203
        //AST2530-A0 0x04000403
        //AST2500-A1 0x04010303
        //AST2510-A1 0x04010103
        //AST2520-A1 0x04010203
        //AST2530-A1 0x04010403

        //if ((ReadAHBDword (SCU7C) & 0xff0000ff) == 0x04000003) {
        //PresenceStatus = 1;
        //}

        PresenceStatus = 1;
    }

    outb(ASPEED2500_SIO_LOCK, ASPEED2500_SIO_INDEX_PORT);
    return PresenceStatus;
}

unsigned int is_sio_exist (void) {
    unsigned int   SioExist = 0;

    if (is_aspeed_present ()) {
        SioExist |= ASPEED_EXIST;
    }

//    if (IsNuvotonPresent ()) {
//      SioExist |= NCT5104D_EXIST;
//    }

    return SioExist;
}

static void sio_init (unsigned int ComBase) {
    unsigned int  SioExist;
    unsigned int  SioEnable;
    unsigned int  Index;
    unsigned int  Decode;
    unsigned int  Enable;
    unsigned int  SpiConfigValue;

    //
    // Enable LPC decode
    // Set COMA/COMB base
    //
    Decode =  ((V_LPC_CFG_IOD_COMA_3F8 << N_LPC_CFG_IOD_COMA) | (V_LPC_CFG_IOD_COMB_2F8 << N_LPC_CFG_IOD_COMB));
    SpiConfigValue = MmioRead32 (PCH_PCR_ADDRESS (PID_ESPISPI, R_PCH_PCR_SPI_CONF_VALUE));
    if (SpiConfigValue & B_ESPI_ENABLE_STRAP) {
        Enable =  ( B_LPC_CFG_IOE_ME2 | B_LPC_CFG_IOE_SE | B_LPC_CFG_IOE_ME1 \
        | B_LPC_CFG_IOE_CBE | B_LPC_CFG_IOE_CAE);
    } else {
        Enable =  ( B_LPC_CFG_IOE_ME2 | B_LPC_CFG_IOE_SE | B_LPC_CFG_IOE_ME1 \
        | B_LPC_CFG_IOE_KE | B_LPC_CFG_IOE_CBE | B_LPC_CFG_IOE_CAE);
    }
    outl((unsigned int) (PCH_LPC_CF8_ADDR (R_LPC_CFG_IOD)), R_PCH_IOPORT_PCI_INDEX);

    outl(Decode | (Enable << 16), R_PCH_IOPORT_PCI_DATA);

    MmioWrite16 (PCH_PCR_ADDRESS(PID_DMI, R_PCH_DMI_PCR_LPCIOD), (unsigned short)Decode);
    MmioWrite16 (PCH_PCR_ADDRESS(PID_DMI, R_PCH_DMI_PCR_LPCIOE), (unsigned short)Enable);

    SioExist = is_sio_exist();

    if ((SioExist & (PILOTIV_EXIST | PC8374_EXIST)) == (PILOTIV_EXIST | PC8374_EXIST) ) {
        //
        // Both are there, we use DEFAULT_SIO as debug port anyway
        //
        if (DEFAULT_SIO == PILOTIV_EXIST) {
            SioEnable = PILOTIV_EXIST;
        } else {
            SioEnable = PC8374_EXIST;
        }
    } else {
        SioEnable = SioExist;
    }

    //
    // ASPEED AST2500/AST2600 UART init.
    //
    if (SioEnable == ASPEED_EXIST) {
        //
        // Unlock SIO
        //
        outb (ASPEED2500_SIO_UNLOCK, ASPEED2500_SIO_INDEX_PORT);
        outb (ASPEED2500_SIO_UNLOCK, ASPEED2500_SIO_INDEX_PORT);

        //
        // COM1 & COM2
        //
        for (Index = 0; Index < sizeof (mASPEED2500Table)/sizeof (SIO_REG_TABLE); Index++) {
            outb (mASPEED2500Table[Index].Index, ASPEED2500_SIO_INDEX_PORT);
            outb (mASPEED2500Table[Index].Data, ASPEED2500_SIO_DATA_PORT);
        }

        //
        // Lock SIO
        //
        outb (ASPEED2500_SIO_LOCK, ASPEED2500_SIO_INDEX_PORT);
    }
}

static unsigned int uart16550_init (unsigned int ComBase) {
    unsigned int        Divisor;
    unsigned char       OutputData;
    unsigned char       Data;

    unsigned char  gData     = 8;
    unsigned char  gStop     = 1;
    unsigned char  gParity   = 0;
    unsigned char  gBreakSet = 0;

    Data = (unsigned char) (gData - (unsigned char) 5);

    Divisor = 115200 / 115200;

    OutputData = (unsigned char) ((DLAB << 7) | ((gBreakSet << 6) | ((gParity << 3) | ((gStop << 2) | Data))));
    outb (OutputData, ComBase + LCR_OFFSET);

    outb ((unsigned char) (Divisor >> 8), ComBase + BAUD_HIGH_OFFSET);
    outb ((unsigned char) (Divisor & 0xff), ComBase + BAUD_LOW_OFFSET);

    OutputData = (unsigned char) ((~DLAB << 7) | ((gBreakSet << 6) | ((gParity << 3) | ((gStop << 2) | Data))));
    outb (OutputData, ComBase + LCR_OFFSET);

    return 1;
}

void uart_sio_init(unsigned int idx) {

    sio_init(uart_platform_base(idx));

    uart16550_init(uart_platform_base(idx));
}

void uart_8250_init(unsigned int idx)
{
	unsigned int baudrate = 115200;
	unsigned int refclk = baudrate * 16;
	unsigned int oversample = 16;
	unsigned int divisor = ((1 + (2 * refclk) / (baudrate * oversample)) / 2); // divisor = 3/2 =1
	unsigned int base_port = uart_platform_base(idx);

	/* Disable interrupts */
	outb(0x0, base_port + UART8250_IER);
	/* Enable FIFOs */
	outb(UART8250_FCR_FIFO_EN, base_port + UART8250_FCR);

	/* assert DTR and RTS so the other end is happy */
	outb(UART8250_MCR_DTR | UART8250_MCR_RTS, base_port + UART8250_MCR);

	/* DLAB on */
	outb(UART8250_LCR_DLAB | CONFIG_TTYS0_LCS, base_port + UART8250_LCR);

	/* Set Baud Rate Divisor. 12 ==> 9600 Baud */
	outb(divisor & 0xFF,   base_port + UART8250_DLL);
	outb((divisor >> 8) & 0xFF,    base_port + UART8250_DLM);

	/* Set to 3 for 8N1 */
	outb(CONFIG_TTYS0_LCS, base_port + UART8250_LCR);
}

static int uart8250_can_tx_byte(unsigned int base_port)
{
	return inb(base_port + UART8250_LSR) & UART8250_LSR_THRE;
}

void uart_tx_byte(unsigned int idx, unsigned char data)
{
	unsigned int base_port = uart_platform_base(idx);
	unsigned long int i = SINGLE_CHAR_TIMEOUT;
	while (i-- && !uart8250_can_tx_byte(base_port));

	outb(data, base_port + UART8250_TBR);
}

static int uart8250_can_rx_byte(unsigned int base_port)
{
	return inb(base_port + UART8250_LSR) & UART8250_LSR_DR;
}

unsigned char uart_rx_byte(unsigned int idx)
{
	unsigned int base_port = uart_platform_base(idx);
	unsigned long int i = SINGLE_CHAR_TIMEOUT;
	while (i-- && !uart8250_can_rx_byte(base_port));

	if (i)
		return inb(base_port + UART8250_RBR);
	else
		return 0x0;
}

/*
void uart_tx_flush(unsigned int idx)
{
	uart8250_tx_flush(uart_platform_base(idx));
}

static void uart8250_tx_flush(unsigned int base_port)
{
	unsigned long int i = FIFO_TIMEOUT;
	while (i-- && !(inb(base_port + UART8250_LSR) & UART8250_LSR_TEMT));
}

enum cb_err fill_lb_serial(struct lb_serial *serial)
{
	serial->type = LB_SERIAL_TYPE_IO_MAPPED;
	serial->baseaddr = uart_platform_base(CONFIG_UART_FOR_CONSOLE);
	serial->baud = get_uart_baudrate();
	serial->regwidth = 1;
	serial->input_hertz = uart_platform_refclk();

	return CB_SUCCESS;
}
*/