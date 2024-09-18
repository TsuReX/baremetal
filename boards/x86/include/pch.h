#ifndef __PCH__
#define __PCH__

#include <stdint.h>
#include <io.h>

// Definition for PCR base address (defined in PchReservedResources.h)
#define PCH_PCR_BASE_ADDRESS		0xF0000000
#define PCH_PCR_MMIO_SIZE		0x01000000
#define PCH_PCR_ADDRESS(Pid, Offset)	(PCH_PCR_BASE_ADDRESS | ((unsigned char) (Pid) << 16) | (unsigned short) (Offset))

// Intel C740 Emitsburg PCH
// Private Configuration Space Register Target Port IDs
//#define PID_DMI			0x88
//#define PID_ESPISPI		0x72

// Intel C620 Lewisburg PCH
// Private Configuration Space Register Target Port IDs
#define PID_DMI			0xEF
#define PID_ESPISPI		0xEE

// DMI Configuration Registers
#define R_PCH_DMI_PCR_LPCIOD	0x2770
#define R_PCH_DMI_PCR_LPCIOE	0x2774

#define PCI_IOPORT_INDEX	0xCF8
#define PCI_IOPORT_DATA		0xCFC

#define PCI_CF8_ADDR(Bus, Dev, Func, Off) (((Off) & 0xFF) | (((Func) & 0x07) << 8) | (((Dev) & 0x1F) << 11) | (((Bus) & 0xFF) << 16) | (1 << 31))

// LPC PCI Devices and Functions
#define LPC_PCI_BUS_NUM		0x00
#define LPC_PCI_DEV_NUM		0x1F
#define LPC_PCI_FUN_NUM		0x00

// P2SD PCI Devices and Functions
#define P2SB_PCI_BUS_NUM		0x00
#define P2SB_PCI_DEV_NUM		0x1F
#define P2SB_PCI_FUN_NUM		0x01

#define LPC_PCI_CF8_ADDR(Offset)	PCI_CF8_ADDR(LPC_PCI_BUS_NUM, LPC_PCI_DEV_NUM, LPC_PCI_FUN_NUM, Offset)
#define P2SB_PCI_CF8_ADDR(Offset)	PCI_CF8_ADDR(P2SB_PCI_BUS_NUM, P2SB_PCI_DEV_NUM, P2SB_PCI_FUN_NUM, Offset)

// P2SB Configuration Registers
#define R_P2SB_CFG_CMD			0x04
#define R_P2SB_CFG_BAR0			0x10

// PCI Command
#define B_P2SB_CFG_CMD_MSE		(1 << 1)

// LPC Configuration Registers
#define R_LPC_CFG_IOD			0x80
#define R_LPC_CFG_IOE			0x82

// I/O Decode Ranges
#define V_LPC_CFG_IOD_COMA_3F8		0x0
#define N_LPC_CFG_IOD_COMA		0x0
#define V_LPC_CFG_IOD_COMB_2F8		0x1
#define N_LPC_CFG_IOD_COMB		0x4

// I/O Enables
#define B_LPC_CFG_IOE_ME2		(1 << 13)
#define B_LPC_CFG_IOE_SE		(1 << 12)
#define B_LPC_CFG_IOE_ME1		(1 << 11)
#define B_LPC_CFG_IOE_KE		(1 << 10)
#define B_LPC_CFG_IOE_CBE		(1 << 1)
#define B_LPC_CFG_IOE_CAE		(1 << 0)

#endif // __PCH__