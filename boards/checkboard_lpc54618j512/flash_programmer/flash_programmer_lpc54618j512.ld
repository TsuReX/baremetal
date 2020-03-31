/*
 * GENERATED FILE - DO NOT EDIT
 * Copyright (c) 2008-2013 Code Red Technologies Ltd,
 * Copyright 2015, 2018-2019 NXP
 * (c) NXP Semiconductors 2013-2020
 * Generated linker script file for LPC54618J512
 * Created from linkscript.ldt by FMCreateLinkLibraries
 * Using Freemarker v2.3.23
 * MCUXpresso IDE v11.1.0 [Build 3209] [2019-12-12] on Mar 30, 2020 9:43:22 AM
 */

MEMORY
{
  /* Define each memory region */
	PROGRAM_FLASH (rx) : ORIGIN = 0x0, LENGTH = 0x80000 /* 512K bytes (alias Flash) */  
	SRAM (rwx) : ORIGIN = 0x20000000, LENGTH = 0x28000 /* 160K bytes (alias RAM) */
}

	/* Define a symbol for the top of each memory region */
/* __base_PROGRAM_FLASH = 0x0  ; /* PROGRAM_FLASH */
/* __top_PROGRAM_FLASH = 0x0 + 0x80000 ; /* 512K bytes */
/*__base_SRAM = 0x20000000  ; /* SRAM_UPPER */
/* __top_SRAM = 0x20000000 + 0x28000 ; /* 160K bytes */


ENTRY(main)

SECTIONS
{
	.text : ALIGN(4)
	{
		*(.isr_vector)
		*(.after_vectors*)
		*(.text*)
		*(.rodata .rodata .constdata)
	} > SRAM

	.data : ALIGN(4)
	{
		*(.data)
	} > SRAM

	.bss : ALIGN(4)
	{
		*(.bss)
	} > SRAM

	_StackSize = 0x1000;
	.stack ORIGIN(SRAM) + LENGTH(SRAM) - _StackSize - 0:  ALIGN(4)
    {
        _vStackBase = .;
        . = ALIGN(4);
        _vStackTop = . + _StackSize;
    } > SRAM

	/* ## Create checksum value (used in startup) ## */
	PROVIDE(__valid_user_code_checksum = 0 - 
                                         (_vStackTop 
                                         + (ResetISR + 1) 
                                         + (NMI_Handler + 1) 
                                         + (HardFault_Handler + 1) 
                                         + (( DEFINED(MemManage_Handler) ? MemManage_Handler : 0 ) + 1)   /* MemManage_Handler may not be defined */
                                         + (( DEFINED(BusFault_Handler) ? BusFault_Handler : 0 ) + 1)     /* BusFault_Handler may not be defined */
                                         + (( DEFINED(UsageFault_Handler) ? UsageFault_Handler : 0 ) + 1) /* UsageFault_Handler may not be defined */
                                         ) );

}