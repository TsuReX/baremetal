target remote localhost:1234
layout asm
layout regs
focus cmd
symbol-file imx8m.elf
set architecture aarch64
b _start
b clear_cache
info reg pc
list
