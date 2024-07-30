ENTRY(_reset_vector)

PROVIDE(__TEXT_BEGIN__ = 0xFFFFF000);
PROVIDE(__TEXT_END__ = __TEXT_BEGIN__ + 0xE00 - 1);

PROVIDE(__DATA_BEGIN__ = __TEXT_BEGIN__ + 0xE00);
PROVIDE(__DATA_END__ = __TEXT_BEGIN__ + 0x1000 - 0x10 - 1);

__TEXT_RESET_BEGIN__ = __DATA_END__ + 1;

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