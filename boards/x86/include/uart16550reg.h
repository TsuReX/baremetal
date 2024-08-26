#ifndef __UART_16550__
#define __UART_16550__

//
// ---------------------------------------------
// UART 16550 Register Offsets
// ---------------------------------------------
//
#define BAUD_LOW_OFFSET   0x00
#define BAUD_HIGH_OFFSET  0x01
#define IER_OFFSET        0x01
#define LCR_SHADOW_OFFSET 0x01
#define FCR_SHADOW_OFFSET 0x02
#define IR_CONTROL_OFFSET 0x02
#define FCR_OFFSET        0x02
#define EIR_OFFSET        0x02
#define BSR_OFFSET        0x03
#define LCR_OFFSET        0x03
#define MCR_OFFSET        0x04
#define LSR_OFFSET        0x05
#define MSR_OFFSET        0x06

//
// ---------------------------------------------
// UART 16550 Register Bit Defines
// ---------------------------------------------
//
#define LSR_TXRDY 0x20
#define LSR_RXDA  0x01
#define DLAB      0x01

#define UART_DATA    8
#define UART_STOP    1
#define UART_PARITY  0
#define UART_BREAK_SET  0

#endif // __UART_16550__