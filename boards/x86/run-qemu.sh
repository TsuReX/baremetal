#!/bin/bash

# TODO: Check presence of $1 argument and file existance and execute qemu with appropriate parameters

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
FSP_T_ADDR=0x00010000
printf "FSP_T_ADDR: 0x%08X\n" $FSP_T_ADDR
FSP_T_ADDR_COMPLEMENT=$(( ((1 << 32) - 1) & (~FSP_T_ADDR) ))
printf "FSP_T_ADDR_COMPLEMENT: 0x%08X\n" $FSP_T_ADDR_COMPLEMENT

# This value contains address where Microcode Update is placed
MICRO_UPD_ADDR=0x00020000
printf "MICRO_UPD_ADDR: 0x%08X\n" $MICRO_UPD_ADDR
MICRO_UPD_ADDR_COMPLEMENT=$(( ((1 << 32) - 1) & (~MICRO_UPD_ADDR) ))
printf "MICRO_UPD_ADDR_COMPLEMENT: 0x%08X\n" $MICRO_UPD_ADDR_COMPLEMENT

# This value contains path and name of being loaded FSP-T file
FSP_T_FILE=$1

# This value contains path and name of being loaded Microcode Update file
MICRO_UPD_FILE=$2

# This value contains address of FSP-T address
BIN_TABLE=$(((1 << 32) - 0x40))
printf "BIN_TABLE: 0x%08X\n" $BIN_TABLE

# This value contains path and name of being loaded BIOS file
BIOS_FILE=x86.bin

BIOS_SIZE=`stat --format="%s" $BIOS_FILE`
printf "BIOS_SIZE: 0x%08X\n" $BIOS_SIZE

BIOS_ADDR=$(( (1<<32) - BIOS_SIZE ))
printf "BIOS_ADDR: 0x%08X\n" $BIOS_ADDR

to_little_ending() {
    V=`printf "%08x" $1`
    V_LE=0x`echo ${V:6:2}${V:4:2}${V:2:2}${V:0:2}`
    echo $V_LE
}

ADDR=$((0x3000 - 0x40 + 0x0)); VAR=`printf "\"%08x: %08x\"" $ADDR $(to_little_ending $FSP_T_ADDR)`; echo $VAR | xxd -r - $BIOS_FILE
ADDR=$((0x3000 - 0x40 + 0x4)); VAR=`printf "\"%08x: %08x\"" $ADDR $(to_little_ending $FSP_T_ADDR_COMPLEMENT)`; echo $VAR | xxd -r - $BIOS_FILE

ADDR=$((0x3000 - 0x40 + 0x8)); VAR=`printf "\"%08x: %08x\"" $ADDR $(to_little_ending $MICRO_UPD_ADDR)`; echo $VAR | xxd -r - $BIOS_FILE
ADDR=$((0x3000 - 0x40 + 0xC)); VAR=`printf "\"%08x: %08x\"" $ADDR $(to_little_ending $MICRO_UPD_ADDR_COMPLEMENT)`; echo $VAR | xxd -r - $BIOS_FILE

qemu-system-i386 \
-m 256M -cpu Icelake-Server-v1 -nographic -serial mon:stdio -s -S \
-device loader,file=$FSP_T_FILE,addr=$FSP_T_ADDR \
-device loader,file=$MICRO_UPD_FILE,addr=$MICRO_UPD_ADDR \
-device loader,file=$BIOS_FILE,addr=$BIOS_ADDR

#-device loader,addr=$FSP_T_ADDR_STORAGE,data=$FSP_T_ADDR,data-len=4
#-device loader,addr=$((FSP_T_ADDR_STORAGE + 4)),data=$FSP_T_ADDR_COMPLEMENT,data-len=4

echo PID: $!