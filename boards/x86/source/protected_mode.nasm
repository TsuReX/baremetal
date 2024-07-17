bits 16

global setup_protected_mode
extern setup_protected_mode_return

section .text.secphase

setup_protected_mode:

jmp setup_protected_mode_return

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
