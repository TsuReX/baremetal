#include <string.h>
#include <stdint.h>
#include <uart.h>
#include <io.h>

void c_entry(int base, int size) {
    unsigned int uart_idx;

    outb(0x88, 0x80);

#if defined UART
    uart_idx = UART;
    uart_8250_init(uart_idx);
    outb(0x89, 0x80);

#elif defined UART_SIO
    uart_idx = UART_SIO;
    uart_sio_init(uart_idx);
    outb(0x8A, 0x80);

#else
    #error "UART wasn't specified"
#endif

    outb(0x8B, 0x80);
    unsigned char str[] = "EagleStream";
    for (unsigned int i = 0; i < strlen(str); ++i)
        uart_tx_byte(uart_idx, str[i]);

    outb(0x8C, 0x80);
    while(1) {
	outb(0x8D, 0x80);
	outb(0x8E, 0x80);
    }
    // This is used for compile time checking
    unsigned char chr = uart_rx_byte(uart_idx);
}