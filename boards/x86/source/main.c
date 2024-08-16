#include <stdint.h>
#include <uart.h>

int c_entry(int base, int size) {

    uart8250_init(0);

    uart_tx_byte(0, '5');

    uart_tx_flush(0);

    unsigned char chr = uart_rx_byte(0);

    return base + size;
}