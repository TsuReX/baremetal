#!/bin/sh

# qemu-system-aarch64 -machine type=virt,virtualization=on,secure=on -cpu cortex-a53 -smp cpus=1 -s -S -bios ./imx8m.bin -nographic &

# This command doesn't set PC to specified address. I don't know why
# qemu-system-aarch64 -machine type=virt,virtualization=on,secure=on -cpu cortex-a53 -smp cpus=2 -s -S -device loader,file=./imx8m.bin,addr=0x100 -device loader,addr=0x100,cpu-num=0 -nographic &

qemu-system-aarch64 -machine type=raspi3b -s -S -device loader,file=./imx8m.bin,addr=0x100 -device loader,addr=0x100,cpu-num=0 -nographic &

echo PID: $!