
#include <stdint.h>
#include <stddef.h>

#include <console.h>
#include <drivers.h>
#include <context.h>

void nmi_trigger()
{
	SCB->ICSR |= (1u << SCB_ICSR_NMIPENDSET_Pos);
}

#define EXC_RETURN_BASE_FRAME (1u << 4u)
#define EXC_RETURN_THREAD_MODE (1u << 3u)
#define EXC_RETURN_PROCESS_STACK (1u << 2u)

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
}
