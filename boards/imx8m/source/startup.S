//#include <asm-offsets.h>
//#include <config.h>
//#include <linux/linkage.h>
//#include <asm/macro.h>
//#include <asm/armv8/mmu.h>

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
core_0:
		wfe
		b .
core_other:
		b .

