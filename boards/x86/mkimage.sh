#!/bin/bash

rm -rf ../build_x86/* ;cmake .. -DBOARD_TYPE=x86 -DCMAKE_C_COMPILER=i386-unknown-elf-gcc -DUART_TYPE=LOCAL -DUART_NUM=0; cmake --build .

if [ $? -ne 0 ]; then
    echo Compilation error
    exit 1
fi

#Bochs

#../boards/x86/run-bochs.sh

FSP_PATH=".."
PLATFORM="eaglestream"

#Qemu

FSP_NAME="fsp_${PLATFORM}_rebased.fd"
MCUPD_PATH="../../microcodes/eaglestream_05.15.2023.mcb"

FSPT_ADDR=0x00010000
FSPM_ADDR=0x00040000
FSPS_ADDR=0x00080000
MCUPD_ADDR=0x00020000

python ../boards/x86/SplitFspBin.py rebase -f ${FSP_PATH}/fsp_eaglestream.fd -c s m t -b ${FSPS_ADDR} ${FSPM_ADDR} ${FSPT_ADDR} -o . -n ${FSP_NAME}

if [ $? -ne 0 ]; then
    echo FSP rebasing error
    exit 2
fi

python ../boards/x86/SplitFspBin.py info -f ${FSP_NAME}

if [ $? -ne 0 ]; then
    echo Information printing error
    exit 3
fi

python ../boards/x86/SplitFspBin.py split -f ${FSP_NAME} -o . -n ${FSP_NAME}

if [ $? -ne 0 ]; then
    echo FSP splitting error
    exit 4
fi

../boards/x86/setup-binary-table.sh --bios ./x86.bin --fspt-addr ${FSPT_ADDR} --mcupd-addr ${MCUPD_ADDR}

if [ $? -ne 0 ]; then
    echo Setting up binary table error
    exit 5
fi

../boards/x86/create-bios-section.sh --bios ./x86.bin --fspt fsp_${PLATFORM}_rebased_T.fd  --mcupd ${MCUPD_PATH} --image bios-section.bin

if [ $? -ne 0 ]; then
    echo Creating bios section error
    exit 6
fi

#../boards/x86/replace_bios_region.sh ../../intel_gen4_ifwi.bin bios-section.bin oy_gen4_ifwi.bin

#../boards/x86/run-qemu.sh --bios ./bios-section.bin --fspt ../fsp_eaglestream_rebased_T.fd --fspt-addr 0x00010000 --mcupd ../../microcodes/eaglestream_05.09.2023.mcb --mcupd-addr 0x00020000

#HW

#python ../boards/x86/SplitFspBin.py rebase -f ../fsp_eaglestream.fd -c s m t -b 0xFFB00000 0xFFC0000 0xFFF00000 -o .. -n fsp_eaglestream_rebased.fd

#python ../boards/x86/SplitFspBin.py info -f ../fsp_eaglestream_rebased.fd

#python ../boards/x86/SplitFspBin.py split -f ../fsp_eaglestream_rebased.fd -o .. -n fsp_eaglestream_rebased.fd

#../boards/x86/setup-binary-table.sh --bios ./x86.bin --fspt-addr 0xFFF00000 --mcupd-addr 0xFFF10000

#../boards/x86/create-bios-section.sh --bios ./x86.bin --fspt ../fsp_eaglestream_rebased_T.fd  --mcupd ../../microcodes/eaglestream_05.15.2023.mcb --image bios-section.bin
