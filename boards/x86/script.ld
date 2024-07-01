ENTRY(_start)

PROVIDE(__TEXT_BEGIN__ = 0x100);
PROVIDE(__TEXT_END__ = 0x200);
PROVIDE(__DATA_BEGIN__ = 0x300);
PROVIDE(__DATA_END__ = 0x400);

SECTIONS
{
	. = __TEXT_BEGIN__;
	.text : {*(.text)}
	. = __TEXT_END__;
	. = __DATA_BEGIN__;
	.data : {*(.data)}
	. = __DATA_END__;
	.bss : {*(.bss)}
    /DISCARD/ :
    {
	*(.rel.reloc)
	*(.eh_frame)
	*(.note*)
	*(.comment*)
    }
}