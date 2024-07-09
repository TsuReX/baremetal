bits 32

global setup_car

section .text.secphase

;594768_3rd Gen Intel Xeon Scalable Processors_BWG_Rev1p4
;5.3.1 Enabling Cache for Stack and Code Use Prior to Memory Initialization
setup_car:

;Disable Fast_String support prior to NEM
    mov ecx, 0x1A0	;IA32_MTRR_DEF_TYPE
    rdmsr
    and eax, ~0x1	;Reset FAST_STRINGS bit
    wrmsr

; Configure MTRR_PHYS_MASK_HIGH for proper addressing above 4GB
; based on the physical address size supported for this processor
; This is based on read from CPUID.(EAX=080000008h), EAX bits [7:0]
;
; Examples:
; MTRR_PHYS_MASK_HIGH = 00000000Fh For 36 bit addressing
; MTRR_PHYS_MASK_HIGH = 0000000FFh For 40 bit addressing
; MTRR_PHYS_MASK_HIGH = 00000FFFFh For 48 bit addressing
;
    mov eax, 80000008h	; Address sizes leaf
    cpuid
    sub al, 32		;!IF! al == 48 -> al = 48 - 32 = 16 
    movzx eax, al	;eax = al -> eax == 16	
    xor esi, esi	;esi = 0
    bts esi, eax	;esi[eax] = 1 -> esi[16] = 1 -> esi == 0b00000001.00000000.00000000 == 0x1.0000 == (1 << 16)
    dec esi		;esi = esi - 1 -> esi = 0x1.0000 - 1 -> esi = 0xFFFF

    DATA_STACK_BASE_ADDRESS	equ 0xFFFE0000
    DATA_STACK_SIZE_MASK	equ 0x0000FFFF
    CODE_REGION_BASE_ADDRESS	equ 0xFFFF0000
    CODE_REGION_SIZE_MASK	equ 0x0000FFFF
    MTRR_MEMORY_TYPE_WB		equ 0x00000006
    MTRR_MEMORY_TYPE_WP		equ 0x00000005
    MTRR_PHYS_MASK_VALID	equ (0x1 << 11)

    MTRR_PHYS_BASE_0	equ 0x000000200
    MTRR_PHYS_MASK_0	equ 0x000000201
    MTRR_PHYS_BASE_1	equ 0x000000202
    MTRR_PHYS_MASK_1	equ 0x000000203

;8
;Configure the DataStack region as write-back (WB) cacheable memory type using the variable range MTRRs.

    mov eax, (DATA_STACK_BASE_ADDRESS | MTRR_MEMORY_TYPE_WB)	; Load the write-back cache value
    xor edx, edx						; clear upper dword
    mov ecx, MTRR_PHYS_BASE_0					; Load the MTRR index
    wrmsr							; the value in MTRR_PHYS_BASE_0

    mov eax, (DATA_STACK_SIZE_MASK | MTRR_PHYS_MASK_VALID)	; turn on the Valid flag
    mov edx, esi						; edx <- MTRR_PHYS_MASK_HIGH
    mov ecx, MTRR_PHYS_MASK_0					; Load the MTRR index
    wrmsr 							; the value in MTRR_PHYS_BASE_0

;10
;Configure the CodeRegion region as write-protected (WP) cacheable memory type using the variable range MTRRs.

    mov eax, (CODE_REGION_BASE_ADDRESS | MTRR_MEMORY_TYPE_WP)	; Load the write-protected cache value
    xor edx, edx						; clear upper dword
    mov ecx, MTRR_PHYS_BASE_1					; Load the MTRR index
    wrmsr							; the value in MTRR_PHYS_BASE_1

    mov eax, (CODE_REGION_SIZE_MASK | MTRR_PHYS_MASK_VALID)	; turn on the Valid flag
    mov edx, esi						; edx <- MTRR_PHYS_MASK_HIGH
    mov ecx, MTRR_PHYS_MASK_1					; Load the MTRR index
    wrmsr




;11
    mov ecx, 0x2FF	;IA32_MTRR_DEF_TYPE
    rdmsr
    or eax, 0x1 << 11	;Set EN bit
    wrmsr

;12
    invd
    mov eax, cr0
    and eax, ~(0x1 << 30 | 0x1 << 29)	;Reset NW and CD bits
    mov cr0, eax

;13
    mov ecx, 0x2E0	;Read MSR NEM
    rdmsr
    or eax, 0x1		;Set SETUP bit
    wrmsr

;14
;One location in each 64-byte cache line of the DataStack region must be written to
;set all cached values to the modified state.

;15
    mov ecx, 0x2E0	;Read MSR NEM
    rdmsr
    or eax, 0x2		;Set RUN bit
    wrmsr

section .data
gdt_entry_zero:
    dd 0x0
    dd 0x0

gdt_entry_code:

%define cs_limit	0x0000FFFF
%define cs_base		0x000F0000
%define cs_access	0x9F
%define cs_flag		(0x0 & 0xF)
    dw (cs_limit)
    dw (cs_base & 0xFFFF)
    db ((cs_base >> 16) & 0xFF)
    db (cs_access)
    db (((cs_limit >> 16) & 0xF) | cs_flag)
    db ((cs_base >> 24) & 0xFF)

gdt_entry_data:

%define ds_limit	0x0000FFFF
%define ds_base		0x000F0000
%define ds_access	0x93
%define ds_flag		(0x0 & 0xF)
    dw (ds_limit)
    dw (ds_base & 0xFFFF)
    db ((ds_base >> 16) & 0xFF)
    db (ds_access)
    db (((ds_limit >> 16) & 0xF) | cs_flag)
    db ((ds_base >> 24) & 0xFF)

gdt_len equ ($ - gdt_entry_zero)
