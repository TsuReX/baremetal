#include "usb_regs.h"
#include "usb_lib.h"
#include "console.h"
#include "debug.h"

void _SetEPTxAddr(uint32_t ep_ind, uint16_t ep_tx_addr)
{
	uint16_t *ptr_ep_tx_addr = (uint16_t*)(PACKAGE_MEMORY_ADDR + (*BTABLE + ep_ind * 8) * 2);
	*ptr_ep_tx_addr = ep_tx_addr;
}

uint16_t _GetEPTxAddr(uint32_t ep_ind)
{
	uint16_t *ptr_ep_tx_addr = (uint16_t*)(PACKAGE_MEMORY_ADDR + (*BTABLE + ep_ind * 8) * 2);
	return *ptr_ep_tx_addr;
}

void _SetEPTxCount(uint32_t ep_ind, uint16_t ep_tx_count)
{
	uint16_t *ptr_ep_tx_count = (uint16_t*)(PACKAGE_MEMORY_ADDR + (*BTABLE + ep_ind * 8 + 2) * 2);
	*ptr_ep_tx_count = ep_tx_count;
}

uint16_t _GetEPTxCount(uint32_t ep_ind)
{
	uint16_t *ptr_ep_tx_count = (uint16_t*)(PACKAGE_MEMORY_ADDR + (*BTABLE + ep_ind * 8 + 2) * 2);
	return *ptr_ep_tx_count;
}

void _SetEPRxAddr(uint32_t ep_ind, uint16_t ep_rx_addr)
{
	uint16_t *ptr_ep_rx_addr = (uint16_t*)(PACKAGE_MEMORY_ADDR + (*BTABLE + ep_ind * 8 + 4) * 2);
	*ptr_ep_rx_addr = ep_rx_addr;
}

uint16_t _GetEPRxAddr(uint32_t ep_ind)
{
	uint16_t *ptr_ep_rx_addr = (uint16_t*)(PACKAGE_MEMORY_ADDR + (*BTABLE + ep_ind * 8 + 4) * 2);
	return *ptr_ep_rx_addr;
}

void _SetEPRxCount(uint32_t ep_ind, uint16_t ep_rx_count)
{
	uint16_t *ptr_ep_rx_count = (uint16_t*)(PACKAGE_MEMORY_ADDR + (*BTABLE + ep_ind * 8 + 6) * 2);

	/* TODO: USB Make calculation algorithm. */
	/* BLSIZE = 1 -> 32bytes, NUM_BLOCK = 1 -> 2, buffer size 2 * 32 = 64 bytes */
	*ptr_ep_rx_count = (1 << 15) | (1 << 10);
}

uint16_t _GetEPRxCount(uint32_t ep_ind)
{
	uint16_t *ptr_ep_rx_count = (uint16_t*)(PACKAGE_MEMORY_ADDR + (*BTABLE + ep_ind * 8 + 6) * 2);
	return *ptr_ep_rx_count & 0x3FF;
}

void pma_init(void)
{
	size_t pma_words_count = 256;
	uint16_t *ptr_pma = (uint16_t*)PACKAGE_MEMORY_ADDR;
	size_t word_ind = 0;

	for (; word_ind < pma_words_count; ++word_ind) {
		ptr_pma[word_ind * 2] = 0x0000;
	}
}

void pma_print(uint16_t offset_usb_word, uint16_t word_count)
{
	uint16_t *ptr_pma = (uint16_t*)(PACKAGE_MEMORY_ADDR + offset_usb_word * 2);
	size_t dword_ind = 0;
	printk(DEBUG, "PMA\r\n");
	for (; dword_ind < word_count; ++dword_ind) {
		printk(DEBUG, "0x%08lX:0x%04X\r\n", ((uint32_t)ptr_pma) + dword_ind * 4, ptr_pma[dword_ind * 2]);
	}
}
