/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef CONSOLE_SIO_UART_H
#define CONSOLE_SIO_UART_H

#include <stdint.h>

void sio_uart_init(void);
void uart_tx_byte(unsigned int idx, unsigned char data);
void uart_tx_flush(unsigned int idx);
unsigned char uart_rx_byte(unsigned int idx);

#endif //CONSOLE_SIO_UART_H