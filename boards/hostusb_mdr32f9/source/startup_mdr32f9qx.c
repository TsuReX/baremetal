#include <stdint.h>
#include <stddef.h>
#include <system_MDR32F9Qx.h>

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

WEAK void IntDefaultHandler(void);

WEAK void CAN1_IRQHandler(void);
WEAK void CAN2_IRQHandler(void);
WEAK void USB_IRQHandler(void);
WEAK void DMA_IRQHandler(void);
WEAK void UART1_IRQHandler(void);
WEAK void UART2_IRQHandler(void);
WEAK void SSP1_IRQHandler(void);
WEAK void I2C_IRQHandler(void);
WEAK void POWER_IRQHandler(void);
WEAK void WWDG_IRQHandler(void);
WEAK void Timer1_IRQHandler(void);
WEAK void Timer2_IRQHandler(void);
WEAK void Timer3_IRQHandler(void);
WEAK void ADC_IRQHandler(void);
WEAK void COMPARATOR_IRQHandler(void);
WEAK void SSP2_IRQHandler(void);
WEAK void BACKUP_IRQHandler(void);
WEAK void EXT_INT1_IRQHandler(void);
WEAK void EXT_INT2_IRQHandler(void);
WEAK void EXT_INT3_IRQHandler(void);
WEAK void EXT_INT4_IRQHandler(void);

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
	CAN1_IRQHandler,
	CAN2_IRQHandler,
	USB_IRQHandler,
	0,
	0,
	DMA_IRQHandler,
	UART1_IRQHandler,
	UART2_IRQHandler,
	SSP1_IRQHandler,
	0,
	I2C_IRQHandler,
	POWER_IRQHandler,
	WWDG_IRQHandler,
	0,
	Timer1_IRQHandler,
	Timer2_IRQHandler,
	Timer3_IRQHandler,
	ADC_IRQHandler,
	0,
	COMPARATOR_IRQHandler,
	SSP2_IRQHandler,
	0,
	0,
	0,
	0,
	0,
	0,
	BACKUP_IRQHandler,
	EXT_INT1_IRQHandler,
	EXT_INT2_IRQHandler,
	EXT_INT3_IRQHandler,
	EXT_INT4_IRQHandler,
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

void setup_psp()
{
	__asm(	".syntax unified\n"
			"ldr r0, =0x12345678\n"
			"msr PSP, r0\n"
			".syntax divided\n"
		);
	return;
}

__attribute__ ((section(".after_vectors.reset")))
void reset_handler(void) {

    // Disable interrupts
    __asm volatile ("cpsid i");


    // Enable SRAM clock used by Stack
    __asm volatile ("LDR R0, =0x40000220\n\t"
                    "MOV R1, #56\n\t"
                    "STR R1, [R0]");
    setup_psp();

    data_init((uint32_t)&__data_load__, (uint32_t)&__data_start__, (size_t)&__data_end__ - (size_t)&__data_start__);
    bss_init((uint32_t)&__bss_start__, (uint32_t)&__bss_end__ - (uint32_t)&__bss_start__);

    // Check to see if we are running the code from a non-zero
    // address (eg RAM, external flash), in which case we need
    // to modify the VTOR register to tell the CPU that the
    // vector table is located at a non-0x0 address.
    uint32_t* pSCB_VTOR = (uint32_t *) 0xE000ED08;
    if ((uint32_t *)isr_vector_table!=(uint32_t *) 0x00000000) {
        *pSCB_VTOR = (uint32_t)isr_vector_table;
    }

    // Reenable interrupts
    __asm volatile ("cpsie i");

	//	LDR     R0, =SystemInit
	//	BLX     R0
	//	LDR     R0, =__main
	//	BX      R0
	//	ENDP

	SystemInit();
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


WEAK_AV void IntDefaultHandler(void)
{	while(1) {}
}


WEAK void CAN1_IRQHandler(void)
{	while(1) {}
}

WEAK void CAN2_IRQHandler(void)
{	while(1) {}
}

WEAK void USB_IRQHandler(void)
{	while(1) {}
}

WEAK void DMA_IRQHandler(void)
{	while(1) {}
}

WEAK void UART1_IRQHandler(void)
{	while(1) {}
}

WEAK void UART2_IRQHandler(void)
{	while(1) {}
}

WEAK void SSP1_IRQHandler(void)
{	while(1) {}
}

WEAK void I2C_IRQHandler(void)
{	while(1) {}
}

WEAK void POWER_IRQHandler(void)
{	while(1) {}
}

WEAK void WWDG_IRQHandler(void)
{	while(1) {}
}

WEAK void Timer1_IRQHandler(void)
{	while(1) {}
}

WEAK void Timer2_IRQHandler(void)
{	while(1) {}
}

WEAK void Timer3_IRQHandler(void)
{	while(1) {}
}

WEAK void ADC_IRQHandler(void)
{	while(1) {}
}


WEAK void COMPARATOR_IRQHandler(void)
{	while(1) {}
}

WEAK void SSP2_IRQHandler(void)
{	while(1) {}
}


WEAK void BACKUP_IRQHandler(void)
{	while(1) {}
}

WEAK void EXT_INT1_IRQHandler(void)
{	while(1) {}
}

WEAK void EXT_INT2_IRQHandler(void)
{	while(1) {}
}

WEAK void EXT_INT3_IRQHandler(void)
{	while(1) {}
}

WEAK void EXT_INT4_IRQHandler(void)
{	while(1) {}
}

