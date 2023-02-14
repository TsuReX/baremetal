#include <stdint.h>
#include <stddef.h>
#include <system_stm32l0xx.h>

#define WEAK __attribute__ ((weak))
#define WEAK_AV __attribute__ ((weak, section(".after_vectors")))
     
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

WEAK void WWDG_IRQHandler(void);
WEAK void PVD_IRQHandler(void);
WEAK void RTC_IRQHandler(void);
WEAK void FLASH_IRQHandler(void);
WEAK void RCC_IRQHandler(void);
WEAK void EXTI0_1_IRQHandler(void);
WEAK void EXTI2_3_IRQHandler(void);
WEAK void EXTI4_15_IRQHandler(void);
WEAK void DMA1_Channel1_IRQHandler(void);
WEAK void DMA1_Channel2_3_IRQHandler(void);
WEAK void DMA1_Channel4_5_6_7_IRQHandler(void);
WEAK void ADC1_COMP_IRQHandler(void);
WEAK void LPTIM1_IRQHandler(void);
WEAK void USART4_5_IRQHandler(void);
WEAK void TIM2_IRQHandler(void);
WEAK void TIM3_IRQHandler(void);
WEAK void TIM6_IRQHandler(void);
WEAK void TIM7_IRQHandler(void);
WEAK void TIM21_IRQHandler(void);
WEAK void I2C3_IRQHandler(void);
WEAK void TIM22_IRQHandler(void);
WEAK void I2C1_IRQHandler(void);
WEAK void I2C2_IRQHandler(void);
WEAK void SPI1_IRQHandler(void);
WEAK void SPI2_IRQHandler(void);
WEAK void USART1_IRQHandler(void);
WEAK void USART2_IRQHandler(void);
WEAK void LPUART1_IRQHandler(void);

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
	reset_handler,              	// The reset handler
	nmi_handler,                	// The NMI handler
	hard_fault_handler,         	// The hard fault handler
	0,								// The MPU fault handler
	0,								// The bus fault handler
	0,								// The usage fault handler
	0,
	0,                          	// ECRP
	0,                          	// Reserved
	0,                          	// Reserved
	svc_handler,                	// SVCall handler
	0,           					// Debug monitor handler
	0,                          	// Reserved
	pendsv_handler,             	// The PendSV handler
	systick_handler,            	// The SysTick handler
	WWDG_IRQHandler,				// Window WatchDog
	PVD_IRQHandler,					// PVD through EXTI Line detection
	RTC_IRQHandler,					// RTC through the EXTI line
	FLASH_IRQHandler,				// FLASH
	RCC_IRQHandler,					// RCC
	EXTI0_1_IRQHandler,				// EXTI Line 0 and 1
	EXTI2_3_IRQHandler,				// EXTI Line 2 and 3
	EXTI4_15_IRQHandler,			// EXTI Line 4 to 15
	0,              				// Reserved
	DMA1_Channel1_IRQHandler,		// DMA1 Channel 1
	DMA1_Channel2_3_IRQHandler,		// DMA1 Channel 2 and Channel 3
	DMA1_Channel4_5_6_7_IRQHandler,	// DMA1 Channel 4, Channel 5, Channel 6 and Channel 7
	ADC1_COMP_IRQHandler,			// ADC1, COMP1 and COMP2
	LPTIM1_IRQHandler,				// LPTIM1
	USART4_5_IRQHandler,			// USART4 and USART 5
	TIM2_IRQHandler,				// TIM2
	TIM3_IRQHandler,				// TIM3
	TIM6_IRQHandler,				// TIM6 and DAC
	TIM7_IRQHandler,				// TIM7
	0,								// Reserved
	TIM21_IRQHandler,				// TIM21
	I2C3_IRQHandler,				// I2C3
	TIM22_IRQHandler,				// TIM22
	I2C1_IRQHandler,				// I2C1
	I2C2_IRQHandler,				// I2C2
	SPI1_IRQHandler,				// SPI1
	SPI2_IRQHandler,				// SPI2
	USART1_IRQHandler,				// USART1
	USART2_IRQHandler,				// USART2
	LPUART1_IRQHandler,				// LPUART1
	0,								// Reserved
	0,								// Reserved*/
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

WEAK void WWDG_IRQHandler(void)
{	while(1) {}
}

WEAK void PVD_IRQHandler(void)
{	while(1) {}
}

WEAK void RTC_IRQHandler(void)
{	while(1) {}
}

WEAK void FLASH_IRQHandler(void)
{	while(1) {}
}

WEAK void RCC_IRQHandler(void)
{	while(1) {}
}

WEAK void EXTI0_1_IRQHandler(void)
{	while(1) {}
}

WEAK void EXTI2_3_IRQHandler(void)
{	while(1) {}
}

WEAK void EXTI4_15_IRQHandler(void)
{	while(1) {}
}

WEAK void DMA1_Channel1_IRQHandler(void)
{	while(1) {}
}

WEAK void DMA1_Channel2_3_IRQHandler(void)
{	while(1) {}
}

WEAK void DMA1_Channel4_5_6_7_IRQHandler(void)
{	while(1) {}
}

WEAK void ADC1_COMP_IRQHandler(void)
{	while(1) {}
}

WEAK void LPTIM1_IRQHandler(void)
{	while(1) {}
}

WEAK void USART4_5_IRQHandler(void)
{	while(1) {}
}

WEAK void TIM2_IRQHandler(void)
{	while(1) {}
}

WEAK void TIM3_IRQHandler(void)
{	while(1) {}
}

WEAK void TIM6_IRQHandler(void)
{	while(1) {}
}

WEAK void TIM7_IRQHandler(void)
{	while(1) {}
}

WEAK void TIM21_IRQHandler(void)
{	while(1) {}
}

WEAK void I2C3_IRQHandler(void)
{	while(1) {}
}

WEAK void TIM22_IRQHandler(void)
{	while(1) {}
}

WEAK void I2C1_IRQHandler(void)
{	while(1) {}
}

WEAK void I2C2_IRQHandler(void)
{	while(1) {}
}

WEAK void SPI1_IRQHandler(void)
{	while(1) {}
}

WEAK void SPI2_IRQHandler(void)
{	while(1) {}
}

WEAK void USART1_IRQHandler(void)
{	while(1) {}
}

WEAK void USART2_IRQHandler(void)
{	while(1) {}
}

WEAK void LPUART1_IRQHandler(void)
{	while(1) {}
}


