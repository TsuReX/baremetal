bits 32
IDT		equ 0x00001000
IDT_BASE	equ 0x00001008
IDT_LIMIT	equ ((8 * 256) - 1)

global setup_idt
global isrn_test

section .text.secphase

; arg0 - being tested isr number
isrn_test:
    push ebp
    mov ebp, esp
    push eax
    push edx
    ;mov eax, [ebp + 0x8]

    mov al, 0x80
    out 0xA5, al

    mov edx, 0x0
    mov [edx], dword 0xAA

    int 3

    mov edx, 0x0
    mov edx, dword [edx]

    mov al, 0x80
    out dx, al

    jmp $

    leave
    ret

setup_idt:
    push ebp		; *esp = ebp, ++esp
    mov ebp, esp	; ebp = esp

    push esi
    push eax
    push ds

    mov esi, IDT
    mov [esi], word IDT_LIMIT
    mov [esi + 4], dword IDT_BASE

    ;mov ax, 0x10
    ;mov ds, ax

    ;0th
    push isr0_handler	; handler_addr

    ;1st
    mov eax, IDT_BASE	; base of descriptor table
    add eax, 0x18	; offset of the first descriptor
    push eax		; descriptor_addr

    ;2nd
    mov eax, 0x8	; code segment index
    push eax		; segment_idx

    ;3rd
    push (0x80 | 0x00 | 0x00 | 0x0E)	; p_dpl_zer0_gate_value


    call setup_isr_n
    add esp, 0x10

    lidt [ds:esi]
    mov eax, 0x00
    sidt [eax]

    pop ds
    pop eax
    pop esi

    leave
    ret

; Arguments (n = 4): handler_addr, descriptor_addr, segment_idx, p_dpl_zer0_gate_value
setup_isr_n:
    push ebp		; *esp = ebp, ++esp
    mov ebp, esp	; ebp = esp

    push eax
    push ebx

    ;(ebp + 0x00) = ebp
    ;(ebp + 0x04) = return address
    ;(ebp + 0x08) = n-1 argument (P, DPL, Zero, Gate type) p_dpl_zer0_gate_value
    ;(ebp + 0x0C) = n-2 argument (code segment of handler) segment_idx
    ;(ebp + 0x10) = n-3 argument (descriptor address to be filled) descriptor_addr
    ;(ebp + 0x14) = n-4 argument (handler address) handler_addr

    ; Interrupt descriptor structure
    ; (0)dw Handler offset0
    ; (2)dw Code segment
    ; (4)db Reserver
    ; (5)db Present(1), DPL(2), Zero(1), Gate(4)
    ; (6)dw Handler offset1

    mov eax, [ebp + 0x10]
    ; *(descriptor_addr + 0) = handler_addr & 0xFFFF
    mov ebx, [ebp + 0x14]
    mov [eax + 0x0], ebx

    ; *(descriptor_addr + 2) = segment_idx
    mov bx, word [ebp + 0x0C]
    mov [eax + 0x2], word bx

    ; *(descriptor_addr + 4) = 0x0
    xor ebx, ebx
    mov [eax + 0x4], byte bl

    ; *(descriptor_addr + 5) = p_dpl_zer0_gate_value
    mov bl, byte [ebp + 0x08]
    mov [eax + 0x5], byte bl

    ; *(descriptor_addr + 6) = (handler_addr >> 16) & 0xFFFF
    mov ebx, [ebp + 0x14]
    shr ebx, 0x10
    mov [eax + 0x6], word bx

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

    sti
    iret

handler:
    push ebp		; *esp = ebp, ++esp
    mov ebp, esp	; ebp = esp

    nop
    mov edx, 0x0
    inc dword [edx]


    leave			; esp = ebp, pop ebp
    ret

section .data
