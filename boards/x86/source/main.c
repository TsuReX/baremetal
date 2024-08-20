#include <string.h>
#include <stdint.h>
#include <uart.h>

void c_entry(int base, int size) {
    unsigned int uart_idx;

#if defined UART
    uart_idx = UART;
    uart_8250_init(uart_idx);
#elif defined UART_SIO
    uart_idx = UART_SIO;
    uart_sio_init(uart_idx);
#else
    #error "UART wasn't specified"
#endif
    unsigned char str[] = "EagleStream";
    for (unsigned int i = 0; i < strlen(str); ++i)
        uart_tx_byte(uart_idx, str[i]);

    while(1);
    // This is used for compile time checking
    unsigned char chr = uart_rx_byte(uart_idx);
}