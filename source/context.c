
#include <stdint.h>
#include <stddef.h>

#include <console.h>
#include <drivers.h>
#include <context.h>

void nmi_trigger()
{
	SCB->ICSR |= (1u << SCB_ICSR_NMIPENDSET_Pos);
}

#define EXC_RETURN_BASE_FRAME		(1u << 4u)
#define EXC_RETURN_THREAD_MODE		(1u << 3u)
#define EXC_RETURN_PROCESS_STACK	(1u << 2u)
#define STACK_ALIGNED				(1u << 9u)

static void base_frame_print(const struct base_frame_t *frame)
{
	print("Base frame:\n\r");

	print("r0: 0x%08X\r\nr1: 0x%08X\r\nr2: 0x%08X\r\nr3: 0x%08X\r\nr12: 0x%08X\r\n",
			frame->r0, frame->r1, frame->r2, frame->r3, frame->r12);

	print("lr: 0x%08X\r\npc: 0x%08X\r\nxPSR: 0x%08X\r\n",
			frame->lr, frame->pc, frame->xpsr);
}

static void extended_frame_print(const struct extended_frame_t *frame)
{
	print("Extended frame:\n\r");

	print("r0: 0x%08X\r\nr1: 0x%08X\r\nr2: 0x%08X\r\nr3: 0x%08X\r\nr12: 0x%08X\r\n",
			frame->r0, frame->r1, frame->r2, frame->r3, frame->r12);

	print("lr: 0x%08X\r\npc: 0x%08X\r\nxPSR: 0x%08X\r\n",
			frame->lr, frame->pc, frame->xpsr);

	print("s0: 0x%08X\r\ns1: 0x%08X\r\ns2: 0x%08X\r\ns3: 0x%08X\r\n",
			frame->s0, frame->s1, frame->s2, frame->s3);
	print("s4: 0x%08X\r\ns5: 0x%08X\r\ns6: 0x%08X\r\ns7: 0x%08X\r\n",
			frame->s4, frame->s5, frame->s6, frame->s7);
	print("s8: 0x%08X\r\ns9: 0x%08X\r\ns10: 0x%08X\r\ns11: 0x%08X\r\n",
			frame->s8, frame->s9, frame->s10, frame->s11);
	print("s12: 0x%08X\r\ns13: 0x%08X\r\ns14: 0x%08X\r\ns15: 0x%08X\r\n",
			frame->s12, frame->s13, frame->s14, frame->s15);
}

void context_parse(size_t frame_size, uint32_t exc_return, uint32_t sp)
{
	print("frame_size: 0x%02X, lr: 0x%08X, sp: 0x%08X \r\n", frame_size, exc_return, sp);
	print("Interrupt saved ");
	if ((exc_return & EXC_RETURN_BASE_FRAME) !=0)
		print("BASE FRAME ");
	else
		print("EXTENDED FRAME ");

	print("and will return to ");
	if ((exc_return & EXC_RETURN_THREAD_MODE) !=0)
		print("THREAD MODE ");
	else
		print("HANDLER MODE ");

	print("and ");
	if ((exc_return & EXC_RETURN_PROCESS_STACK) !=0)
		print("PROCESS STACK\n\r");
	else
		print("MAIN STACK\n\r");

	if ((exc_return & EXC_RETURN_BASE_FRAME) !=0)
		base_frame_print((const struct base_frame_t*)sp);
	else
		extended_frame_print((const struct extended_frame_t*)sp);

	if ((((const struct base_frame_t*)sp)->xpsr & STACK_ALIGNED) !=0 )
		print("Stack pointer was ALIGNED\n\r");
	else
		print("Stack pointer was NOT ALIGNED\n\r");
}
