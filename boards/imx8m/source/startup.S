
.section .text
.globl	_start
_start:
	ldr x0, =0
	mrs	x0, CurrentEL
	ldr x0, =0
	mrs	x0, MPIDR_EL1
	and x0, x0, #0xFF
	cmp x0, #0x0
	bne	core_other
	b	core_0

core_0:
	bl clear_cache
	b .
core_other:
		b .

