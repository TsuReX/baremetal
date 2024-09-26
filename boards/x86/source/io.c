#include <io.h>


void d_print(unsigned int data) {
    char* data_arr = (char*)&data;

    outb(0xB0,0x80);
    outb(*(data_arr+0),0x80);

    outb(0xB1,0x80);
    outb(*(data_arr+1),0x80);

    outb(0xB2,0x80);
    outb(*(data_arr+2),0x80);

    outb(0xB3,0x80);
    outb(*(data_arr+3),0x80);
}

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