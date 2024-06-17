target remote localhost:1234
layout asm
layout regs
layout split
focus cmd
symbol-file imx8m.elf
set architecture aarch64
b _start
info reg pc
list