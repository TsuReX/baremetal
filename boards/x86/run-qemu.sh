#!/bin/sh

#qemu-system-i386 -m 256M -machine q35 -nographic -serial mon:stdio -pflash x86.bin -s -S

#qemu-system-i386 -m 256M -cpu Icelake-Server-v1 -nographic -serial mon:stdio -device loader,file=fspt.bin,addr=0x08000000 -device loader,file=x86.bin,addr=0xFFF00000 -s -S

#qemu-system-i386 -m 256M -cpu Icelake-Server-v1 -nographic -serial mon:stdio -device loader,file=x86.bin,addr=0xFFF00000 -s -S

#qemu-system-i386 -m 256M -machine q35 -nographic -serial mon:stdio -device loader,file=x86.bin,addr=0xFFFFFE00 -s -S

#Memory map (BE SURE that regions desribed in script.ld match the memory map placed here).
#0x0000.0000 256MB (0x1000.0000)
#	RAM
#
#	0x0008.8000 32KB (0x8000)
#		FSP-T
#0x1000.0000 4GB (0x1.0000.0000) - 256MB (0x1000.0000) - 256KB (0x4.0000)
#	NOT AVAILABLE
#0xFFFC.0000 256KB (0x4.0000)
#	BIOS REGION
#
#	0xFFFC.0000 4B (0x4)
#		FSP_T_ADDR_STORAGE
#	0xFFFC.0004 4B (0x4)
#		FSP_T_ADDR_STORAGE_COMPLEMENT
#	0xFFFF.E000 7680B (0x1E00)
#		BIOS TEXT
#	0xFFFF.FE00 496B (0x1F0)
#		BIOS DATA
#	0xFFFF.FFF0 16B (0x10)
#		POWER UP CODE
#0x1.0000.0000 END OF 4GB MEMORY SPACE

# This value contains address where FSP-T is placed
FSP_T_ADDR=0x00088000
printf "0x%08X\n" $FSP_T_ADDR
FSP_T_ADDR_COMPLEMENT=$(( ((1 << 32) - 1) & (~FSP_T_ADDR) ))
printf "0x%08X\n" $FSP_T_ADDR_COMPLEMENT

# This value contains path and name of being loaded FSP-T file
FSP_T_FILE=$1
# This value contains address of FSP-T address
FSP_T_ADDR_STORAGE=0x00080000
# This value contains path and name of being loaded BIOS file
BIOS_FILE=x86.bin

qemu-system-i386 \
-m 256M -cpu Icelake-Server-v1 -nographic -serial mon:stdio -s -S \
-device loader,file=$FSP_T_FILE,addr=$FSP_T_ADDR \
-device loader,file=$BIOS_FILE,addr=0xFFF00000 \
-device loader,addr=$FSP_T_ADDR_STORAGE,data=$FSP_T_ADDR,data-len=4 \
-device loader,addr=$((FSP_T_ADDR_STORAGE + 4)),data=$FSP_T_ADDR_COMPLEMENT,data-len=4

echo PID: $!