bits 32

CR0_CD		equ (0x1 << 29)
CR0_NW		equ (0x1 << 30)

NEM			equ 0x000002E0
NEM_RUN			equ (0x1 << 1)
NEM_SETUP		equ (0x1 << 0)

IA32_MTRR_CAP			equ 0x000000FE

IA32_MTRR_DEF_TYPE		equ 0x000002FF
IA32_MTRR_DEF_TYPE_EN		equ (0x1 < 11)
IA32_MTRR_DEF_TYPE_FE		equ (0x1 < 10)
IA32_MTRR_DEF_TYPE_MEMTYPE_WP	equ (0x5 < 0)
IA32_MTRR_DEF_TYPE_MEMTYPE_WB	equ (0x6 < 0)

IA32_MTRR_PHYS_BASE_0	equ 0x00000200
IA32_MTRR_PHYS_BASE_1	equ 0x00000202

IA32_MTRR_PHYS_MASK_0	equ 0x00000201
IA32_MTRR_PHYS_MASK_1	equ 0x00000203
IA32_MTRR_PHYS_MASK_VALID	equ (0x1 << 11)


DATA_STACK_BASE_ADDRESS		equ 0xFFFE0000
DATA_STACK_SIZE_MASK		equ 0x0000FFFF
CODE_REGION_BASE_ADDRESS	equ 0xFFFF0000
CODE_REGION_SIZE_MASK		equ 0x0000FFFF

IA32_MTRR_FIX_64K_00000		equ 0x250
IA32_MTRR_FIX_16K_80000		equ 0x258
IA32_MTRR_FIX_16K_A0000		equ 0x259
IA32_MTRR_FIX_4K_C0000		equ 0x268
IA32_MTRR_FIX_4K_C8000		equ 0x269
IA32_MTRR_FIX_4K_D0000		equ 0x26a
IA32_MTRR_FIX_4K_D8000		equ 0x26b
IA32_MTRR_FIX_4K_E0000		equ 0x26c
IA32_MTRR_FIX_4K_E8000		equ 0x26d
IA32_MTRR_FIX_4K_F0000		equ 0x26e
IA32_MTRR_FIX_4K_F8000		equ 0x26f



IA32_MISC_ENABLE		equ 0x000001A0
IA32_MISC_ENABLE_FAST_STRINGS	equ (0x1 < 0)

global setup_car
extern setup_car_return

section .text.secphase

check_mtrr:
;Use the MTRR default type MSR as a proxy for detecting INIT#.
;Reset the system if any known bits are set in that MSR. That is
;an indication of the CPU not being properly reset.

check_for_clean_reset:
    mov ecx, IA32_MTRR_DEF_TYPE
    rdmsr
    and eax, (IA32_MTRR_DEF_TYPE_EN | IA32_MTRR_DEF_TYPE_FE)
    cmp eax, 0x0
    jnz warm_reset
    jmp esp

;Perform warm reset
warm_reset:
    mov dx, 0x0CF9
    mov al, 0x06
    out dx, al

;594768_3rd Gen Intel Xeon Scalable Processors_BWG_Rev1p4
;5.3.1 Enabling Cache for Stack and Code Use Prior to Memory Initialization
setup_car:
    mov esp, cache_as_ram
    jmp esp

cache_as_ram:

;Send INIT IPI to all excluding ourself.
    mov eax, 0x000C4500
    mov esi, 0xFEE00300
    mov [esi], eax

;All CPUs need to be in Wait for SIPI state
wait_for_sipi:
    mov eax,[esi]
    bt eax, 12
    jc wait_for_sipi

;Clean-up IA32_MTRR_DEF_TYPE
    mov ecx, IA32_MTRR_DEF_TYPE
    xor eax, eax
    xor edx, edx
    wrmsr

;Clear/disable fixed MTRRs
    mov ebx, fixed_mtrr_list
    xor eax, eax
    xor edx, edx

clear_fixed_mtrr:
    movzx ecx, word [ebx]; ??? word ???
    wrmsr
    add ebx, 0x2
    cmp ebx, fixed_mtrr_list_end
    jl clear_fixed_mtrr

;Zero out all variable range MTRRs.
    mov ecx, IA32_MTRR_CAP
    rdmsr
    and eax, 0xFF
    shl eax, 0x1
    mov edi, eax
    mov ecx, 0x200
    xor eax, eax
    xor edx, edx

clear_var_mtrrs:
    wrmsr
    add ecx, 0x1
    dec edi
    jnz clear_var_mtrrs

;Disable Fast_String support prior to NEM
    mov ecx, IA32_MISC_ENABLE
    rdmsr
    and eax, ~IA32_MISC_ENABLE_FAST_STRINGS
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


;8
;Configure the DataStack region as write-back (WB) cacheable memory type using the variable range MTRRs.

    mov eax, (DATA_STACK_BASE_ADDRESS | IA32_MTRR_DEF_TYPE_MEMTYPE_WB)	; Load the write-back cache value
    xor edx, edx						; clear upper dword
    mov ecx, IA32_MTRR_PHYS_BASE_0				; Load the MTRR index
    wrmsr							; the value in MTRR_PHYS_BASE_0

    mov eax, (DATA_STACK_SIZE_MASK | IA32_MTRR_PHYS_MASK_VALID)	; turn on the Valid flag
    mov edx, esi						; edx <- MTRR_PHYS_MASK_HIGH
    mov ecx, IA32_MTRR_PHYS_MASK_0				; Load the MTRR index
    wrmsr 							; the value in MTRR_PHYS_BASE_0

;10
;Configure the CodeRegion region as write-protected (WP) cacheable memory type using the variable range MTRRs.

    mov eax, (CODE_REGION_BASE_ADDRESS | IA32_MTRR_DEF_TYPE_MEMTYPE_WP)	; Load the write-protected cache value
    xor edx, edx						; clear upper dword
    mov ecx, IA32_MTRR_PHYS_BASE_1				; Load the MTRR index
    wrmsr							; the value in MTRR_PHYS_BASE_1

    mov eax, (CODE_REGION_SIZE_MASK | IA32_MTRR_PHYS_MASK_VALID)	; turn on the Valid flag
    mov edx, esi						; edx <- MTRR_PHYS_MASK_HIGH
    mov ecx, IA32_MTRR_PHYS_MASK_1				; Load the MTRR index
    wrmsr


;11
    mov ecx, IA32_MTRR_DEF_TYPE
    rdmsr
    or eax, IA32_MTRR_DEF_TYPE_EN
    wrmsr

;12
    invd
    mov eax, cr0
    and eax, ~(CR0_NW | CR0_CD)	;Reset NW and CD bits
    mov cr0, eax

;13
    mov ecx, NEM	;Read MSR NEM
    rdmsr
    or eax, NEM_SETUP	;Set SETUP bit
    wrmsr

;14
;One location in each 64-byte cache line of the DataStack region must be written to
;set all cached values to the modified state.

;15
    mov ecx, NEM
    rdmsr
    or eax, NEM_RUN	;Set RUN bit
    wrmsr

    jmp setup_car_return

section .data
fixed_mtrr_list:
    DW IA32_MTRR_FIX_64K_00000
    DW IA32_MTRR_FIX_16K_80000
    DW IA32_MTRR_FIX_16K_A0000
    DW IA32_MTRR_FIX_4K_C0000
    DW IA32_MTRR_FIX_4K_C8000
    DW IA32_MTRR_FIX_4K_D0000
    DW IA32_MTRR_FIX_4K_D8000
    DW IA32_MTRR_FIX_4K_E0000
    DW IA32_MTRR_FIX_4K_E8000
    DW IA32_MTRR_FIX_4K_F0000
    DW IA32_MTRR_FIX_4K_F8000
fixed_mtrr_list_end:
