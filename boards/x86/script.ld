ENTRY(_reset_vector)

PROVIDE(__MICROCODE_UPDATE_BEGIN__ = 0xFFFFE000);
PROVIDE(__MICROCODE_UPDATE_END__ = 0xFFFE0000 - 1);
MIC_UPD = __MICROCODE_UPDATE_BEGIN__;

PROVIDE(__TEXT_BEGIN__ = 0xFFFFF000);
PROVIDE(__TEXT_SIZE__ = 0x00000E00); # 3584 bytes
PROVIDE(__TEXT_END__ = __TEXT_BEGIN__ + __TEXT_SIZE__ - 1);

PROVIDE(__DATA_BEGIN__ = __TEXT_BEGIN__ + __TEXT_SIZE__);
PROVIDE(__DATA_SIZE__ = 0x00000200 - 0x10); # 502 bytes
PROVIDE(__DATA_END__ = __DATA_BEGIN__ + __DATA_SIZE__ - 1);

PROVIDE(__TEXT_RESET_BEGIN__ = 0xFFFFFFF0); # 4GB - 16 bytes

#INPUT(microcode_update.bin);

SECTIONS
{
	. = __MICROCODE_UPDATE_BEGIN__;
	.microcode_update : {microcode_update.out(.data)}
	. = __MICROCODE_UPDATE_END__;

	. = __TEXT_BEGIN__;
	.text.secphase : {*(.text.secphase)}
	.text : {*(.text)}
	. = __TEXT_END__;

	. = __DATA_BEGIN__;
	.data : {*(.data)}
	.rodata : {*(.rodata)}
	. = __DATA_END__;

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