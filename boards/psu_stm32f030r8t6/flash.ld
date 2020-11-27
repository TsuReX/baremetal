ENTRY(reset_handler)

MEMORY {
	SRAM (xrw)	: ORIGIN = 0x20000000, LENGTH = 0x1000
	PROGRAM_FLASH (rx)	: ORIGIN = 0x0, LENGTH = 16K
}

__flash_start__	= ORIGIN(PROGRAM_FLASH);
__flash_end__	= ORIGIN(PROGRAM_FLASH) + LENGTH(PROGRAM_FLASH);

__sram_start__	= ORIGIN(SRAM);
__sram_end__	= ORIGIN(SRAM) + LENGTH(SRAM);


SECTIONS
{
	.text : ALIGN(4) {

		__text_start__ = .;
		*(.isr_vector)
		*(.after_vectors*)
		*(.text*)
		*(.rodata .rodata .constdata)
		__text_end__ = .;
	
	} > PROGRAM_FLASH
		
	.data  :  ALIGN(4) {

		__data_load__ = LOADADDR (.data);
        __data_start__ = .;
		*(.data)
		__data_end__ = .;
	
	} > SRAM AT > PROGRAM_FLASH

	.bss : ALIGN(4) {

		__bss_start__ = .;
		*(.bss)
		PROVIDE(end = .);
		__bss_end__ = .;
	
	} > SRAM

	__stack_size__ 	= 0x0400;
	.stack ORIGIN(SRAM) + LENGTH(SRAM) - __stack_size__ :  ALIGN(4) {

		__stack_start__ = .;
		. = ALIGN(4);
		__stack_end__ = . + __stack_size__ - 4;
	
	} > SRAM

}


