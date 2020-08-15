#include <usb_lib.h>

void _SetEPTxAddr(uint32_t bEpNum, uint16_t wAddr)
{
	uint16_t *ep_tx_addr = (uint16_t*)(PMAAddr + (*BTABLE + bEpNum * 8) * 2);
	*ep_tx_addr = wAddr & 0xFE;
}

uint16_t _GetEPTxAddr(uint32_t bEpNum)
{
	uint16_t *ep_tx_addr = (uint16_t*)(PMAAddr + (*BTABLE + bEpNum * 8) * 2);
	return *ep_tx_addr;
}

void _SetEPTxCount(uint32_t bEpNum, uint16_t wCount)
{
	uint16_t *ep_tx_count = (uint16_t*)(PMAAddr + (*BTABLE + bEpNum * 8 + 2) * 2);
	*ep_tx_count = wCount;
}

uint16_t _GetEPTxCount(uint32_t bEpNum)
{
	uint16_t *ep_tx_count = (uint16_t*)(PMAAddr + (*BTABLE + bEpNum * 8 + 2) * 2);
	return *ep_tx_count;
}

void _SetEPRxAddr(uint32_t bEpNum, uint16_t wAddr)
{
	uint16_t *ep_rx_addr = (uint16_t*)(PMAAddr + (*BTABLE + bEpNum * 8 + 4) * 2);
	*ep_rx_addr = wAddr & 0xFE;
}

uint16_t _GetEPRxAddr(uint32_t bEpNum)
{
	uint16_t *ep_rx_addr = (uint16_t*)(PMAAddr + (*BTABLE + bEpNum * 8 + 4) * 2);
	return *ep_rx_addr;
}

void _SetEPRxCount(uint32_t bEpNum, uint16_t wCount)
{
	uint16_t *ep_rx_count = (uint16_t*)(PMAAddr + (*BTABLE + bEpNum * 8 + 6) * 2);
//	uint16_t wNBlocks;
//
//	if (wCount > 32) {
//		wNBlocks = wCount >> 5;
//
//		if ((wCount & 0x1f) == 0) {
//			wNBlocks--;
//		}
//
//		*ep_rx_count = (uint32_t)((wNBlocks << 10) | 0x8000);
//
//	} else {
//		wNBlocks = wCount >> 1;
//
//		if((wCount & 0x1) != 0) {
//			wNBlocks++;
//		}
//
//		*ep_rx_count = (uint32_t)(wNBlocks << 10);
//	}
	*ep_rx_count = wCount;
}

uint16_t _GetEPRxCount(uint32_t bEpNum)
{
	uint16_t *ep_rx_count = (uint16_t*)(PMAAddr + (*BTABLE + bEpNum * 8 + 6) * 2);
	return *ep_rx_count;
}
