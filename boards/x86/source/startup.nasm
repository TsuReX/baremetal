bits 16

global _reset_vector
extern setup_car
global setup_car_return

section .text.resetvector
_reset_vector:
    jmp _sec_entry
    TIMES(0x10 - ($ - $$)) nop

section .text.secphase
_sec_entry:
    mov ax, 0x0
    cpuid

    mov ax, 0x1
    cpuid

    mov eax, edx	;The CPUID instruction should be used to determine
    and eax, (0x1 << 5)	;whether MSRs are supported (CPUID.01H:EDX[5] = 1) before using this instruction.

    mov eax, edx	;The processor supports the Memory Type Range Registers specifically
    and eax, (0x1 << 12);the MTRR_CAP register.

    mov ax, 0x2		;Intel Processor Identification and the CPUID instruction.
    cpuid		;Paragraph 5.1.3 Cache Descriptors (Function 02h)

    mov ax, 0x3
    cpuid

    mov cx, 0xFE	;IA32_MTRR_CAP
    rdmsr

    mov cx, 0x2FF	;IA32_MTRR_DEF_TYPE
    rdmsr


    jmp $

    jmp setup_car
setup_car_return:
    jmp $

section .data
gdt_entry_zero:
    dd 0x0
    dd 0x0

gdt_entry_code:

%define cs_limit	0x0000FFFF
%define cs_base		0x000F0000
%define cs_access	0x9F
%define cs_flag		(0x0 & 0xF)
    dw (cs_limit)
    dw (cs_base & 0xFFFF)
    db ((cs_base >> 16) & 0xFF)
    db (cs_access)
    db (((cs_limit >> 16) & 0xF) | cs_flag)
    db ((cs_base >> 24) & 0xFF)

gdt_entry_data:

%define ds_limit	0x0000FFFF
%define ds_base		0x000F0000
%define ds_access	0x93
%define ds_flag		(0x0 & 0xF)
    dw (ds_limit)
    dw (ds_base & 0xFFFF)
    db ((ds_base >> 16) & 0xFF)
    db (ds_access)
    db (((ds_limit >> 16) & 0xF) | cs_flag)
    db ((ds_base >> 24) & 0xFF)

gdt_len equ ($ - gdt_entry_zero)
