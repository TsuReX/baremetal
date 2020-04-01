#include <stdint.h>
#include <stddef.h>
#include "crp.h"

#define WEAK __attribute__ ((weak))
#define WEAK_AV __attribute__ ((weak, section(".after_vectors")))
#define ALIAS(f) __attribute__ ((weak, alias (#f)))

//*****************************************************************************
// Variable to store CRP value in. Will be placed automatically
// by the linker when "Enable Code Read Protect" selected.
// See crp.h header for more information
//*****************************************************************************
__CRP const unsigned int CRP_WORD = CRP_NO_CRP ;

#define IAP_BASE_ADDR		0x03000204
#define PREPARE_OPCODE		50
#define WRITE_OPCODE		51
#define ERASE_OPCODE		52

#define IAP_CMD_SUCCESS		0
#define IAP_INVALID_SECTOR	7
#define IAP_BUSY			11

typedef void (*iap_call)(uint32_t arguments[], uint32_t results[]);

uint32_t op_status = 0;

extern int main();

//*****************************************************************************
// Forward declaration of the core exception handlers.
// When the application defines a handler (with the same name), this will
// automatically take precedence over these weak definitions.
// If your application is a C++ one, then any interrupt handlers defined
// in C++ files within in your main application will need to have C linkage
// rather than C++ linkage. To do this, make sure that you are using extern "C"
// { .... } around the interrupt handler within your main application code.
//*****************************************************************************
     void ResetISR(void);
WEAK void NMI_Handler(void);
WEAK void HardFault_Handler(void);
WEAK void MemManage_Handler(void);
WEAK void BusFault_Handler(void);
WEAK void UsageFault_Handler(void);
WEAK void SVC_Handler(void);
WEAK void DebugMon_Handler(void);
WEAK void PendSV_Handler(void);
WEAK void SysTick_Handler(void);
WEAK void IntDefaultHandler(void);

//*****************************************************************************
// Forward declaration of the application IRQ handlers. When the application
// defines a handler (with the same name), this will automatically take
// precedence over weak definitions below
//*****************************************************************************
WEAK void WDT_BOD_IRQHandler(void);
WEAK void DMA0_IRQHandler(void);
WEAK void GINT0_IRQHandler(void);
WEAK void GINT1_IRQHandler(void);
WEAK void PIN_INT0_IRQHandler(void);
WEAK void PIN_INT1_IRQHandler(void);
WEAK void PIN_INT2_IRQHandler(void);
WEAK void PIN_INT3_IRQHandler(void);
WEAK void UTICK0_IRQHandler(void);
WEAK void MRT0_IRQHandler(void);
WEAK void CTIMER0_IRQHandler(void);
WEAK void CTIMER1_IRQHandler(void);
WEAK void SCT0_IRQHandler(void);
WEAK void CTIMER3_IRQHandler(void);
WEAK void FLEXCOMM0_IRQHandler(void);
WEAK void FLEXCOMM1_IRQHandler(void);
WEAK void FLEXCOMM2_IRQHandler(void);
WEAK void FLEXCOMM3_IRQHandler(void);
WEAK void FLEXCOMM4_IRQHandler(void);
WEAK void FLEXCOMM5_IRQHandler(void);
WEAK void FLEXCOMM6_IRQHandler(void);
WEAK void FLEXCOMM7_IRQHandler(void);
WEAK void ADC0_SEQA_IRQHandler(void);
WEAK void ADC0_SEQB_IRQHandler(void);
WEAK void ADC0_THCMP_IRQHandler(void);
WEAK void DMIC0_IRQHandler(void);
WEAK void HWVAD0_IRQHandler(void);
WEAK void USB0_NEEDCLK_IRQHandler(void);
WEAK void USB0_IRQHandler(void);
WEAK void RTC_IRQHandler(void);
WEAK void Reserved46_IRQHandler(void);
WEAK void Reserved47_IRQHandler(void);
WEAK void PIN_INT4_IRQHandler(void);
WEAK void PIN_INT5_IRQHandler(void);
WEAK void PIN_INT6_IRQHandler(void);
WEAK void PIN_INT7_IRQHandler(void);
WEAK void CTIMER2_IRQHandler(void);
WEAK void CTIMER4_IRQHandler(void);
WEAK void RIT_IRQHandler(void);
WEAK void SPIFI0_IRQHandler(void);
WEAK void FLEXCOMM8_IRQHandler(void);
WEAK void FLEXCOMM9_IRQHandler(void);
WEAK void SDIO_IRQHandler(void);
WEAK void CAN0_IRQ0_IRQHandler(void);
WEAK void CAN0_IRQ1_IRQHandler(void);
WEAK void CAN1_IRQ0_IRQHandler(void);
WEAK void CAN1_IRQ1_IRQHandler(void);
WEAK void USB1_IRQHandler(void);
WEAK void USB1_NEEDCLK_IRQHandler(void);
WEAK void ETHERNET_IRQHandler(void);
WEAK void ETHERNET_PMT_IRQHandler(void);
WEAK void ETHERNET_MACLP_IRQHandler(void);
WEAK void EEPROM_IRQHandler(void);
WEAK void LCD_IRQHandler(void);
WEAK void SHA_IRQHandler(void);
WEAK void SMARTCARD0_IRQHandler(void);
WEAK void SMARTCARD1_IRQHandler(void);

//*****************************************************************************
// Forward declaration of the driver IRQ handlers. These are aliased
// to the IntDefaultHandler, which is a 'forever' loop. When the driver
// defines a handler (with the same name), this will automatically take
// precedence over these weak definitions
//*****************************************************************************
void WDT_BOD_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
void DMA0_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
void GINT0_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
void GINT1_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
void PIN_INT0_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
void PIN_INT1_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
void PIN_INT2_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
void PIN_INT3_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
void UTICK0_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
void MRT0_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
void CTIMER0_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
void CTIMER1_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
void SCT0_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
void CTIMER3_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
void FLEXCOMM0_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
void FLEXCOMM1_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
void FLEXCOMM2_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
void FLEXCOMM3_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
void FLEXCOMM4_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
void FLEXCOMM5_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
void FLEXCOMM6_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
void FLEXCOMM7_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
void ADC0_SEQA_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
void ADC0_SEQB_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
void ADC0_THCMP_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
void DMIC0_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
void HWVAD0_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
void USB0_NEEDCLK_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
void USB0_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
void RTC_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
void Reserved46_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
void Reserved47_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
void PIN_INT4_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
void PIN_INT5_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
void PIN_INT6_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
void PIN_INT7_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
void CTIMER2_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
void CTIMER4_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
void RIT_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
void SPIFI0_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
void FLEXCOMM8_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
void FLEXCOMM9_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
void SDIO_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
void CAN0_IRQ0_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
void CAN0_IRQ1_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
void CAN1_IRQ0_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
void CAN1_IRQ1_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
void USB1_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
void USB1_NEEDCLK_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
void ETHERNET_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
void ETHERNET_PMT_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
void ETHERNET_MACLP_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
void EEPROM_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
void LCD_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
void SHA_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
void SMARTCARD0_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
void SMARTCARD1_DriverIRQHandler(void) ALIAS(IntDefaultHandler);

//*****************************************************************************
// External declaration for the pointer to the stack top from the Linker Script
//*****************************************************************************
extern void _vStackTop(void);

//*****************************************************************************
// External declaration for LPC MCU vector table checksum from  Linker Script
//*****************************************************************************
WEAK extern void __valid_user_code_checksum();

//*****************************************************************************
// The vector table.
// This relies on the linker script to place at correct location in memory.
//*****************************************************************************
extern void (* const g_pfnVectors[])(void);

__attribute__ ((used, section(".isr_vector")))
void (* const g_pfnVectors[])(void) = {
    // Core Level - CM4
    &_vStackTop,                       // The initial stack pointer
    ResetISR,                          // The reset handler
    NMI_Handler,                       // The NMI handler
    HardFault_Handler,                 // The hard fault handler
    MemManage_Handler,                 // The MPU fault handler
    BusFault_Handler,                  // The bus fault handler
    UsageFault_Handler,                // The usage fault handler
    __valid_user_code_checksum,        // LPC MCU checksum
    0,                                 // ECRP
    0,                                 // Reserved
    0,                                 // Reserved
    SVC_Handler,                       // SVCall handler
    DebugMon_Handler,                  // Debug monitor handler
    0,                                 // Reserved
    PendSV_Handler,                    // The PendSV handler
    SysTick_Handler,                   // The SysTick handler

    // Chip Level - LPC54618
    WDT_BOD_IRQHandler,         // 16: Windowed watchdog timer, Brownout detect
    DMA0_IRQHandler,            // 17: DMA controller
    GINT0_IRQHandler,           // 18: GPIO group 0
    GINT1_IRQHandler,           // 19: GPIO group 1
    PIN_INT0_IRQHandler,        // 20: Pin interrupt 0 or pattern match engine slice 0
    PIN_INT1_IRQHandler,        // 21: Pin interrupt 1or pattern match engine slice 1
    PIN_INT2_IRQHandler,        // 22: Pin interrupt 2 or pattern match engine slice 2
    PIN_INT3_IRQHandler,        // 23: Pin interrupt 3 or pattern match engine slice 3
    UTICK0_IRQHandler,          // 24: Micro-tick Timer
    MRT0_IRQHandler,            // 25: Multi-rate timer
    CTIMER0_IRQHandler,         // 26: Standard counter/timer CTIMER0
    CTIMER1_IRQHandler,         // 27: Standard counter/timer CTIMER1
    SCT0_IRQHandler,            // 28: SCTimer/PWM
    CTIMER3_IRQHandler,         // 29: Standard counter/timer CTIMER3
    FLEXCOMM0_IRQHandler,       // 30: Flexcomm Interface 0 (USART, SPI, I2C, FLEXCOMM)
    FLEXCOMM1_IRQHandler,       // 31: Flexcomm Interface 1 (USART, SPI, I2C, FLEXCOMM)
    FLEXCOMM2_IRQHandler,       // 32: Flexcomm Interface 2 (USART, SPI, I2C, FLEXCOMM)
    FLEXCOMM3_IRQHandler,       // 33: Flexcomm Interface 3 (USART, SPI, I2C, FLEXCOMM)
    FLEXCOMM4_IRQHandler,       // 34: Flexcomm Interface 4 (USART, SPI, I2C, FLEXCOMM)
    FLEXCOMM5_IRQHandler,       // 35: Flexcomm Interface 5 (USART, SPI, I2C,, FLEXCOMM)
    FLEXCOMM6_IRQHandler,       // 36: Flexcomm Interface 6 (USART, SPI, I2C, I2S,, FLEXCOMM)
    FLEXCOMM7_IRQHandler,       // 37: Flexcomm Interface 7 (USART, SPI, I2C, I2S,, FLEXCOMM)
    ADC0_SEQA_IRQHandler,       // 38: ADC0 sequence A completion.
    ADC0_SEQB_IRQHandler,       // 39: ADC0 sequence B completion.
    ADC0_THCMP_IRQHandler,      // 40: ADC0 threshold compare and error.
    DMIC0_IRQHandler,           // 41: Digital microphone and DMIC subsystem
    HWVAD0_IRQHandler,          // 42: Hardware Voice Activity Detector
    USB0_NEEDCLK_IRQHandler,    // 43: USB Activity Wake-up Interrupt
    USB0_IRQHandler,            // 44: USB device
    RTC_IRQHandler,             // 45: RTC alarm and wake-up interrupts
    Reserved46_IRQHandler,      // 46: Reserved interrupt
    Reserved47_IRQHandler,      // 47: Reserved interrupt
    PIN_INT4_IRQHandler,        // 48: Pin interrupt 4 or pattern match engine slice 4 int
    PIN_INT5_IRQHandler,        // 49: Pin interrupt 5 or pattern match engine slice 5 int
    PIN_INT6_IRQHandler,        // 50: Pin interrupt 6 or pattern match engine slice 6 int
    PIN_INT7_IRQHandler,        // 51: Pin interrupt 7 or pattern match engine slice 7 int
    CTIMER2_IRQHandler,         // 52: Standard counter/timer CTIMER2
    CTIMER4_IRQHandler,         // 53: Standard counter/timer CTIMER4
    RIT_IRQHandler,             // 54: Repetitive Interrupt Timer
    SPIFI0_IRQHandler,          // 55: SPI flash interface
    FLEXCOMM8_IRQHandler,       // 56: Flexcomm Interface 8 (USART, SPI, I2C, FLEXCOMM)
    FLEXCOMM9_IRQHandler,       // 57: Flexcomm Interface 9 (USART, SPI, I2C, FLEXCOMM)
    SDIO_IRQHandler,            // 58: SD/MMC
    CAN0_IRQ0_IRQHandler,       // 59: CAN0 interrupt0
    CAN0_IRQ1_IRQHandler,       // 60: CAN0 interrupt1
    CAN1_IRQ0_IRQHandler,       // 61: CAN1 interrupt0
    CAN1_IRQ1_IRQHandler,       // 62: CAN1 interrupt1
    USB1_IRQHandler,            // 63: USB1 interrupt
    USB1_NEEDCLK_IRQHandler,    // 64: USB1 activity
    ETHERNET_IRQHandler,        // 65: Ethernet
    ETHERNET_PMT_IRQHandler,    // 66: Ethernet power management interrupt
    ETHERNET_MACLP_IRQHandler,  // 67: Ethernet MAC interrupt
    EEPROM_IRQHandler,          // 68: EEPROM interrupt
    LCD_IRQHandler,             // 69: LCD interrupt
    SHA_IRQHandler,             // 70: SHA interrupt
    SMARTCARD0_IRQHandler,      // 71: Smart card 0 interrupt
    SMARTCARD1_IRQHandler,      // 72: Smart card 1 interrupt

}; /* End of g_pfnVectors */

//*****************************************************************************
// Reset entry point for your code.
// Sets up a simple runtime environment and initializes the C/C++
// library.
//*****************************************************************************
__attribute__ ((section(".after_vectors.reset")))
void ResetISR(void) {

    // Disable interrupts
    __asm volatile ("cpsid i");


    // Enable SRAM clock used by Stack
    __asm volatile ("LDR R0, =0x40000220\n\t"
                    "MOV R1, #56\n\t"
                    "STR R1, [R0]");

#if defined (__USE_CMSIS)
// If __USE_CMSIS defined, then call CMSIS SystemInit code
    SystemInit();

#endif // (__USE_CMSIS)

    // Check to see if we are running the code from a non-zero
    // address (eg RAM, external flash), in which case we need
    // to modify the VTOR register to tell the CPU that the
    // vector table is located at a non-0x0 address.
    unsigned int * pSCB_VTOR = (unsigned int *) 0xE000ED08;
    if ((unsigned int *)g_pfnVectors!=(unsigned int *) 0x00000000) {
        *pSCB_VTOR = (unsigned int)g_pfnVectors;
    }

    // Reenable interrupts
    __asm volatile ("cpsie i");

    main();

    //
    // main() shouldn't return, but if it does, we'll just enter an infinite loop
    //
    while (1) {
        ;
    }
}

//*****************************************************************************
// Default core exception handlers. Override the ones here by defining your own
// handler routines in your application code.
//*****************************************************************************
WEAK_AV void NMI_Handler(void)
{ while(1) {}
}

WEAK_AV void HardFault_Handler(void)
{ while(1) {}
}

WEAK_AV void MemManage_Handler(void)
{ while(1) {}
}

WEAK_AV void BusFault_Handler(void)
{ while(1) {}
}

WEAK_AV void UsageFault_Handler(void)
{ while(1) {}
}

WEAK_AV void SVC_Handler(void)
{ while(1) {}
}

WEAK_AV void DebugMon_Handler(void)
{ while(1) {}
}

WEAK_AV void PendSV_Handler(void)
{ while(1) {}
}

WEAK_AV void SysTick_Handler(void)
{ while(1) {}
}

//*****************************************************************************
// Processor ends up here if an unexpected interrupt occurs or a specific
// handler is not present in the application code.
//*****************************************************************************
WEAK_AV void IntDefaultHandler(void)
{ while(1) {}
}

//*****************************************************************************
// Default application exception handlers. Override the ones here by defining
// your own handler routines in your application code. These routines call
// driver exception handlers or IntDefaultHandler() if no driver exception
// handler is included.
//*****************************************************************************
WEAK void WDT_BOD_IRQHandler(void)
{   WDT_BOD_DriverIRQHandler();
}

WEAK void DMA0_IRQHandler(void)
{   DMA0_DriverIRQHandler();
}

WEAK void GINT0_IRQHandler(void)
{   GINT0_DriverIRQHandler();
}

WEAK void GINT1_IRQHandler(void)
{   GINT1_DriverIRQHandler();
}

WEAK void PIN_INT0_IRQHandler(void)
{   PIN_INT0_DriverIRQHandler();
}

WEAK void PIN_INT1_IRQHandler(void)
{   PIN_INT1_DriverIRQHandler();
}

WEAK void PIN_INT2_IRQHandler(void)
{   PIN_INT2_DriverIRQHandler();
}

WEAK void PIN_INT3_IRQHandler(void)
{   PIN_INT3_DriverIRQHandler();
}

WEAK void UTICK0_IRQHandler(void)
{   UTICK0_DriverIRQHandler();
}

WEAK void MRT0_IRQHandler(void)
{   MRT0_DriverIRQHandler();
}

WEAK void CTIMER0_IRQHandler(void)
{   CTIMER0_DriverIRQHandler();
}

WEAK void CTIMER1_IRQHandler(void)
{   CTIMER1_DriverIRQHandler();
}

WEAK void SCT0_IRQHandler(void)
{   SCT0_DriverIRQHandler();
}

WEAK void CTIMER3_IRQHandler(void)
{   CTIMER3_DriverIRQHandler();
}

WEAK void FLEXCOMM0_IRQHandler(void)
{   FLEXCOMM0_DriverIRQHandler();
}

WEAK void FLEXCOMM1_IRQHandler(void)
{   FLEXCOMM1_DriverIRQHandler();
}

WEAK void FLEXCOMM2_IRQHandler(void)
{   FLEXCOMM2_DriverIRQHandler();
}

WEAK void FLEXCOMM3_IRQHandler(void)
{   FLEXCOMM3_DriverIRQHandler();
}

WEAK void FLEXCOMM4_IRQHandler(void)
{   FLEXCOMM4_DriverIRQHandler();
}

WEAK void FLEXCOMM5_IRQHandler(void)
{   FLEXCOMM5_DriverIRQHandler();
}

WEAK void FLEXCOMM6_IRQHandler(void)
{   FLEXCOMM6_DriverIRQHandler();
}

WEAK void FLEXCOMM7_IRQHandler(void)
{   FLEXCOMM7_DriverIRQHandler();
}

WEAK void ADC0_SEQA_IRQHandler(void)
{   ADC0_SEQA_DriverIRQHandler();
}

WEAK void ADC0_SEQB_IRQHandler(void)
{   ADC0_SEQB_DriverIRQHandler();
}

WEAK void ADC0_THCMP_IRQHandler(void)
{   ADC0_THCMP_DriverIRQHandler();
}

WEAK void DMIC0_IRQHandler(void)
{   DMIC0_DriverIRQHandler();
}

WEAK void HWVAD0_IRQHandler(void)
{   HWVAD0_DriverIRQHandler();
}

WEAK void USB0_NEEDCLK_IRQHandler(void)
{   USB0_NEEDCLK_DriverIRQHandler();
}

WEAK void USB0_IRQHandler(void)
{   USB0_DriverIRQHandler();
}

WEAK void RTC_IRQHandler(void)
{   RTC_DriverIRQHandler();
}

WEAK void Reserved46_IRQHandler(void)
{   Reserved46_DriverIRQHandler();
}

WEAK void Reserved47_IRQHandler(void)
{   Reserved47_DriverIRQHandler();
}

WEAK void PIN_INT4_IRQHandler(void)
{   PIN_INT4_DriverIRQHandler();
}

WEAK void PIN_INT5_IRQHandler(void)
{   PIN_INT5_DriverIRQHandler();
}

WEAK void PIN_INT6_IRQHandler(void)
{   PIN_INT6_DriverIRQHandler();
}

WEAK void PIN_INT7_IRQHandler(void)
{   PIN_INT7_DriverIRQHandler();
}

WEAK void CTIMER2_IRQHandler(void)
{   CTIMER2_DriverIRQHandler();
}

WEAK void CTIMER4_IRQHandler(void)
{   CTIMER4_DriverIRQHandler();
}

WEAK void RIT_IRQHandler(void)
{   RIT_DriverIRQHandler();
}

WEAK void SPIFI0_IRQHandler(void)
{   SPIFI0_DriverIRQHandler();
}

WEAK void FLEXCOMM8_IRQHandler(void)
{   FLEXCOMM8_DriverIRQHandler();
}

WEAK void FLEXCOMM9_IRQHandler(void)
{   FLEXCOMM9_DriverIRQHandler();
}

WEAK void SDIO_IRQHandler(void)
{   SDIO_DriverIRQHandler();
}

WEAK void CAN0_IRQ0_IRQHandler(void)
{   CAN0_IRQ0_DriverIRQHandler();
}

WEAK void CAN0_IRQ1_IRQHandler(void)
{   CAN0_IRQ1_DriverIRQHandler();
}

WEAK void CAN1_IRQ0_IRQHandler(void)
{   CAN1_IRQ0_DriverIRQHandler();
}

WEAK void CAN1_IRQ1_IRQHandler(void)
{   CAN1_IRQ1_DriverIRQHandler();
}

WEAK void USB1_IRQHandler(void)
{   USB1_DriverIRQHandler();
}

WEAK void USB1_NEEDCLK_IRQHandler(void)
{   USB1_NEEDCLK_DriverIRQHandler();
}

WEAK void ETHERNET_IRQHandler(void)
{   ETHERNET_DriverIRQHandler();
}

WEAK void ETHERNET_PMT_IRQHandler(void)
{   ETHERNET_PMT_DriverIRQHandler();
}

WEAK void ETHERNET_MACLP_IRQHandler(void)
{   ETHERNET_MACLP_DriverIRQHandler();
}

WEAK void EEPROM_IRQHandler(void)
{   EEPROM_DriverIRQHandler();
}

WEAK void LCD_IRQHandler(void)
{   LCD_DriverIRQHandler();
}

WEAK void SHA_IRQHandler(void)
{   SHA_DriverIRQHandler();
}

WEAK void SMARTCARD0_IRQHandler(void)
{   SMARTCARD0_DriverIRQHandler();
}

WEAK void SMARTCARD1_IRQHandler(void)
{   SMARTCARD1_DriverIRQHandler();
}

static int32_t prepare_flash(uint32_t start_sector, size_t sectors_count)
{
	iap_call	prepare = (iap_call)(IAP_BASE_ADDR | 0x1);
	uint32_t	prep_args[] = {PREPARE_OPCODE, start_sector, start_sector + sectors_count - 1};
	uint32_t	prep_res[] = {0xFFFFFFFF};

	prepare(prep_args, prep_res);
	if (prep_res[0] != IAP_CMD_SUCCESS) {
		op_status = prep_res[0];
		return -1;
	}

	return 0;
}

static int32_t erase_flash(uint32_t start_sector, size_t sectors_count, uint32_t sys_clk)
{
	iap_call	erase = (iap_call)(IAP_BASE_ADDR | 0x1);
	uint32_t	erase_args[] = {ERASE_OPCODE, start_sector, start_sector + sectors_count - 1, sys_clk};
	uint32_t	erase_res[] = {0xFFFFFFFF};

	erase(erase_args, erase_res);
	if (erase_res[0] != IAP_CMD_SUCCESS) {
		op_status = erase_res[0];
		return -1;
	}

	return 0;
}

static int32_t write_flash(uint32_t dst_addr, uint32_t src_addr, size_t size, uint32_t sys_clk)
{
	iap_call	write = (iap_call)(IAP_BASE_ADDR | 0x1);
	uint32_t	write_args[] = {WRITE_OPCODE, dst_addr, src_addr, size, sys_clk};
	uint32_t	write_res[] = {0};

	if (size & 0xFF != 0)
		return -2;

	if (size > 4096)
		return -3;

	write(write_args, write_res);
	if (write_res[0] != IAP_CMD_SUCCESS) {
		op_status = write_res[0];
		return -1;
	}
	return 0;
}

static void program_flash(uint32_t src_addr, uint32_t dst_addr, size_t size)
{
	// Disable interrupts
    __asm volatile ("cpsid i");


    // Enable SRAM clock used by Stack
    __asm volatile ("LDR R0, =0x40000220\n\t"
                    "MOV R1, #56\n\t"
                    "STR R1, [R0]");

    //SystemInit();

    // Check to see if we are running the code from a non-zero
    // address (eg RAM, external flash), in which case we need
    // to modify the VTOR register to tell the CPU that the
    // vector table is located at a non-0x0 address.
    unsigned int * pSCB_VTOR = (unsigned int *) 0xE000ED08;
    if ((unsigned int *)g_pfnVectors!=(unsigned int *) 0x00000000) {
        *pSCB_VTOR = (unsigned int)g_pfnVectors;
    }
    // Reenable interrupts
    __asm volatile ("cpsie i");

    uint32_t sys_clk = 204000;

    if (prepare_flash(0, 1) != 0)
		goto finish;

	if (erase_flash(0, 1, sys_clk) != 0)
		goto finish;

    if (prepare_flash(0, 1) != 0)
		goto finish;

	if (write_flash(dst_addr, src_addr, size, sys_clk) != 0)
		goto finish;
finish:
	while (1);
}

int main()
{

	return 0;
}

