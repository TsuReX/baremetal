bits 32

global setup_idt

section .text.secphase

setup_idt:
    push ebp		; *esp = ebp, ++esp
    mov ebp, esp	; ebp = esp

    push esi
    push eax
    push ds

    mov eax, 0x10
    mov esi, idt
    lidt [ds:esi]

    pop ds

    leave
    ret

; Arguments (n = 4): handler_addr, descriptor_addr, segment_idx, p_dpl_zer0_gate_value
setup_isr_n:
    push ebp		; *esp = ebp, ++esp
    mov ebp, esp	; ebp = esp

    push eax
    push ebx

    ;(ebp + 0x00) = esp
    ;(ebp + 0x04) = n-1 argument (P, DPL, Zero, Gate type) p_dpl_zer0_gate_value
    ;(ebp + 0x08) = n-2 argument (code segment of handler) segment_idx
    ;(ebp + 0x0C) = n-3 argument (descriptor address to be filled) descriptor_addr
    ;(ebp + 0x10) = n-4 argument (handler address) handler_addr

    ; Interrupt descriptor structure
    ; (0)dw Handler offset0
    ; (2)dw Code segment
    ; (4)db Reserver
    ; (5)db Present(1), DPL(2), Zero(1), Gate(4)
    ; (6)dw Handler offset1

    mov eax, [ebp + 0x0C]
    ; *(descriptor_addr + 0) = handler_addr & 0xFFFF
    mov ebx, [ebp + 0x10]
    mov [eax], ebx

    ; *(descriptor_addr + 2) = segment_idx
    mov bx, word [ebp + 0x08]
    mov [eax], word bx

    ; *(descriptor_addr + 4) = 0x0
    xor ebx, ebx
    mov [eax], byte bl

    ; *(descriptor_addr + 5) = p_dpl_zer0_gate_value
    mov bl, byte [ebp + 0x04]
    mov [eax], byte bl

    ; *(descriptor_addr + 6) = (handler_addr >> 16) & 0xFFFF
    mov ebx, [ebp + 0x10]
    shr ebx, 0x10
    mov [eax], word bx

    leave			; esp = ebp, pop ebp
    ret

isr0_handler:
    cli				;? interrupt gate and trap gate have differet behaviour in relation to disabling interrup request
    ; prologue
    push byte 0x0	; error code
    push byte 0x0	; isr number

    pusha			; ?
    push ds
    push es
    push fs
    push gs

    ; ? Why segments should be changed ?
    mov eax, 0x10	; offset of Data segment
    mov ds, eax
    mov es, eax
    mov fs, eax
    mov gs, eax

    mov eax, esp	; store esp on stack to use it 
    push eax		; in c-function as base pointer of structure
    mov eax, handler	; ? Why direct call isn't used ?
    call eax		;

    pop eax			; pop saved esp value

    pop gs
    pop fs
    pop es
    pop ds
    popa			; ?

    ;epilog
    add esp, 0x8	; 'pop' two values: isr number and error code

    iret

handler:
    push ebp		; *esp = ebp, ++esp
    mov ebp, esp	; ebp = esp

    nop

    leave			; esp = ebp, pop EBP
    ret

section .data

idt:
    dw idt_limit - 1	;
    dd idt_base		;

idt_base:
    times 8*32 db 0x0 ; reserve memory for the first 32 interrupt descriptors
idt_limit equ $