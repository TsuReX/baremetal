#ifndef __AST2500__
#define __AST2500__

#include <stdint.h>
#include <io.h>

// AST2600 Integrated Remote Management Processor A3 Datasheet
// Chapter 50 SuperIO Controller (SIO)
#define REG_LOGICAL_DEVICE		0x7
#define ASPEED2500_SIO_UART1		0x2
#define ASPEED2500_SIO_UART2		0x3
#define ACTIVATE			0x30
#define PRIMARY_INTERRUPT_SELECT	0x70
#define INTERRUPT_TYPE			0x71

#define ASPEED2500_SIO_INDEX_PORT	0x2E
#define ASPEED2500_SIO_UNLOCK		0xA5
#define ASPEED2500_SIO_DATA_PORT	0x2F
#define ASPEED2500_SIO_LOCK		0xAA

void sio_ast2500_init(unsigned int);

#endif // __AST2500__