#ifndef __UART_16550__
#define __UART_16550__

/// AST2600 Integrated Remote Management Processor A3 Datasheet
/// Chapter 50 SuperIO Controller (SIO)
#define REG_LOGICAL_DEVICE                      0x7
#define ASPEED2500_SIO_UART1            0x2
#define ASPEED2500_SIO_UART2            0x3
#define ACTIVATE                                        0x30
#define PRIMARY_INTERRUPT_SELECT        0x70
#define INTERRUPT_TYPE                          0x71

//
// ---------------------------------------------
// UART 16550 Register Offsets
// ---------------------------------------------
//
#define BAUD_LOW_OFFSET   0x00
#define BAUD_HIGH_OFFSET  0x01
#define IER_OFFSET        0x01
#define LCR_SHADOW_OFFSET 0x01
#define FCR_SHADOW_OFFSET 0x02
#define IR_CONTROL_OFFSET 0x02
#define FCR_OFFSET        0x02
#define EIR_OFFSET        0x02
#define BSR_OFFSET        0x03
#define LCR_OFFSET        0x03
#define MCR_OFFSET        0x04
#define LSR_OFFSET        0x05
#define MSR_OFFSET        0x06

//
// ---------------------------------------------
// UART 16550 Register Bit Defines
// ---------------------------------------------
//
#define LSR_TXRDY 0x20
#define LSR_RXDA  0x01
#define DLAB      0x01

#define UART_DATA    8
#define UART_STOP    1
#define UART_PARITY  0
#define UART_BREAK_SET  0

#define V_LPC_CFG_IOD_COMA_3F8          0x0
#define N_LPC_CFG_IOD_COMA                      0x0
#define V_LPC_CFG_IOD_COMB_2F8          0x1
#define N_LPC_CFG_IOD_COMB                      0x4
#define PID_ESPISPI                                     0xEE
#define R_PCH_PCR_SPI_CONF_VALUE        0xC00C
#define B_ESPI_ENABLE_STRAP                     (1 << 0)
#define B_LPC_CFG_IOE_ME2                       (1 << 13)
#define B_LPC_CFG_IOE_SE                        (1 << 12)
#define B_LPC_CFG_IOE_ME1                       (1 << 11)
#define B_LPC_CFG_IOE_KE                        (1 << 10)
#define B_LPC_CFG_IOE_CBE                       (1 << 1)
#define B_LPC_CFG_IOE_CAE                       (1 << 0)
#define R_PCH_IOPORT_PCI_INDEX          0xCF8
#define R_PCH_IOPORT_PCI_DATA           0xCFC
#define DEFAULT_PCI_BUS_NUMBER_PCH  0x0
#define PCI_DEVICE_NUMBER_PCH_LPC       0x1F
#define PCI_FUNCTION_NUMBER_PCH_LPC     0x0
#define PCI_CF8_ADDR(Bus, Dev, Func, Off) \
          (((Off) & 0xFF) | (((Func) & 0x07) << 8) | (((Dev) & 0x1F) << 11) | (((Bus) & 0xFF) << 16) | (1 << 31))
#define PCH_LPC_CF8_ADDR(Offset)    PCI_CF8_ADDR(DEFAULT_PCI_BUS_NUMBER_PCH, PCI_DEVICE_NUMBER_PCH_LPC, PCI_FUNCTION_NUMBER_PCH_LPC, Offset)
#define R_LPC_CFG_IOD                           0x0
#define PID_DMI                                         0xEF
#define R_PCH_DMI_PCR_LPCIOD            0x2770
#define R_PCH_DMI_PCR_LPCIOE            0x2774
#define PILOTIV_EXIST                           (1 << 0)
#define PC8374_EXIST                            (1 << 1)
#define W83527_EXIST                            (1 << 2)
#define NCT5104D_EXIST                          (1 << 3)
#define ASPEED_EXIST                            (1 << 4)
#define  DEFAULT_SIO                    PILOTIV_EXIST
#define ASPEED2500_SIO_INDEX_PORT       0x2E
#define ASPEED2500_SIO_UNLOCK           0xA5
#define ASPEED2500_SIO_DATA_PORT        0x2F
#define ASPEED2500_SIO_LOCK                     0xAA


///
/// Definition for PCR base address (defined in PchReservedResources.h)
///
#define PCH_PCR_BASE_ADDRESS            0xFD000000
#define PCH_PCR_MMIO_SIZE               0x01000000
#define PCH_PCR_ADDRESS(Pid, Offset)    (PCH_PCR_BASE_ADDRESS | ((unsigned char) (Pid) << 16) | (unsigned short) (Offset))
#endif // __UART_16550__