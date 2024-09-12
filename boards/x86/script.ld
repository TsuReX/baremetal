ENTRY(_reset_vector)

PROVIDE(__TEXT_BEGIN__ = 0xFFFFD000);
PROVIDE(__TEXT_SIZE__ = 0x00002000); # 8KB
PROVIDE(__TEXT_END__ = __TEXT_BEGIN__ + __TEXT_SIZE__ - 1);

PROVIDE(__DATA_BEGIN__ = __TEXT_BEGIN__ + __TEXT_SIZE__);
PROVIDE(__DATA_SIZE__ = 0x00001000 - __DATA_BIN_TABLE_SIZE__ - __TEXT_RESET_SIZE__); # 4096 - 48 -16 bytes
PROVIDE(__DATA_END__ = __DATA_BEGIN__ + __DATA_SIZE__ - 1);

PROVIDE(__DATA_BIN_TABLE_BEGIN__ = __DATA_BEGIN__ + __DATA_SIZE__);
PROVIDE(__DATA_BIN_TABLE_SIZE__ = 0x00000030);
PROVIDE(__DATA_BIN_TABLE_END__ = __DATA_BIN_TABLE_BEGIN__ + __DATA_BIN_TABLE_SIZE__ - 1);

PROVIDE(__TEXT_RESET_BEGIN__ = 0x100000000 - __TEXT_RESET_SIZE__); # 4GB - 16 bytes
PROVIDE(__TEXT_RESET_SIZE__ = 0x00000010);

SECTIONS
{
    . = __TEXT_BEGIN__;
    .text.secphase : {*(.text.secphase)}
    .text : {*(.text)}
    . = __TEXT_END__;

    . = __DATA_BEGIN__;
    .data : {*(.data)}
    .rodata : {*(.rodata)}
    . = __DATA_END__;

    . = __DATA_BIN_TABLE_BEGIN__;
    .data.bin.table : {*(.data.bin.table)}
    . = __DATA_BIN_TABLE_END__;

    . = __TEXT_RESET_BEGIN__;
    .text.resetvector : {*(.text.resetvector)}

    /DISCARD/ :
    {
    *(.rel.reloc)
    *(.eh_frame)
    *(.note*)
    *(.comment*)
    }
}