bits 16

global _reset_vector

extern setup_car
global setup_car_return

extern setup_protected_mode
global setup_protected_mode_return


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

    jmp setup_protected_mode
setup_protected_mode_return:

    jmp setup_car
setup_car_return:

    jmp $
