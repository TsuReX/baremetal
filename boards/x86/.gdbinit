target remote localhost:1234

symbol-file x86.elf

set architecture i386

layout asm

layout regs

focus cmd

set disassembly-flavor intel

b *&call_fspt

info  break