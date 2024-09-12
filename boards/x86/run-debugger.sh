#!/bin/sh

i386-unknown-elf-gdb --command=../boards/x86/.gdbinit
killall qemu-system-i386
killall qemu-system-i386
killall qemu-system-i386