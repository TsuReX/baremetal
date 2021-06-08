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
		mov x0, #0xEF
		mov x1, #0xEF
		mov x2, #0xEF
		mov x3, #0xEF

		mrs x0, SP_EL0
		mrs x1, SP_EL1
		mrs x2, SP_EL2
		mov x3, SP //SP_EL3

		mov x0, #0x1000
		mov x1, #0x2000
		mov x2, #0x3000
		mov x3, #0x4000

		msr SP_EL0, x0
		msr SP_EL1, x1
		msr SP_EL2, x2
		mov SP, x3 //SP_EL3

		msr SPSel, #0
		mov x0, SP

		msr SPSel, #1
		mov x0, SP

		b .
core_other:
		b .

