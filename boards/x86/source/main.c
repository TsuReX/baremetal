#include <stdint.h>
#include <uart.h>

int c_entry(int base, int size) {

	uart_8250_init(0);

	uart_sio_init(1);

	uart_tx_byte(0, '5');

	unsigned char chr = uart_rx_byte(0);

	return base + size;
}