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

//    Key values
//    The key values are as follows:
//    ● RDPRT key = 0x00A5
//    ● KEY1 = 0x45670123
//    ● KEY2 = 0xCDEF89AB
//
//	Unlocking the Flash memory
//After reset, the FPEC block is protected. The FLASH_CR register is not accessible in write
//mode. An unlocking sequence should be written to the FLASH_KEYR register to open up
//the FPEC block. This sequence consists of two write cycles, where two key values (KEY1
//and KEY2) are written to the FLASH_KEYR address (refer to Section 2.3.1 for key values).
//Any wrong sequence locks up the FPEC block and FLASH_CR register until the next reset.
//Also a bus error is returned on a wrong key sequence. This is done after the first write cycle
//if KEY1 does not match, or during the second write cycle if KEY1 has been correctly written
//but KEY2 does not match. The FPEC block and FLASH_CR register can be locked by the
//user’s software by writing the LOCK bit of the FLASH_CR register to 1. In this case, the
//FPEC can be unlocked by writing the correct sequence of keys into FLASH_KEYR
//
//    Mass Erase
//    The Mass Erase command can be used to completely erase the user pages of the Flash
//    memory. The information block is unaffected by this procedure. The following sequence is
//    recommended:
//    ● Check that no Flash memory operation is ongoing by checking the BSY bit in the
//    FLASH_SR register
//    ● Set the MER bit in the FLASH_CR register
//    ● Set the STRT bit in the FLASH_CR register
//    ● Wait for the BSY bit to be reset
//    ● Read all the pages and verify
//
//	The main Flash memory programming sequence in standard mode is as follows:
//	● Check that no main Flash memory operation is ongoing by checking the BSY bit in the
//	FLASH_SR register.
//	● Set the PG bit in the FLASH_CR register.
//	● Perform the data write (half-word) at the desired address.
//	● Wait for the BSY bit to be reset.
//	● Read the programmed value and verify.
//


    op_status = 0xA000B000;
	reset();

//finish:
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
