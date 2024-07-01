#!/bin/sh

#qemu-system-i386 -m 256M -machine q35 -nographic -serial mon:stdio -pflash x86.bin -s -S

qemu-system-i386 -m 256M -machine microvm -nographic -serial mon:stdio -device loader,file=x86.bin,addr=0xFFFFFFC0 -s -S

echo PID: $!