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

; BE SURE this value match the same value from run-qemu.sh file
; For Qemu
FSP_T_ADDR_STORAGE equ 0x00080000
; For real hardware
;FSP_T_ADDR_STORAGE equ 0x????.????

    mov edi, FSP_T_ADDR_STORAGE
    mov ebx, [edi]	; Read address of FSP-T
    mov ecx, [edi + 4]	; Read complement to address of FSP-T
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

    jmp eax

without_fsp_t:

    mov ebp, microcode_update_return
    jmp microcode_update
microcode_update_return:

    mov ebp, setup_car_return
    jmp setup_car
setup_car_return:

    mov esp, eax; Region base address
    add esp, ebx; Add region size

    mov edx, eax
    mov al, 0x80
    out 0x57, al
    mov eax, edx

; Filling stack for debugging purpose
; It's needed to check ability to access memory
    mov ebp, esp
    mov ecx, ebx
    shr ecx, 2
fill_stack:
    push esp
    loop fill_stack
    mov esp, ebp

    call setup_idt
;    call isr_0_test
;    call isr_11_test

    push ebx
    push eax
    call c_entry

    jmp $
