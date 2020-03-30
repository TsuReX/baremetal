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
	SRAM_UPPER (rwx) : ORIGIN = 0x20000000, LENGTH = 0x28000 /* 160K bytes (alias RAM) */  
}

  /* Define a symbol for the top of each memory region */
__base_PROGRAM_FLASH = 0x0  ; /* PROGRAM_FLASH */  
__base_Flash = 0x0 ; /* Flash */  
__top_PROGRAM_FLASH = 0x0 + 0x80000 ; /* 512K bytes */  
__top_Flash = 0x0 + 0x80000 ; /* 512K bytes */  

__base_SRAM_UPPER = 0x20000000  ; /* SRAM_UPPER */  
__base_RAM = 0x20000000 ; /* RAM */  
__top_SRAM_UPPER = 0x20000000 + 0x28000 ; /* 160K bytes */  
__top_RAM = 0x20000000 + 0x28000 ; /* 160K bytes */  

ENTRY(main)

SECTIONS
{
	. = __base_SRAM_UPPER;
    .text : ALIGN(4)
    {
       *(.text*)
       *(.rodata .rodata .constdata)
       . = ALIGN(4);
    } > SRAM_UPPER
	
	. = 1024;
    
    .data : ALIGN(4)
    {
        *(.data)

     } > SRAM_UPPER AT > PROGRAM_FLASH
	
	. = 2048;
    
    .bss : ALIGN(4)
    {
       *(.bss)

    } > SRAM_UPPER AT > PROGRAM_FLASH
}