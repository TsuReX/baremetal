#include <stdint.h>
#include <stddef.h>
#include <system_stm32f1xx.h>

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
WEAK void TAMPER_IRQHandler(void);
WEAK void RTC_IRQHandler(void);
WEAK void FLASH_IRQHandler(void);
WEAK void RCC_IRQHandler(void);
WEAK void EXTI0_IRQHandler(void);
WEAK void EXTI1_IRQHandler(void);
WEAK void EXTI2_IRQHandler(void);
WEAK void EXTI3_IRQHandler(void);
WEAK void EXTI4_IRQHandler(void);
WEAK void DMA1_Channel1_IRQHandler(void);
WEAK void DMA1_Channel2_IRQHandler(void);
WEAK void DMA1_Channel3_IRQHandler(void);
WEAK void DMA1_Channel4_IRQHandler(void);
WEAK void DMA1_Channel5_IRQHandler(void);
WEAK void DMA1_Channel6_IRQHandler(void);
WEAK void DMA1_Channel7_IRQHandler(void);
WEAK void ADC1_2_IRQHandler(void);
WEAK void USB_HP_CAN1_TX_IRQHandler(void);
WEAK void usb_lp_can1_rx0_handle(void);
WEAK void CAN1_RX1_IRQHandler(void);
WEAK void CAN1_SCE_IRQHandler(void);
WEAK void EXTI9_5_IRQHandler(void);
WEAK void TIM1_BRK_IRQHandler(void);
WEAK void TIM1_UP_IRQHandler(void);
WEAK void TIM1_TRG_COM_IRQHandler(void);
WEAK void TIM1_CC_IRQHandler(void);
WEAK void TIM2_IRQHandler(void);
WEAK void TIM3_IRQHandler(void);
WEAK void TIM4_IRQHandler(void);
WEAK void I2C1_EV_IRQHandler(void);
WEAK void I2C1_ER_IRQHandler(void);
WEAK void I2C2_EV_IRQHandler(void);
WEAK void I2C2_ER_IRQHandler(void);
WEAK void spi1_irq_handler(void);
WEAK void SPI2_IRQHandler(void);
WEAK void USART1_IRQHandler(void);
WEAK void USART2_IRQHandler(void);
WEAK void USART3_IRQHandler(void);
WEAK void EXTI15_10_IRQHandler(void);
WEAK void RTC_Alarm_IRQHandler(void);
WEAK void usbwakeup_handle(void);

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
	WWDG_IRQHandler,
	PVD_IRQHandler,
	TAMPER_IRQHandler,
	RTC_IRQHandler,
	FLASH_IRQHandler,
	RCC_IRQHandler,
	EXTI0_IRQHandler,
	EXTI1_IRQHandler,
	EXTI2_IRQHandler,
	EXTI3_IRQHandler,
	EXTI4_IRQHandler,
	DMA1_Channel1_IRQHandler,
	DMA1_Channel2_IRQHandler,
	DMA1_Channel3_IRQHandler,
	DMA1_Channel4_IRQHandler,
	DMA1_Channel5_IRQHandler,
	DMA1_Channel6_IRQHandler,
	DMA1_Channel7_IRQHandler,
	ADC1_2_IRQHandler,
	USB_HP_CAN1_TX_IRQHandler,
	usb_lp_can1_rx0_handle,
	CAN1_RX1_IRQHandler,
	CAN1_SCE_IRQHandler,
	EXTI9_5_IRQHandler,
	TIM1_BRK_IRQHandler,
	TIM1_UP_IRQHandler,
	TIM1_TRG_COM_IRQHandler,
	TIM1_CC_IRQHandler,
	TIM2_IRQHandler,
	TIM3_IRQHandler,
	TIM4_IRQHandler,
	I2C1_EV_IRQHandler,
	I2C1_ER_IRQHandler,
	I2C2_EV_IRQHandler,
	I2C2_ER_IRQHandler,
	spi1_irq_handler,
	SPI2_IRQHandler,
	USART1_IRQHandler,
	USART2_IRQHandler,
	USART3_IRQHandler,
	EXTI15_10_IRQHandler,
	RTC_Alarm_IRQHandler,
	usbwakeup_handle,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
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


WEAK_AV void IntDefaultHandler(void)
{	while(1) {}
}


WEAK void WWDG_IRQHandler(void)
{	while(1) {}
}

WEAK void PVD_IRQHandler(void)
{	while(1) {}
}

WEAK void TAMPER_IRQHandler(void)
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

WEAK void DMA1_Channel1_IRQHandler(void)
{	while(1) {}
}

WEAK void DMA1_Channel2_IRQHandler(void)
{	while(1) {}
}

WEAK void DMA1_Channel3_IRQHandler(void)
{	while(1) {}
}

WEAK void DMA1_Channel4_IRQHandler(void)
{	while(1) {}
}

WEAK void DMA1_Channel5_IRQHandler(void)
{	while(1) {}
}

WEAK void DMA1_Channel6_IRQHandler(void)
{	while(1) {}
}

WEAK void DMA1_Channel7_IRQHandler(void)
{	while(1) {}
}

WEAK void ADC1_2_IRQHandler(void)
{	while(1) {}
}

WEAK void USB_HP_CAN1_TX_IRQHandler(void)
{	while(1) {}
}

WEAK void usb_lp_can1_rx0_handle(void)
{	while(1) {}
}

WEAK void CAN1_RX1_IRQHandler(void)
{	while(1) {}
}

WEAK void CAN1_SCE_IRQHandler(void)
{	while(1) {}
}

WEAK void EXTI9_5_IRQHandler(void)
{	while(1) {}
}

WEAK void TIM1_BRK_IRQHandler(void)
{	while(1) {}
}

WEAK void TIM1_UP_IRQHandler(void)
{	while(1) {}
}

WEAK void TIM1_TRG_COM_IRQHandler(void)
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

WEAK void spi1_irq_handler(void)
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

WEAK void USART3_IRQHandler(void)
{	while(1) {}
}

WEAK void EXTI15_10_IRQHandler(void)
{	while(1) {}
}

WEAK void RTC_Alarm_IRQHandler(void)
{	while(1) {}
}

WEAK void usbwakeup_handle(void)
{	while(1) {}
}

