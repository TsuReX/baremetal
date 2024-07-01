section .text

global _start

_start:
    mov ax, bx

section .data
    db 10h, 0x11
    ;incbin "file.bin"
    db 0xEF, 0xFF
    ;db 10 dup (0x5A)

