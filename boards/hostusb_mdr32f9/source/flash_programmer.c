#include <stdint.h>
#include <stddef.h>
#include <drivers.h>

uint32_t op_status = 0xDEADBEEF;

int32_t prepare_flash(uint32_t start_sector, size_t sectors_count);
int32_t erase_flash(uint32_t start_sector, size_t sectors_count, uint32_t sys_clk);
int32_t write_flash(uint32_t dst_addr, uint32_t src_addr, size_t size, uint32_t sys_clk);
void reset(void);

void program_flash(uint32_t src_addr, uint32_t dst_addr, size_t size)
{
	// Disable interrupts
    __asm volatile ("cpsid i");

    op_status = 0xA000B000;
    /* Enables the clock on EEPROM */
    RST_CLK_PCLKcmd(RST_CLK_PCLK_EEPROM, ENABLE);
    EEPROM_EraseAllPages(EEPROM_Main_Bank_Select);


    size_t words_count = size >> 2; /* Divide by 4b */

    if ((size & 0x3) != 0)
    	++words_count;

    size_t i = 0;
    uint32_t ret_val = 0;
    for (; i < words_count; i++) {

    	EEPROM_ProgramWord(dst_addr + (i << 2), EEPROM_Main_Bank_Select, *((uint32_t*)(src_addr + (i << 2))));

    	ret_val = EEPROM_ReadWord(dst_addr + (i << 2), EEPROM_Main_Bank_Select);

    	if (ret_val != (*((uint32_t*)(src_addr + (i << 2))))) {
    		op_status = 0xFFFFEEE1;
    		break;
    	}
    }
	reset();

	while (1) {
//		TODO: Signalize error
		;
	}
}

int32_t prepare_flash(uint32_t start_sector, size_t sectors_count)
{
	return 0;
}

int32_t erase_flash(uint32_t start_sector, size_t sectors_count, uint32_t sys_clk)
{
	return 0;
}

int32_t write_flash(uint32_t dst_addr, uint32_t src_addr, size_t size, uint32_t sys_clk)
{
	return 0;
}

void reset(void)
{
	SCB->AIRCR |= SCB_AIRCR_SYSRESETREQ_Msk;
}
