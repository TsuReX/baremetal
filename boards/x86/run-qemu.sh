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

QEMU_ARGS="-m 256M -cpu Icelake-Server-v1 -nographic -serial mon:stdio -s -S"

run_qemu() {

    if [ ${FSP_T_FILE} ]; then

	if ! [ ${FSP_T_ADDR} ]; then
	    echo "FSP_T_ADDR isn't declared"
	    exit 1
	fi

	# This value contains path and name of being loaded FSP-T file
	#FSP_T_FILE=$1
	# This value contains address where FSP-T is placed
	#FSP_T_ADDR=0x00010000
	printf "FSP_T_FILE: %s\n" $FSP_T_FILE
	printf "FSP_T_ADDR: 0x%08X\n" $FSP_T_ADDR
	FSP_T_ADDR_COMPLEMENT=$(( ((1 << 32) - 1) & (~FSP_T_ADDR) ))
	printf "FSP_T_ADDR_COMPLEMENT: 0x%08X\n" $FSP_T_ADDR_COMPLEMENT
	printf "\n"
	QEMU_ARGS+=" -device loader,file=$FSP_T_FILE,addr=$FSP_T_ADDR "

    fi

    if [ ${MICRO_UPD_FILE} ]; then

	if ! [ ${MICRO_UPD_ADDR} ]; then
	    echo "MICRO_UPD_ADDR isn't declared"
	    exit 1
	fi

	# This value contains path and name of being loaded Microcode Update file
	#MICRO_UPD_FILE=$2
	# This value contains address where Microcode Update is placed
	#MICRO_UPD_ADDR=0x00020000
	printf "MICRO_UPD_FILE: %s\n" $MICRO_UPD_FILE
	printf "MICRO_UPD_ADDR: 0x%08X\n" $MICRO_UPD_ADDR
	MICRO_UPD_ADDR_COMPLEMENT=$(( ((1 << 32) - 1) & (~MICRO_UPD_ADDR) ))
	printf "MICRO_UPD_ADDR_COMPLEMENT: 0x%08X\n" $MICRO_UPD_ADDR_COMPLEMENT
	printf "\n"
	QEMU_ARGS+=" -device loader,file=$MICRO_UPD_FILE,addr=$MICRO_UPD_ADDR "

    fi

    # This value contains path and name of being loaded BIOS file
    #BIOS_FILE=x86.bin
    printf "BIOS_FILE: %s\n" $BIOS_FILE
    BIOS_SIZE=`stat --format="%s" $BIOS_FILE`
    printf "BIOS_SIZE: 0x%08X\n" $BIOS_SIZE
    BIOS_ADDR=$(( (1<<32) - BIOS_SIZE ))
    printf "BIOS_ADDR: 0x%08X\n" $BIOS_ADDR
    # This value contains address of FSP-T address
    BIN_TABLE=$(((1 << 32) - 0x40))
    printf "BIN_TABLE: 0x%08X\n" $BIN_TABLE
    QEMU_ARGS+=" -device loader,file=$BIOS_FILE,addr=$BIOS_ADDR "
    printf "\n"

    #echo "QEMU_ARGS: $QEMU_ARGS"

    exec qemu-system-i386 $QEMU_ARGS

    echo PID: $!
}


if [ ${#} == 0 ]; then
    echo "Invalid arguments"
    exit -1
fi

while [[ "$#" -gt 0 ]]; do
    case $1 in
	"--bios")
	    BIOS_FILE=${2}
	    shift 
	;;

	"--mcupd")
	    MICRO_UPD_FILE=${2}
	    shift 
	;;

	"--mcupd-addr")
	    MICRO_UPD_ADDR=${2}
	    shift 
	;;

	"--fspt")
	    FSP_T_FILE=${2}
	    shift 
	;;

	"--fspt-addr")
	    FSP_T_ADDR=${2}
	    shift 
	;;

	*)
	    echo "Unknown parameter passed: $1"
	    exit -1
	;;
    esac

    shift
done

run_qemu

#../boards/x86/run-qemu.sh --bios ./x86.bin --fspt ../Fsp_Rebased_T.fd --fspt-addr 0x00010000 --mcupd ../../microcodes/eaglestream_05.09.2023.mcb --mcupd-addr 0x00020000
