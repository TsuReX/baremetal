bits 16

global setup_protected_mode
extern setup_protected_mode_return

section .text.secphase

setup_protected_mode:
    finit
    cli

    mov esi, gdt
    DB 0x66
    lgdt [cs:si]

    mov     eax, cr0                   ; Get control register 0
    or      eax, 00000001h             ; Set PE bit (bit #0) & MP bit (bit #1)
    mov     cr0, eax                   ; Activate protected mode

    mov     eax, 0x10
    mov     ds, ax
    mov     es, ax
    mov     fs, ax
    mov     gs, ax
    mov     ss, ax

    mov esi, protected_mode
    jmp dword far [cs:si]

section .data
align 0x10
protected_mode:
    DD setup_protected_mode_return
    DW 0x00000008

gdt:
    DW gdt_len - 1
    DD gdt_entry_zero

gdt_entry_zero:
    dd 0x0
    dd 0x0

gdt_entry_code:

%define cs_limit	0x000FFFFF
%define cs_base		0x00000000
%define cs_access	0x9B
%define cs_flag		(0xC & 0xF)
    dw (cs_limit)
    dw (cs_base & 0xFFFF)
    db ((cs_base >> 16) & 0xFF)
    db (cs_access)
    db ((cs_flag << 4) | ((cs_limit >> 16) & 0xF))
    db ((cs_base >> 24) & 0xFF)

gdt_entry_data:

%define ds_limit	0x000FFFFF
%define ds_base		0x00000000
%define ds_access	0x92
%define ds_flag		(0xC & 0xF)
    dw (ds_limit)
    dw (ds_base & 0xFFFF)
    db ((ds_base >> 16) & 0xFF)
    db (ds_access)
    db ((ds_flag << 4) | ((ds_limit >> 16) & 0xF))
    db ((ds_base >> 24) & 0xFF)

gdt_len equ ($ - gdt_entry_zero)
