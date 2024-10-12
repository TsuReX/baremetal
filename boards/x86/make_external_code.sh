#!/bin/bash

nasm ../boards/x86/external_code/external_code.nasm -f elf -o external_code.o
i386-unknown-elf-gcc -T ../boards/x86/external_code/script.ld external_code.o -o external_code.elf -nostdlib
i386-unknown-elf-objdump -D external_code.elf > external_code.elf.objdump
i386-unknown-elf-objcopy -O binary external_code.elf external_code.bin
nm external_code.elf