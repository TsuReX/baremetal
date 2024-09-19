#!/bin/bash

create_image() {

    # This value contains path and name of being loaded BIOS file
    printf "BIOS_FILE: %s\n" $BIOS_FILE
    BIOS_SIZE=`stat --format="%s" $BIOS_FILE`
    printf "BIOS_SIZE: 0x%08X\n" $BIOS_SIZE
    printf "\n"


    if [ ${FSP_T_FILE} ]; then

	if ! [ ${FSP_T_ADDR} ]; then
	    echo "FSP_T_ADDR isn't declared"
    #		exit 1
	fi
	printf "FSP_T_FILE: %s\n" $FSP_T_FILE
	printf "FSP_T_ADDR: 0x%08X\n" $FSP_T_ADDR
	printf "\n"

    else
	FSP_T_FILE="FSP_T_FILE_ZERO"
	touch FSP_T_FILE_ZERO
    fi

    if [ ${MICRO_UPD_FILE} ]; then

	if ! [ ${MICRO_UPD_ADDR} ]; then
	    echo "MICRO_UPD_ADDR isn't declared"
    #		exit 1
	fi

	printf "MICRO_UPD_FILE: %s\n" $MICRO_UPD_FILE
	printf "MICRO_UPD_ADDR: 0x%08X\n" $MICRO_UPD_ADDR
	printf "\n"

    else
	MICRO_UPD_FILE="MICRO_UPD_FILE_ZERO"
	touch MICRO_UPD_FILE_ZERO
    fi

    FSP_T_SIZE=`stat --format="%s" $FSP_T_FILE`
    MICRO_UPD_SIZE=`stat --format="%s" $MICRO_UPD_FILE`
    BIOS_SIZE=`stat --format="%s" $BIOS_FILE`

    dd if=/dev/zero of=15MB_PAD bs=1M count=15
    dd if=/dev/zero of=FSP_T_FILE_PAD bs=1M count=1
    dd if=/dev/zero of=MICRO_UPD_FILE_PAD bs=1M count=1

    truncate -s $((64 * 1024 - $FSP_T_SIZE)) FSP_T_FILE_PAD
    truncate -s $(((1024 - 64) * 1024 - $MICRO_UPD_SIZE - $BIOS_SIZE)) MICRO_UPD_FILE_PAD

    cat 15MB_PAD $FSP_T_FILE FSP_T_FILE_PAD $MICRO_UPD_FILE MICRO_UPD_FILE_PAD $BIOS_FILE > $IMAGE_FILE

    rm -f 15MB_PAD FSP_T_FILE_PAD MICRO_UPD_FILE_PAD FSP_T_FILE_ZERO MICRO_UPD_FILE_ZERO
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

	"--image")
	    IMAGE_FILE=${2}
	    shift 
	;;

	*)
	    echo "Unknown parameter passed: $1"
	    exit -1
	;;
    esac

    shift
done

create_image

#../boards/x86/create-hw-image.sh --bios ./x86.bin --fspt fsp_rebased_T.fd  --mcupd ../../eaglestream_05.15.2023.mcb

