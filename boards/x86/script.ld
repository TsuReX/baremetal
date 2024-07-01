ENTRY(_reset_vector)

PROVIDE(__TEXT_BEGIN__ = 0xFFFFFE00);
PROVIDE(__TEXT_END__ = __TEXT_BEGIN__ + 0x180);
PROVIDE(__DATA_BEGIN__ = __TEXT_BEGIN__ + 0x180);
PROVIDE(__DATA_END__ = __TEXT_BEGIN__ + 0x200 - 0x10);
__TEXT_RESET_BEGIN__ = __DATA_END__;

SECTIONS
{
	. = __TEXT_BEGIN__;
	.text.secphase : {*(.text.secphase)}
	.text : {*(.text)}
	. = __TEXT_END__;

	. = __DATA_BEGIN__;
	.data : {*(.data)}
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