bits 16

global _reset_vector

section .text.resetvector
_reset_vector:
    mov eax, 0x0
    cpuid
    jmp _sec_entry
    TIMES(0x10 - ($ - $$)) nop

section .text.secphase
_sec_entry:
    mov ax, cs
    jmp $


section .data
    db 10h, 0x11
    ;incbin "file.bin"
    db 0xEF, 0xFF
    ;db 10 dup (0x5A)

