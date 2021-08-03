#include <stdint.h>
#include <stddef.h>
//#include <system_stm32f4xx.h>

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
WEAK void TAMP_STAMP_IRQHandler(void);
WEAK void RTC_WKUP_IRQHandler(void);
WEAK void FLASH_IRQHandler(void);
WEAK void RCC_IRQHandler(void);
WEAK void EXTI0_IRQHandler(void);
WEAK void EXTI1_IRQHandler(void);
WEAK void EXTI2_IRQHandler(void);
WEAK void EXTI3_IRQHandler(void);
WEAK void EXTI4_IRQHandler(void);
WEAK void DMA1_Stream0_IRQHandler(void);
WEAK void DMA1_Stream1_IRQHandler(void);
WEAK void DMA1_Stream2_IRQHandler(void);
WEAK void DMA1_Stream3_IRQHandler(void);
WEAK void DMA1_Stream4_IRQHandler(void);
WEAK void DMA1_Stream5_IRQHandler(void);
WEAK void DMA1_Stream6_IRQHandler(void);
WEAK void ADC_IRQHandler(void);
WEAK void EXTI9_5_IRQHandler(void);
WEAK void TIM1_BRK_TIM9_IRQHandler(void);
WEAK void TIM1_UP_TIM10_IRQHandler(void);
WEAK void TIM1_TRG_COM_TIM11_IRQHandler(void);
WEAK void TIM1_CC_IRQHandler(void);
WEAK void TIM2_IRQHandler(void);
WEAK void TIM3_IRQHandler(void);
WEAK void TIM4_IRQHandler(void);
WEAK void I2C1_EV_IRQHandler(void);
WEAK void I2C1_ER_IRQHandler(void);
WEAK void I2C2_EV_IRQHandler(void);
WEAK void I2C2_ER_IRQHandler(void);
WEAK void SPI1_IRQHandler(void);
WEAK void SPI2_IRQHandler(void);
WEAK void USART1_IRQHandler(void);
WEAK void USART2_IRQHandler(void);
WEAK void EXTI15_10_IRQHandler(void);
WEAK void RTC_Alarm_IRQHandler(void);
WEAK void OTG_FS_WKUP_IRQHandler(void);
WEAK void DMA1_Stream7_IRQHandler(void);
WEAK void SDIO_IRQHandler(void);
WEAK void TIM5_IRQHandler(void);
WEAK void SPI3_IRQHandler(void);
WEAK void DMA2_Stream0_IRQHandler(void);
WEAK void DMA2_Stream1_IRQHandler(void);
WEAK void DMA2_Stream2_IRQHandler(void);
WEAK void DMA2_Stream3_IRQHandler(void);
WEAK void DMA2_Stream4_IRQHandler(void);
WEAK void OTG_FS_IRQHandler(void);
WEAK void DMA2_Stream5_IRQHandler(void);
WEAK void DMA2_Stream6_IRQHandler(void);
WEAK void DMA2_Stream7_IRQHandler(void);
WEAK void USART6_IRQHandler(void);
WEAK void I2C3_EV_IRQHandler(void);
WEAK void I2C3_ER_IRQHandler(void);
WEAK void FPU_IRQHandler(void);
WEAK void SPI4_IRQHandler(void);

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
	reset_handler,				// The reset handler
	nmi_handler,				// The NMI handler
	hard_fault_handler,			// The hard fault handler
	mem_manage_handler,			// The MPU fault handler
	bus_fault_handler,			// The bus fault handler
	usage_fault_handler,		// The usage fault handler
	0,
	0,							// ECRP
	0,							// Reserved
	0,							// Reserved
	svc_handler,				// SVCall handler
	debugmon_handler,			// Debug monitor handler
	0,							// Reserved
	pendsv_handler,				// The PendSV handler
	systick_handler,			// The SysTick handler
	WWDG_IRQHandler,			/* Window WatchDog			*/
	PVD_IRQHandler,				/* PVD through EXTI Line detection				*/
	TAMP_STAMP_IRQHandler,		/* Tamper and TimeStamps through the EXTI line	*/
	RTC_WKUP_IRQHandler,		/* RTC Wakeup through the EXTI line				*/
	FLASH_IRQHandler,			/* FLASH					*/
	RCC_IRQHandler,				/* RCC						*/
	EXTI0_IRQHandler,			/* EXTI Line0				*/
	EXTI1_IRQHandler,			/* EXTI Line1				*/
	EXTI2_IRQHandler,			/* EXTI Line2				*/
	EXTI3_IRQHandler,			/* EXTI Line3				*/
	EXTI4_IRQHandler,			/* EXTI Line4				*/
	DMA1_Stream0_IRQHandler,	/* DMA1 Stream 0			*/
	DMA1_Stream1_IRQHandler,	/* DMA1 Stream 1			*/
	DMA1_Stream2_IRQHandler,	/* DMA1 Stream 2			*/
	DMA1_Stream3_IRQHandler,	/* DMA1 Stream 3			*/
	DMA1_Stream4_IRQHandler,	/* DMA1 Stream 4			*/
	DMA1_Stream5_IRQHandler,	/* DMA1 Stream 5			*/
	DMA1_Stream6_IRQHandler,	/* DMA1 Stream 6			*/
	ADC_IRQHandler,				/* ADC1, ADC2 and ADC3s		*/
	0,							/* Reserved					*/
	0,							/* Reserved					*/
	0,							/* Reserved					*/
	0,							/* Reserved					*/
	EXTI9_5_IRQHandler,			/* External Line[9:5]s		*/
	TIM1_BRK_TIM9_IRQHandler,	/* TIM1 Break and TIM9		*/
	TIM1_UP_TIM10_IRQHandler,	/* TIM1 Update and TIM10	*/
	TIM1_TRG_COM_TIM11_IRQHandler,	/* TIM1 Trigger and Commutation and TIM11 */
	TIM1_CC_IRQHandler,			/* TIM1 Capture Compare		*/
	TIM2_IRQHandler,			/* TIM2						*/
	TIM3_IRQHandler,			/* TIM3						*/
	TIM4_IRQHandler,			/* TIM4						*/
	I2C1_EV_IRQHandler,			/* I2C1 Event				*/
	I2C1_ER_IRQHandler,			/* I2C1 Error				*/
	I2C2_EV_IRQHandler,			/* I2C2 Event				*/
	I2C2_ER_IRQHandler,			/* I2C2 Error				*/
	SPI1_IRQHandler,			/* SPI1						*/
	SPI2_IRQHandler,			/* SPI2						*/
	USART1_IRQHandler,			/* USART1					*/
	USART2_IRQHandler,			/* USART2					*/
	0,							/* Reserved					*/
	EXTI15_10_IRQHandler,		/* External Line[15:10]s	*/
	RTC_Alarm_IRQHandler,		/* RTC Alarm (A and B) through EXTI Line	*/
	OTG_FS_WKUP_IRQHandler,		/* USB OTG FS Wakeup through EXTI line		*/
	0,							/* Reserved					*/
	0,							/* Reserved					*/
	0,							/* Reserved					*/
	0,							/* Reserved					*/
	DMA1_Stream7_IRQHandler,	/* DMA1 Stream7				*/
	0,							/* Reserved					*/
	SDIO_IRQHandler,			/* SDIO						*/
	TIM5_IRQHandler,			/* TIM5						*/
	SPI3_IRQHandler,			/* SPI3						*/
	0,							/* Reserved					*/
	0,							/* Reserved					*/
	0,							/* Reserved					*/
	0,							/* Reserved					*/
	DMA2_Stream0_IRQHandler,	/* DMA2 Stream 0			*/
	DMA2_Stream1_IRQHandler,	/* DMA2 Stream 1			*/
	DMA2_Stream2_IRQHandler,	/* DMA2 Stream 2			*/
	DMA2_Stream3_IRQHandler,	/* DMA2 Stream 3			*/
	DMA2_Stream4_IRQHandler,	/* DMA2 Stream 4			*/
	0,							/* Reserved					*/
	0,							/* Reserved					*/
	0,							/* Reserved					*/
	0,							/* Reserved					*/
	0,							/* Reserved					*/
	0,							/* Reserved					*/
	OTG_FS_IRQHandler,			/* USB OTG FS				*/
	DMA2_Stream5_IRQHandler,	/* DMA2 Stream 5			*/
	DMA2_Stream6_IRQHandler,	/* DMA2 Stream 6			*/
	DMA2_Stream7_IRQHandler,	/* DMA2 Stream 7			*/
	USART6_IRQHandler,			/* USART6					*/
	I2C3_EV_IRQHandler,			/* I2C3 event				*/
	I2C3_ER_IRQHandler,			/* I2C3 error				*/
	0,							/* Reserved					*/
	0,							/* Reserved					*/
	0,							/* Reserved					*/
	0,							/* Reserved					*/
	0,							/* Reserved					*/
	0,							/* Reserved					*/
	0,							/* Reserved					*/
	FPU_IRQHandler,				/* FPU						*/
	0,							/* Reserved					*/
	0,							/* Reserved					*/
	SPI4_IRQHandler				/* SPI4						*/
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

//	setup_psp();

	data_init((uint32_t)&__data_load__, (uint32_t)&__data_start__, (size_t)&__data_end__ - (size_t)&__data_start__);
	bss_init((uint32_t)&__bss_start__, (uint32_t)&__bss_end__ - (uint32_t)&__bss_start__);

//	// Check to see if we are running the code from a non-zero
//	// address (eg RAM, external flash), in which case we need
//	// to modify the VTOR register to tell the CPU that the
//	// vector table is located at a non-0x0 address.
//	uint32_t* pSCB_VTOR = (uint32_t *) 0xE000ED08;
//	if ((uint32_t *)isr_vector_table!=(uint32_t *) 0x00000000) {
//		*pSCB_VTOR = (uint32_t)isr_vector_table;
//	}

	// Reenable interrupts
	__asm volatile ("cpsie i");

//	SystemInit();
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

WEAK void WWDG_IRQHandler(void)
{	while(1) {}
}

WEAK void PVD_IRQHandler(void)
{	while(1) {}
}

WEAK void TAMP_STAMP_IRQHandler(void)
{	while(1) {}
}

WEAK void RTC_WKUP_IRQHandler(void)
{	while(1) {}
}

WEAK void FLASH_IRQHandler(void)
{	while(1) {}
}

WEAK void RCC_IRQHandler(void)
{	while(1) {}
}

WEAK void EXTI0_IRQHandler(void)
{	while(1) {}
}

WEAK void EXTI1_IRQHandler(void)
{	while(1) {}
}

WEAK void EXTI2_IRQHandler(void)
{	while(1) {}
}

WEAK void EXTI3_IRQHandler(void)
{	while(1) {}
}

WEAK void EXTI4_IRQHandler(void)
{	while(1) {}
}

WEAK void DMA1_Stream0_IRQHandler(void)
{	while(1) {}
}

WEAK void DMA1_Stream1_IRQHandler(void)
{	while(1) {}
}

WEAK void DMA1_Stream2_IRQHandler(void)
{	while(1) {}
}

WEAK void DMA1_Stream3_IRQHandler(void)
{	while(1) {}
}

WEAK void DMA1_Stream4_IRQHandler(void)
{	while(1) {}
}

WEAK void DMA1_Stream5_IRQHandler(void)
{	while(1) {}
}

WEAK void DMA1_Stream6_IRQHandler(void)
{	while(1) {}
}

WEAK void ADC_IRQHandler(void)
{	while(1) {}
}

WEAK void EXTI9_5_IRQHandler(void)
{	while(1) {}
}

WEAK void TIM1_BRK_TIM9_IRQHandler(void)
{	while(1) {}
}

WEAK void TIM1_UP_TIM10_IRQHandler(void)
{	while(1) {}
}

WEAK void TIM1_TRG_COM_TIM11_IRQHandler(void)
{	while(1) {}
}

WEAK void TIM1_CC_IRQHandler(void)
{	while(1) {}
}

WEAK void TIM2_IRQHandler(void)
{	while(1) {}
}

WEAK void TIM3_IRQHandler(void)
{	while(1) {}
}

WEAK void TIM4_IRQHandler(void)
{	while(1) {}
}

WEAK void I2C1_EV_IRQHandler(void)
{	while(1) {}
}

WEAK void I2C1_ER_IRQHandler(void)
{	while(1) {}
}

WEAK void I2C2_EV_IRQHandler(void)
{	while(1) {}
}

WEAK void I2C2_ER_IRQHandler(void)
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

WEAK void EXTI15_10_IRQHandler(void)
{	while(1) {}
}

WEAK void RTC_Alarm_IRQHandler(void)
{	while(1) {}
}

WEAK void OTG_FS_WKUP_IRQHandler(void)
{	while(1) {}
}

WEAK void DMA1_Stream7_IRQHandler(void)
{	while(1) {}
}

WEAK void SDIO_IRQHandler(void)
{	while(1) {}
}

WEAK void TIM5_IRQHandler(void)
{	while(1) {}
}

WEAK void SPI3_IRQHandler(void)
{	while(1) {}
}

WEAK void DMA2_Stream0_IRQHandler(void)
{	while(1) {}
}

WEAK void DMA2_Stream1_IRQHandler(void)
{	while(1) {}
}

WEAK void DMA2_Stream2_IRQHandler(void)
{	while(1) {}
}

WEAK void DMA2_Stream3_IRQHandler(void)
{	while(1) {}
}

WEAK void DMA2_Stream4_IRQHandler(void)
{	while(1) {}
}

WEAK void OTG_FS_IRQHandler(void)
{	while(1) {}
}

WEAK void DMA2_Stream5_IRQHandler(void)
{	while(1) {}
}

WEAK void DMA2_Stream6_IRQHandler(void)
{	while(1) {}
}

WEAK void DMA2_Stream7_IRQHandler(void)
{	while(1) {}
}

WEAK void USART6_IRQHandler(void)
{	while(1) {}
}

WEAK void I2C3_EV_IRQHandler(void)
{	while(1) {}
}

WEAK void I2C3_ER_IRQHandler(void)
{	while(1) {}
}

WEAK void FPU_IRQHandler(void)
{	while(1) {}
}

WEAK void SPI4_IRQHandler(void)
{	while(1) {}
}

