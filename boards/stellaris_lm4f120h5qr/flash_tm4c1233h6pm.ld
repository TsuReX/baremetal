/******************************************************************************
 *
 * Default Linker script for the Texas Instruments SC4C1290KCPDT
 *
 * This is derived from revision 15071 of the TivaWare Library.
 *
 *****************************************************************************/

MEMORY
{
    PROGRAM_FLASH (RX) : ORIGIN = 0x00000000, LENGTH = 0x00040000
    SRAM (WX)  : ORIGIN = 0x20000000, LENGTH = 0x00008000
}
	/* Define a symbol for the top of each memory region */
__flash_start__	= ORIGIN(PROGRAM_FLASH); /* PROGRAM_FLASH */
__flash_end__	= ORIGIN(PROGRAM_FLASH) + LENGTH(PROGRAM_FLASH); /* 256K bytes */

__sram_start__	= ORIGIN(SRAM); /* SRAM_UPPER */
__sram_end__	= ORIGIN(SRAM) + LENGTH(SRAM); /* 32K bytes */


ENTRY(reset_handler)

SECTIONS
{
	.text : ALIGN(4)
	{
		__text_start__ = .;
		*(.isr_vector)
		*(.after_vectors*)
		*(.text*)
		*(.rodata .rodata .constdata)
		__text_end__ = .;
	} > PROGRAM_FLASH
		
	.data  :  ALIGN(4)
	{
		__data_load__ = LOADADDR (.data);
        __data_start__ = .;
		*(.data)
		__data_end__ = .;
	} > SRAM AT > PROGRAM_FLASH

	.bss : ALIGN(4)
	{
		__bss_start__ = .;
		*(.bss)
		PROVIDE(end = .);
		__bss_end__ = .;
	} > SRAM

	__stack_size__ = 0x1000;
	.stack :  ALIGN(1024)
	{
		__stack_start__ = .;
		. = ALIGN(4);
		__stack_end__ = . + __stack_size__;
	} > SRAM

}
