ENTRY(_start)

BASE_ADDR = 0x100;

SECTIONS
{
        .text BASE_ADDR :
        {
                __text_start__ = .;
                *(.text*)
                __text_end__ = .;
        }

        .data ADDR(.text)+0x200 :
        {
                __data_load__ = LOADADDR (.data);
                __data_start__ = .;
                *(.data)
                __data_end__ = .;
        }

        .bss ADDR(.data) + 0x200 :
        {
                __bss_start__ = .;
                *(.bss)
                PROVIDE(end = .);
                __bss_end__ = .;
        }

        __stack_size__ = 0x100;
        .stack ADDR(.bss) + 0x200 :
        {
                __stack_start__ = .;
                . = ALIGN(4);
                __stack_end__ = . + __stack_size__;
        }

}
