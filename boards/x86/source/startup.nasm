bits 16

global _reset_vector

extern setup_car
extern setup_protected_mode
extern microcode_update
extern setup_idt
extern isr_0_test
extern isr_11_test
extern c_entry

section .text.resetvector

_reset_vector:
    mov al, 0xAA
    out 0x80, al

    jmp _sec_entry
    TIMES(0x10 - ($ - $$)) nop

section .text.secphase

_sec_entry:
    mov al, 0x55
    out 0x80, al

    mov ebp, setup_protected_mode_return
    jmp setup_protected_mode

bits 32

setup_protected_mode_return:
    mov al, 0x56
    out 0x80, al

    mov edi, bin_table
    mov ebx, [edi + 0x0]	; Read address of FSP-T
    mov ecx, [edi + 0x4]	; Read complement to address of FSP-T
    and ecx, ebx
    jnz without_fsp_t

    mov edi, ebx
OFFSET 			equ 0x78
OFFSET_1		equ 0xB0
OFFSET_2		equ 0xC4

FSP_HEADER_GUID_DWORD1	equ 0x912740BE
FSP_HEADER_GUID_DWORD2	equ 0x47342284
FSP_HEADER_GUID_DWORD3	equ 0xB08471B9
FSP_HEADER_GUID_DWORD4	equ 0x0C3F3527

fsp_check_ffs_header:
    ; Check the ffs guid
    mov  eax, dword [edi + OFFSET]
    cmp  eax, FSP_HEADER_GUID_DWORD1
    jnz  without_fsp_t

    mov  eax, dword [edi + OFFSET + 4]
    cmp  eax, FSP_HEADER_GUID_DWORD2
    jnz  without_fsp_t

    mov  eax, dword [edi + OFFSET + 8]
    cmp  eax, FSP_HEADER_GUID_DWORD3
    jnz  without_fsp_t

    mov  eax, dword [edi + OFFSET + 0Ch]
    cmp  eax, FSP_HEADER_GUID_DWORD4
    jnz  without_fsp_t

    mov eax, dword [edi + OFFSET_1]
    add eax, dword [edi + OFFSET_2]

    mov esp, fspt_stack
call_fspt:
    jmp eax

without_fsp_t:
    mov edi, bin_table
    mov eax, [edi + 0x8]	; MICRO_UPD_ADDR
    mov ebx, [edi + 0xC]	; MICRO_UPD_ADDR_COMPLEMENT
    and ebx, eax
    jnz microcode_update_return
    mov ebp, microcode_update_return
    jmp microcode_update
microcode_update_return:

    mov ebp, setup_car_return
    jmp setup_car

fspt_return:

    mov ebx, eax
    mov al, 0xEE
    out 0x80, al
    mov al, dl
    out 0x80, al

    cmp ebx, 0x8000000E
    jz setup_car_return

    cmp ebx, 0x0
    jz setup_car_return

    jmp $

setup_car_return:
    ; ecx - base address
    ; edx - end address
    mov esp, edx	; Region end address
    add esp, 0x3
    and esp, 0x0FFFFFFFC		; Add region size

    mov al, 0x57
    out 0x80, al

; Filling stack for debugging purpose
; It's needed to check ability to access memory
check_stack:
    mov ebp, esp ; ebp stores previous esp value
    mov ebx, ecx ; ebx stores stack base

    mov edx, esp ;
    sub edx, ecx ; get size of stack in bytes
    mov ecx, edx ;
    shr ecx, 0x2 ; get size of stack in double words
    mov edx, ecx
.1:
    push esp
    loop .1

    mov ecx, edx
.2:
    pop eax
    cmp eax, esp
    jne check_stack_error
    loop .2
    mov ecx, ebx ; restore stack base

over_pass:
    call setup_idt
;    call isr_0_test
;    call isr_11_test

    add edx, 0x3
    and edx, 0x0FFFFFFFC
    sub edx, ecx
    push edx
    push ecx
    call c_entry

    jmp $

check_stack_error:
    mov al, 0xFF
    out 0x80, al
    mov al, 0xE1
    out 0x80, al
    jmp check_stack_error

section .data

align 10h

; Hardcoded call stack for fsp-t
fspt_stack:
    DD	 fspt_return			; fsp-t return address
;    DD	fspt10_temp_ram_init_params	; fsp-t 1.0 parameters
    DD fspt21_upd			; fsp-t 2.1 parameters
;    DD	fspt23_upd			; fsp-t 2.3 parameters
;    DD	fspt24_upd			; fsp-t 2.4 parameters


fsp10_temp_ram_init_params:
    DD	0xFFF10000		; MicrocodeRegionBase
    DD	0x00080000		; MicrocodeRegionLength
    DD	0xFFC00000		; CodeRegionBase
    DD	0x00400000		; CodeRegionLength

; Total size of FSPT_UPD structure for revision 2.x is FSP_INFO_HEADER.CfgRegionSize
; For details see Intel® Firmware Support Package External Architecture Specification v2.x

fspt21_upd:
;fsp_upd_header:
    DQ "SOCUPD_T"		; Signature
    DB 0x00			; Revision
    TIMES(23)	DB 0x00		; Reserved[23]
;fspt_core_upd:
    DD	0xFFF10000		; Microcode update base
    DD	0x00046400		; Microcode update length
    DD	0xFFC00000		; Code region base
    DD	0x00400000		; Code region size
    TIMES(16)	DB	0x00	; Reserved[16]
;fspt_config:
    DB	0x01			; FsptPort80RouteDisable
    TIMES(31)	DB	0x00	; Reserved[31]
    TIMES(6)	DB	0x00	; Unused space[6]
    DW	0x0000			; Terminator

fspt23_upd:
;fsp_upd_header:
    DQ "SPRUPD_T"		; Signature
    DB 0x01			; Revision
    TIMES(23)	DB 0x00		; Reserved[23]
;fspt_core_upd:
    DD	0xFFF10000		; Microcode update base
    DD	0x00080000		; Microcode update length
    DD	0xFFC00000		; Code region base
    DD	0x00400000		; Code region size
    TIMES(16)	DB	0x00	; Reserved[16]
;fspt_config:
    DB	0x00			; FsptPort80RouteDisable
    TIMES(31)	DB	0x00	; Reserved[31]
    TIMES(6) DB	0x00		; Unused space[6]
    DW	0xAA55			; Terminator

fspt24_upd:
;fsp_upd_header:
    DQ "SPRUPD_T"		; Signature
    DB 0x02			; Revision
    TIMES(23)	DB 0x00		; Reserved[23]
;fspt_arch_upd:
    DB	0x02			; Revision
    DB	0xFF, 0xFF, 0xFF	; Reserver[3]
    DD	0x00000020		; Length of fspt_arch_upd2
    DD	0x00000000		; Debug handler
    TIMES(20) DB 0xFF		; Reserved[20]
;fspt_core_upd:
    DD	0xFFF10000		; Microcode update base
    DD	0x00080000		; Microcode update length
    DD	0xFFC00000		; Code region base
    DD	0x00400000		; Code region size
    TIMES(16)	DB	0x00	; Reserved[16]
;fspt_config:
    DB	0x00			; FsptPort80RouteDisable
    TIMES(31)	DB	0x00	; Reserved[31]
    DW	0xAA55			; Terminator

section .data.bin.table

bin_table:
    DD 0xFFF00000	; FSP_T_ADDR
    DD 0x000FFFFF	; FSP_T_ADDR_COMPLEMENT

    DD 0xFFF10000	; MICRO_UPD_ADDR
    DD 0x000EFFFF	; MICRO_UPD_ADDR_COMPLEMENT

    DD 0xA5A5A5A5	; RESERVED
    DD 0xBABADEDA	; RESERVED