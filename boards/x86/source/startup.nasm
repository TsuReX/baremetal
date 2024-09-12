bits 16

global _reset_vector
;global setup_protected_mode_return

extern setup_car
extern setup_protected_mode
extern microcode_update
extern setup_idt
extern isr_0_test
extern isr_11_test
extern c_entry

section .text.resetvector

_reset_vector:
    mov al, 0x80
    out 0xAA, al

    jmp _sec_entry
    TIMES(0x10 - ($ - $$)) nop

section .text.secphase

_sec_entry:
    mov al, 0x80
    out 0x55, al

    mov ebp, setup_protected_mode_return
    jmp setup_protected_mode

bits 32

setup_protected_mode_return:
    mov al, 0x80
    out 0x56, al

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

    mov esp, temp_ram_init_stack
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
setup_car_return:

temp_ram_init_done:

    mov esp, edx; Region end address
    add esp, 1; Add region size

    mov al, 0x80
    out 0x57, al

; Filling stack for debugging purpose
; It's needed to check ability to access memory
    mov ebp, esp
    mov ebx, ecx
    mov ecx, esp
    shr ecx, 2
fill_stack:
    push esp
    loop fill_stack
    mov esp, ebp
    mov ecx, ebx

    call setup_idt
;    call isr_0_test
;    call isr_11_test

    sub edx, ecx
    add edx, 1
    push edx
    push ecx
    call c_entry

    jmp $

section .data
align 10h
temp_ram_init_stack:
    DD  temp_ram_init_done	; return address
    DD  0x00000000		; fsp-t parameters

section .data.bin.table
bin_table:
    DD 0xA5A5A5A5	; FSP_T_ADDR
    DD 0xBABADEDA	; FSP_T_ADDR_COMPLEMENT

    DD 0xA5A5A5A5	; MICRO_UPD_ADDR
    DD 0xBABADEDA	; MICRO_UPD_ADDR_COMPLEMENT

    DD 0xA5A5A5A5	; RESERVED
    DD 0xBABADEDA	; RESERVED