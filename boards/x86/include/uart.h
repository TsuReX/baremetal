#ifndef CONSOLE_UART_H
#define CONSOLE_UART_H

#include <stdint.h>

void uart_16650_init(unsigned int base);
void uart_8250_init(unsigned int base);
void uart_tx_byte(unsigned int idx, unsigned char data);
unsigned char uart_rx_byte(unsigned int idx);

#endif //CONSOLE_UART_H