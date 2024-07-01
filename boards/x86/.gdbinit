target remote localhost:1234
layout asm
layout regs
focus cmd
symbol-file x86.elf
set architecture i386
b _start
info reg pc
list
