
#include <stdint.h>
#include <stddef.h>

#include <console.h>
#include <drivers.h>
#include <context.h>

#define EXC_RETURN_BASE_FRAME		(1u << 4u)
#define EXC_RETURN_THREAD_MODE		(1u << 3u)
#define EXC_RETURN_PROCESS_STACK	(1u << 2u)
#define STACK_ALIGNED				(1u << 9u)

uint32_t msp = 0xFFFFFFFF;
uint32_t exc_return = 0x0;

uint32_t current_context_num = 0;
uint32_t next_context_num = 0;
uint32_t msp = (uint32_t)&__stack_end__;


struct context context_array[CONTEXT_COUNT];

#define PENDSV_POS	14

void pendsv_setup(func_ptr handler)
{
	SCB->VTOR[PENDSV_POS] = handler;
}

func_ptr pendsv_get()
{
	return SCB->VTOR[PENDSV_POS];
}

void nmi_trigger()
{
	SCB->ICSR |= (1u << SCB_ICSR_NMIPENDSET_Pos);
}

void pendsv_trigger()
{
	SCB->ICSR |= (1u << SCB_ICSR_PENDSVSET_Pos);
}

static void base_frame_print(const struct base_frame_t *frame)
{
	print("Base frame:\n\r");

	print("r0: 0x%08lX\r\nr1: 0x%08lX\r\nr2: 0x%08lX\r\nr3: 0x%08lX\r\nr12: 0x%08lX\r\n",
			frame->r0, frame->r1, frame->r2, frame->r3, frame->r12);

	print("lr: 0x%08lX\r\npc: 0x%08lX\r\nxPSR: 0x%08lX\r\n",
			frame->lr, frame->pc, frame->xpsr);
}

static void extended_frame_print(const struct extended_frame_t *frame)
{
	print("Extended frame:\n\r");

	print("r0: 0x%08lX\r\nr1: 0x%08lX\r\nr2: 0x%08lX\r\nr3: 0x%08lX\r\nr12: 0x%08lX\r\n",
			frame->r0, frame->r1, frame->r2, frame->r3, frame->r12);

	print("lr: 0x%08lX\r\npc: 0x%08lX\r\nxPSR: 0x%08lX\r\n",
			frame->lr, frame->pc, frame->xpsr);

	print("s0: 0x%08lX\r\ns1: 0x%08lX\r\ns2: 0x%08lX\r\ns3: 0x%08lX\r\n",
			frame->s0, frame->s1, frame->s2, frame->s3);
	print("s4: 0x%08lX\r\ns5: 0x%08lX\r\ns6: 0x%08lX\r\ns7: 0x%08lX\r\n",
			frame->s4, frame->s5, frame->s6, frame->s7);
	print("s8: 0x%08lX\r\ns9: 0x%08lX\r\ns10: 0x%08lX\r\ns11: 0x%08lX\r\n",
			frame->s8, frame->s9, frame->s10, frame->s11);
	print("s12: 0x%08lX\r\ns13: 0x%08lX\r\ns14: 0x%08lX\r\ns15: 0x%08lX\r\n",
			frame->s12, frame->s13, frame->s14, frame->s15);
}

void context_parse(size_t frame_size, uint32_t exc_return, uint32_t sp)
{
	print("frame_size: 0x%02X, lr: 0x%08lX, sp: 0x%08lX \r\n", frame_size, exc_return, sp);
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

/*
 * @brief	Сохраняет контекст текущего процесса.
 */
void context_store(uint32_t *context_stack, uint32_t context_num)
{
	if((((struct context*)context_stack)->exc_return & EXC_RETURN_BASE_FRAME) != 0) {
		/* Base context. */
		memcpy(&context_array[context_num], context_stack, BASE_CONTEXT_SIZE);
	}
	else {
		/* Extended context. */
		memcpy(&context_array[context_num], context_stack, FULL_CONTEXT_SIZE);
	}
}

/*
 * @brief	Восстанавливает контекст процесса.
 */
void context_load(uint32_t *context_stack, uint32_t context_num)
{
	if((context_array[context_num].exc_return & EXC_RETURN_BASE_FRAME) != 0) {
		/* Base context. */
		memcpy(context_stack, &context_array[context_num], BASE_CONTEXT_SIZE);
	}
	else {
		/* Extended context. */
		memcpy( context_stack, &context_array[context_num],FULL_CONTEXT_SIZE);
	}
}

__attribute__((naked, section(".after_vectors")))
void pendsv_ret_to_init(void)
{
	__asm(
		".syntax unified\n"
		"@ set new main stack pointer\n"
		"ldr r0, =msp\n"
		"ldr sp, [r0]\n"
		
		"@ r0 - destination address\n"
		"mov r0, sp\n"

		"@ r1 - source address\n"
		"ldr r1, =contex_array\n"
		"ldr r1, [r1]\n"
		"mov r2, #0x90\n"
		"ldr r3, =next_contex_num\n"
		"ldr r3, [r3]\n"
		"mov r0, #0x0\n"
		"umlal r0, r1, r2, r3\n"

		"@ r2 - size\n"
		"ldr r2, [r1, #0x0]	@ r2 = contex_array[next_contex_num].exc_return\n"
		"mov r3, #EXC_RETURN_BASE_FRAME\n"
		"and r2, r3\n"
		"ite neq \n"
		"movne r2, #0x48\n"
		"moveq r2, #0x90\n"

		"@ move sp to the end of exception handler stack\n"
		"add sp, r2\n"

		"@ copy context from special place to stack\n"
		"@mov r3, lr @ unnecessary because this function is the last\n"
		"bl memcpy\n"
		"@mov lr, r3 @ unnecessary\n"

		"@ restore lr, psp, r4-r11\n"
		"ldm sp, {r0,r1,r4-r11}\n"
		"msr PSP, r1\n"
		"mov lr, r0\n"
		"add sp, #0x28\n"
		".syntax divided\n"
	);
}

__attribute__((naked, section(".after_vectors")))
void pendsv_handler(void)
{
	__asm(
		".syntax unified\n"
		"@ save lr, psp, r4-r11\n"
		"add sp, #-0x28\n"
		"mov r0, lr\n"
		"mrs r1, PSP\n"
		"stm sp, {r0,r1,r4-r11}\n"

		"@ r0 - destination address\n"
		"ldr r0, =contex_array\n"
		"ldr r0, [r0]\n"
		"mov r1, #0x90\n"
		"ldr r2, =current_contex_num\n"
		"ldr r2, [r2]\n"
		"mov r3, #0x0\n"
		"umlal r3, r0, r1, r2\n"

		"@ r1 - source address\n"
		"mov r1, sp\n"

		"@ r2 - size\n"
		"mov r3, #EXC_RETURN_BASE_FRAME\n"
		"and lr, r3\n"
		"ite neq \n"
		"movne r2, #0x48\n"
		"moveq r2, #0x90\n"

		"@ copy context from stack to special place\n"
		"mov r3, lr @ save lr for future using\n"
		"bl memcpy\n"
		"mov lr, r3 @ restore modified lr\n"

		"@ move sp to the beginning of exception handler stack\n"
		"add sp, r2\n"

		"@ any action\n"

		"@ r0 - destination address\n"
		"mov r0, sp\n"

		"@ r1 - source address\n"
		"ldr r1, =contex_array\n"
		"ldr r1, [r1]\n"
		"mov r2, #0x90\n"
		"ldr r3, =next_contex_num\n"
		"ldr r3, [r3]\n"
		"mov r0, #0x0\n"
		"umlal r0, r1, r2, r3\n"

		"@ r2 - size\n"
		"ldr r2, [r1, #0x0]	@ r2 = contex_array[next_contex_num].exc_return\n"
		"mov r3, #EXC_RETURN_BASE_FRAME\n"
		"and r2, r3\n"
		"ite neq \n"
		"movne r2, #0x48\n"
		"moveq r2, #0x90\n"

		"@ move sp to the end of exception handler stack\n"
		"add sp, r2\n"

		"@ copy context from special place to stack\n"
		"@mov r3, lr @ unnecessary because this function is the last\n"
		"bl memcpy\n"
		"@mov lr, r3 @ unnecessary\n"

		"@ restore lr, psp, r4-r11\n"
		"ldm sp, {r0,r1,r4-r11}\n"
		"msr PSP, r1\n"
		"mov lr, r0\n"
		"add sp, #0x28\n"
		".syntax divided\n"
	);
}

__attribute__((naked, section(".after_vectors")))
void _pendsv_handler(void)
{
	/* extended_context_store */
	__asm(	".syntax unified\n"
			"add sp, #-0x28\n"
			"mov r0, lr\n"
			"mrs r1, PSP\n"
			"stm sp, {r0,r1,r4-r11}\n"
			".syntax divided\n"
		);

	/* contex_param_save*/
	__asm(	".syntax unified\n"
			"mov %0, lr\n"
			"mov %1, sp\n"
			".syntax divided\n"
			: "=r" (exc_return), "=r" (msp)
		);

	/* any_actions */

	/* context_store */
	if((context_array[current_context_num].exc_return & EXC_RETURN_BASE_FRAME) != 0) {
		/* Base context. */
		memcpy((void*)msp, &context_array[current_context_num], BASE_CONTEXT_SIZE);
		__asm(	".syntax unified\n"
				"add sp, #0x48\n"
				".syntax divided\n"
			);
	}
	else {
		/* Extended context. */
		memcpy((void*)current_context_num, &context_array[current_context_num], FULL_CONTEXT_SIZE);
		__asm(	".syntax unified\n"
				"add sp, #0x90\n"
				".syntax divided\n"
			);
	}

	/* context_load */
	if((context_array[next_context_num].exc_return & EXC_RETURN_BASE_FRAME) != 0) {
		/* Base context. */
		__asm(	".syntax unified\n"
				"add sp, #0x-48\n"
				".syntax divided\n"
			);
		memcpy((void*)msp, &context_array[next_context_num], BASE_CONTEXT_SIZE);
	}
	else {
		/* Extended context. */
		__asm(	".syntax unified\n"
				"add sp, #-0x90\n"
				".syntax divided\n"
			);
		memcpy((void*)msp, &context_array[next_context_num],FULL_CONTEXT_SIZE);
	}

	/* extended_context_load */
	__asm(	".syntax unified\n"
			"ldm sp, {r0,r1,r4-r11}\n"
			"msr PSP, r1\n"
			"mov lr, r0\n"
			"add sp, #0x28\n"
			".syntax divided\n"
		);
}

__attribute__((used, naked, section(".after_vectors")))
void temp_pendsv_handler(void)
{

	__asm(	".syntax unified\n"
			"ldr r0, =__stack_end__\n"
			"mov sp, r0\n"
			".syntax divided\n"
		);

	/* context_load */
	if((context_array[next_context_num].exc_return & EXC_RETURN_BASE_FRAME) != 0) {
		/* Base context. */
		__asm(	".syntax unified\n"
				"add sp, #0x-48\n"
				".syntax divided\n"
			);
		memcpy((void*)msp, &context_array[next_context_num], BASE_CONTEXT_SIZE);
	}
	else {
		/* Extended context. */
		__asm(	".syntax unified\n"
				"add sp, #-0x90\n"
				".syntax divided\n"
			);
		memcpy((void*)msp, &context_array[next_context_num],FULL_CONTEXT_SIZE);
	}

	/* extended_context_load */
	__asm(	".syntax unified\n"
			"ldm sp, {r0,r1,r4-r11}\n"
			"msr PSP, r1\n"
			"mov lr, r0\n"
			"add sp, #0x28\n"
			".syntax divided\n"
		);
}

void init_process_start()
{
	msp = __stack_end__;
	pendsv_setup(pendsv_ret_to_init);
	pendsv_trigger();
	/* Control flow doesn't reach here.
	 * It switches to init_process() function. */
}
void init_process(void)
{
	pendsv_setup(pendsv_handler);
	/* Process body. */
	while(1);
}

void init_process_create(uint32_t proc_addr, size_t  proc_addr_size)
{
	memset(&context_array[0], 0, sizeof(context_array[0]));
	context_array[0].exc_return = 0xFFFFFFFD; /* Return to thread mode and process stack with basic frame. */
	context_array[0].psp = (proc_addr & !0x7) + (proc_addr_size & !0x7) - 8;
	context_array[0].lr = 0xDEADBEEF;
	context_array[0].pc = init_process;
	context_array[0].xpsr = 0x1000000; /* Set EPRS.T bit! */
}
