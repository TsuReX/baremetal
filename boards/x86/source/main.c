#include <string.h>
#include <stdint.h>
#include <uart.h>
#include <io.h>
#include <ite8613.h>
#include <ast2500.h>

#define LOCAL		1
#define ITE8613		2
#define AST2500		3

static const unsigned int bases[] = { 0x3F8, 0x2F8, 0x3E8, 0x2E8 };

static unsigned int uart_platform_base(unsigned int idx) {
    if (idx < (sizeof(bases) /sizeof(bases[0])) )
    return bases[idx];
    return 0;
}

static void read_flash() {
    volatile unsigned int var = 0;
    unsigned int *ptr = (unsigned int *)0xFFFFFFFC;
    for (; ptr > (unsigned int *)0xFFFF0000; ptr--)
	var = *ptr;
}

void c_entry(int stack_base, int size) {

    unsigned int base = uart_platform_base(UART_NUM);

    outb(0x88, 0x80);
    read_flash();

    outb(0x78, 0x80);
    read_flash();

    outb(0x68, 0x80);
    read_flash();

#if (UART_TYPE == LOCAL)

#elif (UART_TYPE == ITE8613)
    sio_ite8613_init(base);

#elif (UART_TYPE == AST2500)
    sio_ast2500_init(base);

#else
    #error "Incorrect UART_TYPE"

#endif
    uart_init(base);
//    uart_8250_init(base);
//    uart_16550_init(base);

    outb(0x8A, 0x80);

    outb(0x8B, 0x80);
    unsigned char str[] = "EagleStream";
    for (unsigned int i = 0; i < strlen(str); ++i)
        uart_tx_byte(base, str[i]);

    outb(0x8C, 0x80);
    while(1) {
	outb(0x8D, 0x80);
	outb(0x8E, 0x80);
    }
    // This is used for compile time checking
    unsigned char chr = uart_rx_byte(base);
}