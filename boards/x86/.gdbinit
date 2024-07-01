target remote localhost:1234
symbol-file x86.elf
set architecture i386
b _reset_vector
b _sec_entry
info reg eip

layout asm
layout regs
focus cmd
