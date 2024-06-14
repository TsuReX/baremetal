#!/bin/sh

aarch64-none-elf-gdb --command=../boards/imx8m/.gdbinit; killall qemu-system-aarch64