#!/bin/bash

to_little_ending() {
    V=`printf "%08x" $1`
    V_LE=0x`echo ${V:6:2}${V:4:2}${V:2:2}${V:0:2}`
    echo $V_LE
}

region_prepare() {

    # This value contains path and name of being loaded BIOS file
    printf "BIOS_FILE: %s\n" $BIOS_FILE
    BIOS_SIZE=`stat --format="%s" $BIOS_FILE`
    printf "BIOS_SIZE: 0x%08X\n" $BIOS_SIZE
    printf "\n"

    BIN_TABLE_END_OFFSET=0x40
    FSP_T_ADDR_OFFSET=0x0
    FSP_T_ADDR_COMPLEMENT_OFFSET=0x4
    MICRO_UPD_ADDR_OFFSET=0x8
    MICRO_UPD_ADDR_COMPLEMENT_OFFSET=0xC

    if [ ${FSP_T_ADDR} ]; then

	printf "FSP_T_ADDR: 0x%08X\n" $FSP_T_ADDR
	FSP_T_ADDR_COMPLEMENT=$(( ((1 << 32) - 1) & (~FSP_T_ADDR) ))
	printf "FSP_T_ADDR_COMPLEMENT: 0x%08X\n" $FSP_T_ADDR_COMPLEMENT
	printf "\n"
	

	
	ADDR=$(($BIOS_SIZE - $BIN_TABLE_END_OFFSET + $FSP_T_ADDR_OFFSET))
	TUPLE=`printf "\"%08x: %08x\"" $ADDR $(to_little_ending $FSP_T_ADDR)`
	echo $TUPLE | xxd -r - $BIOS_FILE
	
	ADDR=$(($BIOS_SIZE - $BIN_TABLE_END_OFFSET + $FSP_T_ADDR_COMPLEMENT_OFFSET))
	TUPLE=`printf "\"%08x: %08x\"" $ADDR $(to_little_ending $FSP_T_ADDR_COMPLEMENT)`
	echo $TUPLE | xxd -r - $BIOS_FILE
    
    fi

    if [ ${MICRO_UPD_ADDR} ]; then

	printf "MICRO_UPD_ADDR: 0x%08X\n" $MICRO_UPD_ADDR
	MICRO_UPD_ADDR_COMPLEMENT=$(( ((1 << 32) - 1) & (~MICRO_UPD_ADDR) ))
	printf "MICRO_UPD_ADDR_COMPLEMENT: 0x%08X\n" $MICRO_UPD_ADDR_COMPLEMENT
	printf "\n"

	ADDR=$(($BIOS_SIZE - $BIN_TABLE_END_OFFSET + $MICRO_UPD_ADDR_OFFSET))
	TUPLE=`printf "\"%08x: %08x\"" $ADDR $(to_little_ending $MICRO_UPD_ADDR)`
	echo $TUPLE | xxd -r - $BIOS_FILE
	
	ADDR=$(($BIOS_SIZE - $BIN_TABLE_END_OFFSET + $MICRO_UPD_ADDR_COMPLEMENT_OFFSET))
	TUPLE=`printf "\"%08x: %08x\"" $ADDR $(to_little_ending $MICRO_UPD_ADDR_COMPLEMENT)`
	echo $TUPLE | xxd -r - $BIOS_FILE

    fi

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

	"--mcupd-addr")
	    MICRO_UPD_ADDR=${2}
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

echo

region_prepare

echo

xxd -g4  -c4 -s -0x40 -l 0x18 $BIOS_FILE

#../boards/x86/bios-region-prepare.sh --bios ./x86.bin --fspt-addr 0x00010000 --mcupd-addr 0x00020000
