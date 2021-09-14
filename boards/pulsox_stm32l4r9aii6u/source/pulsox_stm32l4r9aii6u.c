#include <stdint.h>
#include <stddef.h>

#define WEAK __attribute__ ((weak))
#define WEAK_AV __attribute__ ((weak, section(".after_vectors")))
#define ALIAS(f) __attribute__ ((weak, alias (#f)))
     
     void reset_handler(void);
WEAK void nmi_handler(void);
WEAK void hard_fault_handler(void);
WEAK void mem_manage_handler(void);
WEAK void bus_fault_handler(void);
WEAK void usage_fault_handler(void);
WEAK void svc_handler(void);
WEAK void debugmon_handler(void);
WEAK void pendsv_handler(void);
WEAK void systick_handler(void);

WEAK void int_default_handler(void);

extern int main(void);

extern uint32_t __flash_start__;
extern uint32_t __flash_end__;

extern uint32_t __sram_start__;
extern uint32_t __sram_end__;

extern uint32_t __text_start__;
extern uint32_t __text_end__;
extern uint32_t __data_load__;
extern uint32_t __data_start__;
extern uint32_t __data_end__;
extern uint32_t __bss_start__;
extern uint32_t __bss_end__;
extern uint32_t __stack_start__;
extern uint32_t __stack_end__;
extern void (* const isr_vector_table[])(void);

__attribute__ ((used, section(".isr_vector")))
void (* const isr_vector_table[])(void) = {
	(void (*)(void))&__stack_end__,	// The initial stack pointer
	reset_handler,              // The reset handler
	nmi_handler,                // The NMI handler
	hard_fault_handler,         // The hard fault handler
	mem_manage_handler,         // The MPU fault handler
	bus_fault_handler,          // The bus fault handler
	usage_fault_handler,        // The usage fault handler
	0,
	0,                          // ECRP
	0,                          // Reserved
	0,                          // Reserved
	svc_handler,                // SVCall handler
	debugmon_handler,           // Debug monitor handler
	0,                          // Reserved
	pendsv_handler,             // The PendSV handler
	systick_handler,            // The SysTick handler
	0,							/* Reserved                     */
};

__attribute__ ((section(".after_vectors.init_data")))
static void data_init(uint32_t romstart, uint32_t start, size_t len) {
	uint32_t *pulDest = (uint32_t*) start;
	uint32_t *pulSrc = (uint32_t*) romstart;
	uint32_t loop;
    for (loop = 0; loop < len; loop = loop + 4)
        *pulDest++ = *pulSrc++;
}

__attribute__ ((section(".after_vectors.init_bss")))
static void bss_init(uint32_t start, size_t len) {
	uint32_t *pulDest = (uint32_t*) start;
	uint32_t loop;
    for (loop = 0; loop < len; loop = loop + 4)
        *pulDest++ = 0;
}

__attribute__ ((section(".after_vectors.reset")))
void reset_handler(void) {

    // Disable interrupts
    __asm volatile ("cpsid i");

    data_init((uint32_t)&__data_load__, (uint32_t)&__data_start__, (size_t)&__data_end__ - (size_t)&__data_start__);
    bss_init((uint32_t)&__bss_start__, (uint32_t)&__bss_end__ - (uint32_t)&__bss_start__);

    // Reenable interrupts
    __asm volatile ("cpsie i");

	//	LDR     R0, =SystemInit
	//	BLX     R0
	//	LDR     R0, =__main
	//	BX      R0
	//	ENDP

    main();

    while (1) {
        ;
    }
}

//*****************************************************************************
// Default core exception handlers. Override the ones here by defining your own
// handler routines in your application code.
//*****************************************************************************
WEAK_AV void nmi_handler(void)
{	while(1) {}
}

WEAK_AV void hard_fault_handler(void)
{	while(1) {}
}

WEAK_AV void mem_manage_handler(void)
{	while(1) {}
}

WEAK_AV void bus_fault_handler(void)
{	while(1) {}
}

WEAK_AV void usage_fault_handler(void)
{	while(1) {}
}

WEAK_AV void svc_handler(void)
{	while(1) {}
}

WEAK_AV void debugmon_handler(void)
{	while(1) {}
}

WEAK_AV void pendsv_handler(void)
{	while(1) {}
}

WEAK_AV void systick_handler(void)
{	while(1) {}
}

WEAK_AV void int_default_handler(void)
{	while(1) {}
}
