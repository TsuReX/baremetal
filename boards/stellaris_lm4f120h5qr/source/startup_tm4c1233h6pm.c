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
WEAK void IntDefaultHandler(void);

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

void usb0_irq_handler(void);

extern void (* const isr_vector_table[])(void);

__attribute__ ((used, section(".isr_vector")))
void (* const isr_vector_table[])(void) = {
	(void (*)(void))&__stack_end__,                       // The initial stack pointer
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

	IntDefaultHandler,                      // GPIO Port A
	IntDefaultHandler,                      // GPIO Port B
	IntDefaultHandler,                      // GPIO Port C
	IntDefaultHandler,                      // GPIO Port D
	IntDefaultHandler,                      // GPIO Port E
	IntDefaultHandler,                      // UART0 Rx and Tx
	IntDefaultHandler,                      // UART1 Rx and Tx
	IntDefaultHandler,                      // SSI0 Rx and Tx
	IntDefaultHandler,                      // I2C0 Master and Slave
	IntDefaultHandler,                      // PWM Fault
	IntDefaultHandler,                      // PWM Generator 0
	IntDefaultHandler,                      // PWM Generator 1
	IntDefaultHandler,                      // PWM Generator 2
	IntDefaultHandler,                      // Quadrature Encoder 0
	IntDefaultHandler,                      // ADC Sequence 0
	IntDefaultHandler,                      // ADC Sequence 1
	IntDefaultHandler,                      // ADC Sequence 2
	IntDefaultHandler,                      // ADC Sequence 3
	IntDefaultHandler,                      // Watchdog timer
	IntDefaultHandler,                      // Timer 0 subtimer A
	IntDefaultHandler,                      // Timer 0 subtimer B
	IntDefaultHandler,                      // Timer 1 subtimer A
	IntDefaultHandler,                      // Timer 1 subtimer B
	IntDefaultHandler,                      // Timer 2 subtimer A
	IntDefaultHandler,                      // Timer 2 subtimer B
	IntDefaultHandler,                      // Analog Comparator 0
	IntDefaultHandler,                      // Analog Comparator 1
	IntDefaultHandler,                      // Analog Comparator 2
	IntDefaultHandler,                      // System Control (PLL, OSC, BO)
	IntDefaultHandler,                      // FLASH Control
	IntDefaultHandler,                      // GPIO Port F
	IntDefaultHandler,                      // GPIO Port G
	IntDefaultHandler,                      // GPIO Port H
	IntDefaultHandler,                      // UART2 Rx and Tx
	IntDefaultHandler,                      // SSI1 Rx and Tx
	IntDefaultHandler,                      // Timer 3 subtimer A
	IntDefaultHandler,                      // Timer 3 subtimer B
	IntDefaultHandler,                      // I2C1 Master and Slave
	IntDefaultHandler,                      // Quadrature Encoder 1
	IntDefaultHandler,                      // CAN0
	IntDefaultHandler,                      // CAN1
	0,                                      // Reserved
	0,                                      // Reserved
	IntDefaultHandler,                      // Hibernate
	IntDefaultHandler,                      // USB0
	IntDefaultHandler,                      // PWM Generator 3
	IntDefaultHandler,                      // uDMA Software Transfer
	IntDefaultHandler,                      // uDMA Error
	IntDefaultHandler,                      // ADC1 Sequence 0
	IntDefaultHandler,                      // ADC1 Sequence 1
	IntDefaultHandler,                      // ADC1 Sequence 2
	IntDefaultHandler,                      // ADC1 Sequence 3
	0,                                      // Reserved
	0,                                      // Reserved
	IntDefaultHandler,                      // GPIO Port J
	IntDefaultHandler,                      // GPIO Port K
	IntDefaultHandler,                      // GPIO Port L
	IntDefaultHandler,                      // SSI2 Rx and Tx
	IntDefaultHandler,                      // SSI3 Rx and Tx
	IntDefaultHandler,                      // UART3 Rx and Tx
	IntDefaultHandler,                      // UART4 Rx and Tx
	IntDefaultHandler,                      // UART5 Rx and Tx
	IntDefaultHandler,                      // UART6 Rx and Tx
	IntDefaultHandler,                      // UART7 Rx and Tx
	0,                                      // Reserved
	0,                                      // Reserved
	0,                                      // Reserved
	0,                                      // Reserved
	IntDefaultHandler,                      // I2C2 Master and Slave
	IntDefaultHandler,                      // I2C3 Master and Slave
	IntDefaultHandler,                      // Timer 4 subtimer A
	IntDefaultHandler,                      // Timer 4 subtimer B
	0,                                      // Reserved
	0,                                      // Reserved
	0,                                      // Reserved
	0,                                      // Reserved
	0,                                      // Reserved
	0,                                      // Reserved
	0,                                      // Reserved
	0,                                      // Reserved
	0,                                      // Reserved
	0,                                      // Reserved
	0,                                      // Reserved
	0,                                      // Reserved
	0,                                      // Reserved
	0,                                      // Reserved
	0,                                      // Reserved
	0,                                      // Reserved
	0,                                      // Reserved
	0,                                      // Reserved
	0,                                      // Reserved
	0,                                      // Reserved
	IntDefaultHandler,                      // Timer 5 subtimer A
	IntDefaultHandler,                      // Timer 5 subtimer B
	IntDefaultHandler,                      // Wide Timer 0 subtimer A
	IntDefaultHandler,                      // Wide Timer 0 subtimer B
	IntDefaultHandler,                      // Wide Timer 1 subtimer A
	IntDefaultHandler,                      // Wide Timer 1 subtimer B
	IntDefaultHandler,                      // Wide Timer 2 subtimer A
	IntDefaultHandler,                      // Wide Timer 2 subtimer B
	IntDefaultHandler,                      // Wide Timer 3 subtimer A
	IntDefaultHandler,                      // Wide Timer 3 subtimer B
	IntDefaultHandler,                      // Wide Timer 4 subtimer A
	IntDefaultHandler,                      // Wide Timer 4 subtimer B
	IntDefaultHandler,                      // Wide Timer 5 subtimer A
	IntDefaultHandler,                      // Wide Timer 5 subtimer B
	IntDefaultHandler,                      // FPU
	0,                                      // Reserved
	0,                                      // Reserved
	IntDefaultHandler,                      // I2C4 Master and Slave
	IntDefaultHandler,                      // I2C5 Master and Slave
	IntDefaultHandler,                      // GPIO Port M
	IntDefaultHandler,                      // GPIO Port N
	IntDefaultHandler,                      // Quadrature Encoder 2
	0,                                      // Reserved
	0,                                      // Reserved
	IntDefaultHandler,                      // GPIO Port P (Summary or P0)
	IntDefaultHandler,                      // GPIO Port P1
	IntDefaultHandler,                      // GPIO Port P2
	IntDefaultHandler,                      // GPIO Port P3
	IntDefaultHandler,                      // GPIO Port P4
	IntDefaultHandler,                      // GPIO Port P5
	IntDefaultHandler,                      // GPIO Port P6
	IntDefaultHandler,                      // GPIO Port P7
	IntDefaultHandler,                      // GPIO Port Q (Summary or Q0)
	IntDefaultHandler,                      // GPIO Port Q1
	IntDefaultHandler,                      // GPIO Port Q2
	IntDefaultHandler,                      // GPIO Port Q3
	IntDefaultHandler,                      // GPIO Port Q4
	IntDefaultHandler,                      // GPIO Port Q5
	IntDefaultHandler,                      // GPIO Port Q6
	IntDefaultHandler,                      // GPIO Port Q7
	IntDefaultHandler,                      // GPIO Port R
	IntDefaultHandler,                      // GPIO Port S
	IntDefaultHandler,                      // PWM 1 Generator 0
	IntDefaultHandler,                      // PWM 1 Generator 1
	IntDefaultHandler,                      // PWM 1 Generator 2
	IntDefaultHandler,                      // PWM 1 Generator 3
	IntDefaultHandler                       // PWM 1 Fault

}; /* End of g_pfnVectors */

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
			"ldr r0, =__stack_end__\n"
			"msr PSP, r0\n"
			"ldr r1, =0x2\n"
			"msr CONTROL, r1\n"
			".syntax divided\n"
		);
	return;
}

__attribute__ ((section(".after_vectors.reset")))
void reset_handler(void) {

    // Disable interrupts
    __asm volatile ("cpsid i");

//    setup_psp();

    data_init((uint32_t)&__data_load__, (uint32_t)&__data_start__, (size_t)&__data_end__ - (size_t)&__data_start__);
    bss_init((uint32_t)&__bss_start__, (uint32_t)&__bss_end__ - (uint32_t)&__bss_start__);

    uint32_t* pSCB_VTOR = (uint32_t *) 0xE000ED08;
    if ((uint32_t *)isr_vector_table!=(uint32_t *) 0x00000000) {
        *pSCB_VTOR = (uint32_t)isr_vector_table;
    }

    // Reenable interrupts
    __asm volatile ("cpsie i");

    main();

    while (1) {
        ;
    }
}

WEAK_AV void nmi_handler(void)
{ while(1) {}
}

WEAK_AV void hard_fault_handler(void)
{ while(1) {}
}

WEAK_AV void mem_manage_handler(void)
{ while(1) {}
}

WEAK_AV void bus_fault_handler(void)
{ while(1) {}
}

WEAK_AV void usage_fault_handler(void)
{ while(1) {}
}

WEAK_AV void svc_handler(void)
{ while(1) {}
}

WEAK_AV void debugmon_handler(void)
{ while(1) {}
}

WEAK_AV void pendsv_handler(void)
{ while(1) {}
}

WEAK_AV void systick_handler(void)
{ while(1) {}
}

WEAK_AV void IntDefaultHandler(void)
{ while(1) {}
}
