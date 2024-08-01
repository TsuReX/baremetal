#include <io.h>


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

typedef struct {
    unsigned char   Index;
    unsigned char   Data;
} SIO_REG_TABLE;

static  SIO_REG_TABLE  mASPEED2500Table [] = {
    { REG_LOGICAL_DEVICE,       ASPEED2500_SIO_UART1 },
    { ACTIVATE,                 0x01 },
    { PRIMARY_INTERRUPT_SELECT, 0x04 },    // COMA IRQ routing
    { INTERRUPT_TYPE,                   0x01 },    // COMA Interrupt Type
    { REG_LOGICAL_DEVICE,       ASPEED2500_SIO_UART2 },
    { ACTIVATE,                 0x01 },
    { PRIMARY_INTERRUPT_SELECT, 0x03 },    // COMB IRQ routing
    { INTERRUPT_TYPE,                   0x01 }     // COMB Interrupt Type
};

typedef enum {
    FilterWidth8,
    FilterWidth16,
    FilterWidth32,
    FilterWidth64
} FILTER_IO_WIDTH;

/**
  Filter memory IO before Read operation.

  It will return the flag to decide whether require read real MMIO.
  It can be used for emulation environment.

  @param[in]       Width    Signifies the width of the memory I/O operation.
  @param[in]       Address  The base address of the memory I/O operation.
  @param[in,out]   Buffer   The destination buffer to store the results.

  @retval 1         Need to excute the MMIO read.
  @retval 0        Skip the MMIO read.

**/
unsigned int FilterBeforeMmIoRead(FILTER_IO_WIDTH  Width, unsigned int Address, void *Buffer) {
    return 1;
}

/**
  Tracer memory IO after read operation.

  @param[in]       Width    Signifies the width of the memory I/O operation.
  @param[in]       Address  The base address of the memory I/O operation.
  @param[in]       Buffer   The destination buffer to store the results.

**/
void FilterAfterMmIoRead(FILTER_IO_WIDTH  Width, unsigned int Address, void *Buffer) {
    return;
}

/**
    Filter memory IO before write operation.
    It will return the flag to decide whether require wirte real MMIO.
    It can be used for emulation environment.

    @param[in]       Width    Signifies the width of the memory I/O operation.
    @param[in]       Address  The base address of the memory I/O operation.
    @param[in]       Buffer   The source buffer from which to write data.

    @retval 1         Need to excute the MMIO write.
    @retval 0        Skip the MMIO write.

**/
unsigned int FilterBeforeMmIoWrite(FILTER_IO_WIDTH  Width, unsigned int Address, void   *Buffer) {
    return 1;
}

/**
    Tracer memory IO after write operation.

    @param[in]       Width    Signifies the width of the memory I/O operation.
    @param[in]       Address  The base address of the memory I/O operation.
    @param[in]       Buffer   The source buffer from which to write data.
**/
void FilterAfterMmIoWrite(FILTER_IO_WIDTH  Width, unsigned int  Address, void   *Buffer) {
    return;
}

/**
  Used to serialize load and store operations.
  All loads and stores that proceed calls to this function are guaranteed to be
  globally visible when this function returns.

**/
void MemoryFence (void) {
    // This is a little bit of overkill and it is more about the compiler that it is
    // actually processor synchronization. This is like the _ReadWriteBarrier
    // Microsoft specific intrinsic
    __asm__ __volatile__ ("":::"memory");
}

unsigned short MmioWrite16 (unsigned int Address, unsigned short  Value)
{
    unsigned int        Flag = 0;

    Flag = FilterBeforeMmIoWrite (FilterWidth16, Address, &Value);

    if (Flag) {

        MemoryFence ();

        *(volatile unsigned short *)Address = Value;

        MemoryFence ();
    }

    FilterAfterMmIoWrite (FilterWidth16, Address, &Value);

    return Value;
}

unsigned int MmioRead32 (unsigned int Address) {
    unsigned int Value;
    unsigned int Flag = 0;

    Flag = FilterBeforeMmIoRead (FilterWidth16, Address, &Value);

    if (Flag) {
        MemoryFence ();

        Value = *(volatile unsigned int *)Address;

        MemoryFence ();
    }

    FilterAfterMmIoRead (FilterWidth16, Address, &Value);

    return Value;
}

unsigned int is_aspeed_present(void){
    unsigned int PresenceStatus = 0;

    //
    //ASPEED AST2500/AST2600
    //
    outb(ASPEED2500_SIO_UNLOCK, ASPEED2500_SIO_INDEX_PORT);
    outb(ASPEED2500_SIO_UNLOCK, ASPEED2500_SIO_INDEX_PORT);
    outb(REG_LOGICAL_DEVICE, ASPEED2500_SIO_INDEX_PORT);
    outb(ASPEED2500_SIO_UART1, ASPEED2500_SIO_DATA_PORT);

    if (inb(ASPEED2500_SIO_DATA_PORT) == ASPEED2500_SIO_UART1) {
        //
        //There is a Aspeed card need to support as well. it's type is AST2500 A1 EVB.
        //
        //AST2300-A0 0x01000003
        //AST2300-A1 0x01010303
        //AST1300-A1 0x01010003
        //AST1050-A1 0x01010203
        //AST2400-A0 0x02000303
        //AST2400-A1 0x02010303
        //AST1400-A1 0x02010103
        //AST1250-A1 0x02010303
        //AST2500-A0 0x04000303
        //AST2510-A0 0x04000103
        //AST2520-A0 0x04000203
        //AST2530-A0 0x04000403
        //AST2500-A1 0x04010303
        //AST2510-A1 0x04010103
        //AST2520-A1 0x04010203
        //AST2530-A1 0x04010403

        //if ((ReadAHBDword (SCU7C) & 0xff0000ff) == 0x04000003) {
        //PresenceStatus = 1;
        //}

        PresenceStatus = 1;
    }

    outb(ASPEED2500_SIO_LOCK, ASPEED2500_SIO_INDEX_PORT);
    return PresenceStatus;
}

unsigned int is_sio_exist (void) {
    unsigned int   SioExist = 0;

    if (is_aspeed_present ()) {
        SioExist |= ASPEED_EXIST;
    }

//    if (IsNuvotonPresent ()) {
//      SioExist |= NCT5104D_EXIST;
//    }

    return SioExist;
}

static void sio_init (void) {
    unsigned int  SioExist;
    unsigned int  SioEnable;
    unsigned int  Index;
    unsigned int  Decode;
    unsigned int  Enable;
    unsigned int  SpiConfigValue;

    //
    // Enable LPC decode
    // Set COMA/COMB base
    //
    Decode =  ((V_LPC_CFG_IOD_COMA_3F8 << N_LPC_CFG_IOD_COMA) | (V_LPC_CFG_IOD_COMB_2F8 << N_LPC_CFG_IOD_COMB));
    SpiConfigValue = MmioRead32 (PCH_PCR_ADDRESS (PID_ESPISPI, R_PCH_PCR_SPI_CONF_VALUE));
    if (SpiConfigValue & B_ESPI_ENABLE_STRAP) {
        Enable =  ( B_LPC_CFG_IOE_ME2 | B_LPC_CFG_IOE_SE | B_LPC_CFG_IOE_ME1 \
        | B_LPC_CFG_IOE_CBE | B_LPC_CFG_IOE_CAE);
    } else {
        Enable =  ( B_LPC_CFG_IOE_ME2 | B_LPC_CFG_IOE_SE | B_LPC_CFG_IOE_ME1 \
        | B_LPC_CFG_IOE_KE | B_LPC_CFG_IOE_CBE | B_LPC_CFG_IOE_CAE);
    }
    outl((unsigned int) (PCH_LPC_CF8_ADDR (R_LPC_CFG_IOD)), R_PCH_IOPORT_PCI_INDEX);

    outl(Decode | (Enable << 16), R_PCH_IOPORT_PCI_DATA);

    MmioWrite16 (PCH_PCR_ADDRESS(PID_DMI, R_PCH_DMI_PCR_LPCIOD), (unsigned short)Decode);
    MmioWrite16 (PCH_PCR_ADDRESS(PID_DMI, R_PCH_DMI_PCR_LPCIOE), (unsigned short)Enable);

    SioExist = is_sio_exist();

    if ((SioExist & (PILOTIV_EXIST | PC8374_EXIST)) == (PILOTIV_EXIST | PC8374_EXIST) ) {
        //
        // Both are there, we use DEFAULT_SIO as debug port anyway
        //
        if (DEFAULT_SIO == PILOTIV_EXIST) {
            SioEnable = PILOTIV_EXIST;
        } else {
            SioEnable = PC8374_EXIST;
        }
    } else {
        SioEnable = SioExist;
    }

    //
    // ASPEED AST2500/AST2600 UART init.
    //
    if (SioEnable == ASPEED_EXIST) {
        //
        // Unlock SIO
        //
        outb (ASPEED2500_SIO_UNLOCK, ASPEED2500_SIO_INDEX_PORT);
        outb (ASPEED2500_SIO_UNLOCK, ASPEED2500_SIO_INDEX_PORT);

        //
        // COM1 & COM2
        //
        for (Index = 0; Index < sizeof (mASPEED2500Table)/sizeof (SIO_REG_TABLE); Index++) {
            outb (mASPEED2500Table[Index].Index, ASPEED2500_SIO_INDEX_PORT);
            outb (mASPEED2500Table[Index].Data, ASPEED2500_SIO_DATA_PORT);
        }

        //
        // Lock SIO
        //
        outb (ASPEED2500_SIO_LOCK, ASPEED2500_SIO_INDEX_PORT);
    }
}

/**

  Initialize Serial Port

    The Baud Rate Divisor registers are programmed and the LCR
    is used to configure the communications format. Hard coded
    UART config comes from globals in DebugSerialPlatform lib.

  @param None

  @retval None

**/
static unsigned int uart16550_init (void) {
    unsigned int        Divisor;
    unsigned char       OutputData;
    unsigned char       Data;
    unsigned short      ComBase;

    unsigned char  gData     = 8;
    unsigned char  gStop     = 1;
    unsigned char  gParity   = 0;
    unsigned char  gBreakSet = 0;

    ComBase = 0x3F8;
    //
    // Some init is done by the platform status code initialization.
    //
    //
    // Map 5..8 to 0..3
    //
    Data = (unsigned char) (gData - (unsigned char) 5);

    //
    // Calculate divisor for baud generator
    //
    //Divisor = 115200 / PcdGet32(PcdSerialBaudRate);
    Divisor = 115200 / 115200;

    //
    // Set communications format
    //
    OutputData = (unsigned char) ((DLAB << 7) | ((gBreakSet << 6) | ((gParity << 3) | ((gStop << 2) | Data))));
    outb (OutputData, ComBase + LCR_OFFSET);

    //
    // Configure baud rate
    //
    outb ((unsigned char) (Divisor >> 8), ComBase + BAUD_HIGH_OFFSET);
    outb ((unsigned char) (Divisor & 0xff), ComBase + BAUD_LOW_OFFSET);

    //
    // Switch back to bank 0
    //
    OutputData = (unsigned char) ((~DLAB << 7) | ((gBreakSet << 6) | ((gParity << 3) | ((gStop << 2) | Data))));
    outb (OutputData, ComBase + LCR_OFFSET);

    return 1;
}

void sio_uart_init(void) {

    sio_init();

    uart16550_init();
}
