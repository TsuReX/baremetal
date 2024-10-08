target remote localhost:1234

symbol-file x86.elf

set architecture i386

layout asm

layout regs

focus cmd

set disassembly-flavor intel

set pagination off

b *&call_fspt

b *&fspt_return

# check_fspt_upd_revision
b *0x10612

# check_fspt_upd_revision_failure
b *0x10366

# end_check_fspt_upd_revision
b *0x10617

# revision_above_2.2
b *0x1032A

# revision_equal_2.2
b *0x10349

# revision_below_2.2
b *0x1030B

# microcode_update
b *0x10370

# return_from_check_fspt_upd_revision_and_microcode_update
b *0x10312

# jump_to_setup
b *0x106AC

# main_func
b *0x106C8

# action_1
b *0x10940

# action_2
b *0x108A8

# action_3
b *0x10804

# action_4
b *0x10F78

# action_5
b *0x10967

# action_6
b *0x121C8

# action_7
b *0x108D2

# action_8
b *0x112AB

# check_processor_smth
b *0x109B0

# setup_car
b *0x10ABC

# fill_cache
b *0x10E2F

# find_valid_mtrr
b *0x109FF

# call func_1
b *0x107B9

# call func_2
b *0x107BE

# call func_3
b *0x107C3

# call func_4
b *0x107CC

# call func_5
b *0x107D3

# call func_6
b *0x107D8

# call func_7
b *0x107DD

# call func_8
b *0x107E2

# call func_9
b *0x107E7


set logging on

info  break

# Base of CAR data region
x/1x 0x00012F3C
set *0x00012F3C = 0x00100000
x/1x 0x00012F3C

# Size of CAR data region in bytes
x/1x 0x00012F30
set *0x00012F30 = 0x00010000
x/1x 0x00012F30

b *0x10A89
    command
    silent
    printf "ecx edx:eax  0x%04X 0x%08X:0x%08X\n",$ecx, $edx,$eax
    cont
    end

b *0x10A99
    command
    silent
    printf "ecx edx:eax  0x%04X 0x%08X:0x%08X\n",$ecx, $edx,$eax
    cont
    end

