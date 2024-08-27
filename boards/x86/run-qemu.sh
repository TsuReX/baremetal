#!/bin/sh

#qemu-system-i386 -m 256M -machine q35 -nographic -serial mon:stdio -pflash x86.bin -s -S

qemu-system-i386 -m 256M -cpu Icelake-Server-v1 -nographic -serial mon:stdio -device loader,file=x86.bin,addr=0xFFF00000 -s -S
#qemu-system-i386 -m 256M -machine q35 -nographic -serial mon:stdio -device loader,file=x86.bin,addr=0xFFFFFE00 -s -S

echo PID: $!