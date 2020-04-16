#include <stdint.h>
#include <stddef.h>

#include <fsl_common.h>

#define IAP_BASE_ADDR		0x03000204
#define PREPARE_OPCODE		50
#define WRITE_OPCODE		51
#define ERASE_OPCODE		52

#define IAP_CMD_SUCCESS		0
#define IAP_INVALID_SECTOR	7
#define IAP_BUSY			11

typedef void (*iap_call)(uint32_t arguments[], uint32_t results[]);

uint32_t op_status = 0xDEADBEEF;

int32_t prepare_flash(uint32_t start_sector, size_t sectors_count);
int32_t erase_flash(uint32_t start_sector, size_t sectors_count, uint32_t sys_clk);
int32_t write_flash(uint32_t dst_addr, uint32_t src_addr, size_t size, uint32_t sys_clk);
void reset(void);

void program_flash(uint32_t src_addr, uint32_t dst_addr, size_t size)
{
	// Disable interrupts
    __asm volatile ("cpsid i");

    // Enable SRAM clock used by Stack
    __asm volatile ("LDR R0, =0x40000220\n\t"
                    "MOV R1, #56\n\t"
                    "STR R1, [R0]");

    if (size > (1 << 16)) {
    	op_status = 0xF01;
    	goto finish;
    }

    /* Writing should start from the beginning of the flash!!!!*/
    if (dst_addr != 0) {
		op_status = 0xF02;
		goto finish;
    }
    uint32_t sys_clk = 204000;
    uint32_t sectors_count = size >> 15; /* Divide by 32Kb */

    if (size & 0x7FFF != 0)
    	++sectors_count;

    if (prepare_flash(0, sectors_count) != 0)
		goto finish;

	if (erase_flash(0, sectors_count, sys_clk) != 0)
		goto finish;

    uint32_t blocks_count = (size - 1) >> 12; /* Divide by 4Kb */
    if (blocks_count == 0)
    	++blocks_count;

    uint32_t block_size = 0x1000;
	size_t i = 0;
    for(; i <= blocks_count; ++i) {
    	if (prepare_flash(0, sectors_count) != 0) {
			op_status |= (0xE000 + i) << 16 ;
    		goto finish;
    	}
		if (write_flash(dst_addr + i * block_size, src_addr + i * block_size, block_size, sys_clk) != 0) {
			op_status |= (0xD000 + i) << 16;
			goto finish;
		}
	}
    op_status = 0xA000B000;
	reset();

finish:
	while (1) {
//		TODO: Signalize error
		;
	}
}

int32_t prepare_flash(uint32_t start_sector, size_t sectors_count)
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

int32_t erase_flash(uint32_t start_sector, size_t sectors_count, uint32_t sys_clk)
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

int32_t write_flash(uint32_t dst_addr, uint32_t src_addr, size_t size, uint32_t sys_clk)
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

void reset(void)
{
	SCB->AIRCR |= SCB_AIRCR_SYSRESETREQ_Msk;
}
