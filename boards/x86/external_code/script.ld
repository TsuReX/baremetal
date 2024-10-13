__OFFSET__ = 0x4000;
PROVIDE(__TEXT_BEGIN__ = 0xFFF10000 + __OFFSET__);
PROVIDE(__TEXT_SIZE__ = 0x00000100);
PROVIDE(__TEXT_END__ = __TEXT_BEGIN__ + __TEXT_SIZE__ - 1);

SECTIONS
{

    func_4 = __TEXT_BEGIN__ + 0x193A - __OFFSET__;
    func_5 = __TEXT_BEGIN__ + 0x1706 - __OFFSET__;
    func_6 = __TEXT_BEGIN__ + 0x17EA - __OFFSET__;
    func_7 = __TEXT_BEGIN__ + 0x20C8 - __OFFSET__;
    func_8 = __TEXT_BEGIN__ + 0x2228 - __OFFSET__;
    func_9 = __TEXT_BEGIN__ + 0x17D4 - __OFFSET__;
    return_addr = __TEXT_BEGIN__ + 0x07D1 - __OFFSET__;

    . = __TEXT_BEGIN__;
    .text.external : {*(.text.external)}
    . = __TEXT_END__;

    /DISCARD/ :
    {
    *(.rel.reloc)
    *(.eh_frame)
    *(.note*)
    *(.comment*)
    }
}