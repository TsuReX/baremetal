bits 16

global _reset_vector

extern setup_car
extern setup_protected_mode
extern microcode_update
global setup_protected_mode_return


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

    jmp setup_protected_mode
bits 32
setup_protected_mode_return:
    mov al, 0x80
    out 0x56, al

    mov ebp, microcode_update_return
    jmp microcode_update
microcode_update_return:

    mov ebp, setup_car_return
    jmp setup_car
setup_car_return:
    mov edx, eax
    mov al, 0x80
    out 0x57, al
    mov eax, edx

    mov esp, eax; Region base address
    add esp, ebx; Add region size

; Filling stack for debugging purpose
; It's needed to check ability to access memory
    mov ebp, esp
    mov ecx, 0x400
fill_stack:
    push esp
    loop fill_stack
    mov esp, ebp

    mov ecx, ((1 << 2) - 1)
    mov al, 0x80
delay:
    mov dx, cx
    out dx, al
    loop delay

    push ebx
    push eax
    call c_entry

    jmp $
