#include <io.h>
#include <stdint.h>
#include <uart.h>
#include "uart8250reg.h"

/* Expected character delay at 1200bps is 9ms for a working UART
* and no flow-control. Assume UART as stuck if shift register
* or FIFO takes more than 50ms per character to appear empty.
*
* Estimated that inb() from UART takes 1 microsecond.
*/
#define SINGLE_CHAR_TIMEOUT	(50 * 1000)

void uart_init(unsigned int base) {
    unsigned int baudrate = 115200;
    unsigned int divisor = 1;

    /* Disable interrupts */
    outb(0x0, base + UART8250_IER);

    /* Disable FIFOs */
    outb(0x0, base + UART8250_FCR);

    /* Reset modem state */
    outb(0x0, base + UART8250_MCR);

    /* Setup transmission mode */
    outb(UART8250_LCR_DLAB | UART8250_LCR_WLS_8, base + UART8250_LCR);

    /* Setup baudrate divisor */
    outb(divisor & 0xFF,   base + UART8250_DLL);
    outb((divisor >> 8) & 0xFF,    base + UART8250_DLH);

    /* Setup transmission mode */
    outb(UART8250_LCR_WLS_8, base + UART8250_LCR);

    /* Enable FIFOs */
    outb(UART8250_FCR_FIFO_EN, base + UART8250_FCR);

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