global microcode_update
extern MIC_UPD

IA32_BIOS_UPDT_TRIG	equ 0x00000079

section .text.secphase

microcode_update:

    mov ecx, IA32_BIOS_UPDT_TRIG	; MSR to write in ECX
    mov eax, MIC_UPD			; Offset of microcode update
    add eax, 0x30			; Offset of the Update Data within the Update
    xor edx, edx			; Zero in EDX
    wrmsr				; microcode update trigger

    jmp ebp