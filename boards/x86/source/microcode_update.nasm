global microcode_update
extern MIC_UPD

IA32_BIOS_UPDT_TRIG	equ 0x00000079

section .text.secphase
; eax - Address of microcode update region
; ebp - Return address
microcode_update:

    mov ecx, IA32_BIOS_UPDT_TRIG	; MSR to write in ECX
    add eax, 0x00			; Offset of the Update Data within the Update
    xor edx, edx			; Zero in EDX
    wrmsr				; microcode update trigger

    jmp ebp