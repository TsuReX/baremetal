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
	b	.
