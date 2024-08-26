#include <io.h>
#include <stdint.h>
#include <uart.h>
#include <uart16550reg.h>
#include "uart8250reg.h"

/* Expected character delay at 1200bps is 9ms for a working UART
* and no flow-control. Assume UART as stuck if shift register
* or FIFO takes more than 50ms per character to appear empty.
*
* Estimated that inb() from UART takes 1 microsecond.
*/
#define SINGLE_CHAR_TIMEOUT	(50 * 1000)

void uart_16550_init(unsigned int base) {

    unsigned int	ComBase = base;
    unsigned int	Divisor;
    unsigned char	OutputData;
    unsigned char	Data;

    unsigned char	gData     = 8;
    unsigned char	gStop     = 1;
    unsigned char	gParity   = 0;
    unsigned char	gBreakSet = 0;

    Data = (unsigned char) (gData - (unsigned char) 5);

    Divisor = 115200 / 115200;

    OutputData = (unsigned char) ((DLAB << 7) | ((gBreakSet << 6) | ((gParity << 3) | ((gStop << 2) | Data))));
    outb (OutputData, ComBase + LCR_OFFSET);

    outb ((unsigned char) (Divisor >> 8), ComBase + BAUD_HIGH_OFFSET);
    outb ((unsigned char) (Divisor & 0xff), ComBase + BAUD_LOW_OFFSET);

    OutputData = (unsigned char) ((~DLAB << 7) | ((gBreakSet << 6) | ((gParity << 3) | ((gStop << 2) | Data))));
    outb (OutputData, ComBase + LCR_OFFSET);
}

void uart_8250_init(unsigned int base) {
    unsigned int baudrate = 115200;
    unsigned int refclk = baudrate * 16;
    unsigned int oversample = 16;
    unsigned int divisor = ((1 + (2 * refclk) / (baudrate * oversample)) / 2); // divisor = 3/2 =1
    unsigned int base_port = base;

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

void uart_tx_byte(unsigned int base, unsigned char data) {
    unsigned long int i = SINGLE_CHAR_TIMEOUT;

    while (i-- && !(inb(base + UART8250_LSR) & UART8250_LSR_THRE));

    outb(data, base + UART8250_TBR);
}

unsigned char uart_rx_byte(unsigned int base) {
    unsigned long int i = SINGLE_CHAR_TIMEOUT;
    
    while (i-- && !(inb(base + UART8250_LSR) & UART8250_LSR_DR));

    if (i)
	return inb(base + UART8250_RBR);
    else
	return 0x0;
}