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
	PROGRAM_FLASH (rx) : ORIGIN = 0x8000000, LENGTH = 0x00020000 /* 128K bytes (alias Flash) */  
	SRAM (rwx) : ORIGIN = 0x20000000, LENGTH = 0x00008000 /* 32K bytes (alias RAM) */
}

ENTRY(program_flash)

SECTIONS
{	
	.text : ALIGN(4) 
	{
		*(.text*)
		*(.rodata .rodata .constdata)
	} > SRAM

	.data : ALIGN(0x800)
	{
		*(.data)
	} > SRAM

	.bss : ALIGN(0x800)
	{
		*(.bss)
	} > SRAM

}