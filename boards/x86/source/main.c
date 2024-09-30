#include <string.h>
#include <stdint.h>
#include <uart.h>
#include <io.h>
#include <ite8613.h>
#include <ast2500.h>
#include <dbg.h>

#define LOCAL		1
#define ITE8613		2
#define AST2500		3

static const unsigned int bases[] = { 0x3F8, 0x2F8, 0x3E8, 0x2E8 };

static unsigned int uart_platform_base(unsigned int idx) {
    if (idx < (sizeof(bases) /sizeof(bases[0])) )
	return bases[idx];
    return 0x03F8;
}

void c_entry(int stack_base, int size) {

    outb(0x10, 0x80);

    unsigned int base = uart_platform_base(UART_NUM);

#if (UART_TYPE == LOCAL)

#elif (UART_TYPE == ITE8613)
    sio_ite8613_init(base);

#elif (UART_TYPE == AST2500)
    sio_ast2500_init(base);

#else
    #error "Incorrect UART_TYPE"
#endif

    outb(0x11, 0x80);

    uart_init(base);

    outb(0x12, 0x80);

    unsigned char str3f8[] = "EagleStream_3f8";
    for (unsigned int i = 0; i < strlen(str3f8); ++i) {
	uart_tx_byte(base, str3f8[i]);
    }

    outb(0x13, 0x80);
    while(1) {
	outb(0xFE, 0x80);
	outb(0xFF, 0x80);
    }
    // This is used for compile time checking
    unsigned char chr = uart_rx_byte(base);
}