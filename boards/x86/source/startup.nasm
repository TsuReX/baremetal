bits 16

global _reset_vector

section .text.resetvector
_reset_vector:
    jmp _sec_entry
    TIMES(0x10 - ($ - $$)) nop

section .text.secphase
_sec_entry:
    mov eax, 0x0
    cpuid

    mov eax, 0x1
    cpuid

    mov eax, 0x2
    cpuid

    mov eax, 0x3
    cpuid

    jmp $



section .data
gdt_entry_zero:
    dd 0x0
    dd 0x0

gdt_entry_code:

%define cs_limit	0x0000FFFF
%define cs_base		0x000F0000
%define cs_access	0xFF
%define cs_limit	((0xF & 0xF) << 4)
%define cs_flag		(0xF & 0xF)
    dw (cs_limit)
    dw (cs_base & 0xFFFF)
    db ((cs_base >> 16) & 0xFF)
    db (cs_access)
    db (cs_limit | cs_flag)
    db ((cs_base >> 24) & 0xFF)

gdt_entry_data:

%define ds_limit	0x0000FFFF
%define ds_base		0x000F0000
%define ds_access	0xFF
%define ds_limit	((0xF & 0xF) << 4)
%define ds_flag		(0xF & 0xF)
    dw (ds_limit)
    dw (ds_base & 0xFFFF)
    db ((ds_base >> 16) & 0xFF)
    db (ds_access)
    db (ds_limit | cs_flag)
    db ((ds_base >> 24) & 0xFF)

gdt_len equ ($ - gdt_entry_zero)
