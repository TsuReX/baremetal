/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef CONSOLE_UART_H
#define CONSOLE_UART_H

#include <stdint.h>

void uart_init(unsigned int idx);
void uart_tx_byte(unsigned int idx, unsigned char data);
void uart_tx_flush(unsigned int idx);
unsigned char uart_rx_byte(unsigned int idx);

#endif //CONSOLE_UART_H