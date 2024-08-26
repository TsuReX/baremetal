#include <ast2500.h>
#include <io.h>

typedef struct {
    unsigned char   Index;
    unsigned char   Data;
} SIO_REG_TABLE;

static  SIO_REG_TABLE  mASPEED2500Table [] = {
    { REG_LOGICAL_DEVICE,       ASPEED2500_SIO_UART1 },
    { ACTIVATE,                 0x01 },
    { PRIMARY_INTERRUPT_SELECT, 0x04 },    // COMA IRQ routing
    { INTERRUPT_TYPE,			0x01 },    // COMA Interrupt Type
    { REG_LOGICAL_DEVICE,       ASPEED2500_SIO_UART2 },
    { ACTIVATE,                 0x01 },
    { PRIMARY_INTERRUPT_SELECT, 0x03 },    // COMB IRQ routing
    { INTERRUPT_TYPE,			0x01 }     // COMB Interrupt Type
};

unsigned int is_aspeed_present(void) {
    unsigned int PresenceStatus = 0;


    //ASPEED AST2500/AST2600
    outb(ASPEED2500_SIO_UNLOCK, ASPEED2500_SIO_INDEX_PORT);
    outb(ASPEED2500_SIO_UNLOCK, ASPEED2500_SIO_INDEX_PORT);
    outb(REG_LOGICAL_DEVICE, ASPEED2500_SIO_INDEX_PORT);
    outb(ASPEED2500_SIO_UART1, ASPEED2500_SIO_DATA_PORT);

    if (inb(ASPEED2500_SIO_DATA_PORT) == ASPEED2500_SIO_UART1) {
    //
    //There is a Aspeed card need to support as well. it's type is AST2500 A1 EVB.
    //
    //AST2300-A0 0x01000003
    //AST2300-A1 0x01010303
    //AST1300-A1 0x01010003
    //AST1050-A1 0x01010203
    //AST2400-A0 0x02000303
    //AST2400-A1 0x02010303
    //AST1400-A1 0x02010103
    //AST1250-A1 0x02010303
    //AST2500-A0 0x04000303
    //AST2510-A0 0x04000103
    //AST2520-A0 0x04000203
    //AST2530-A0 0x04000403
    //AST2500-A1 0x04010303
    //AST2510-A1 0x04010103
    //AST2520-A1 0x04010203
    //AST2530-A1 0x04010403

    //if ((ReadAHBDword (SCU7C) & 0xff0000ff) == 0x04000003) {
    //PresenceStatus = 1;
    //}

    PresenceStatus = 1;
}

    outb(ASPEED2500_SIO_LOCK, ASPEED2500_SIO_INDEX_PORT);
    return PresenceStatus;
}

unsigned int is_sio_exist (void) {
    unsigned int   SioExist = 0;

    if (is_aspeed_present ()) {
    SioExist |= ASPEED_EXIST;
    }

//	if (IsNuvotonPresent ()) {
//		SioExist |= NCT5104D_EXIST;
//	}

    return SioExist;
}

void sio_ast2500_init (unsigned int ComBase) {
    unsigned int  SioExist;
    unsigned int  SioEnable;
    unsigned int  Index;
    unsigned int  Decode;
    unsigned int  Enable;
    unsigned int  SpiConfigValue;

    // Enable LPC decode
    // Set COMA/COMB base
    Decode =  ((V_LPC_CFG_IOD_COMA_3F8 << N_LPC_CFG_IOD_COMA) | (V_LPC_CFG_IOD_COMB_2F8 << N_LPC_CFG_IOD_COMB));
    SpiConfigValue = MmioRead32 (PCH_PCR_ADDRESS (PID_ESPISPI, R_PCH_PCR_SPI_CONF_VALUE));
    if (SpiConfigValue & B_ESPI_ENABLE_STRAP) {
    Enable =  ( B_LPC_CFG_IOE_ME2 | B_LPC_CFG_IOE_SE | B_LPC_CFG_IOE_ME1 | B_LPC_CFG_IOE_CBE | B_LPC_CFG_IOE_CAE);
    } else {
    Enable =  ( B_LPC_CFG_IOE_ME2 | B_LPC_CFG_IOE_SE | B_LPC_CFG_IOE_ME1 | B_LPC_CFG_IOE_CBE | B_LPC_CFG_IOE_CAE | B_LPC_CFG_IOE_KE);
    }
    outl((unsigned int) (PCH_LPC_CF8_ADDR (R_LPC_CFG_IOD)), R_PCH_IOPORT_PCI_INDEX);

    outl(Decode | (Enable << 16), R_PCH_IOPORT_PCI_DATA);

    MmioWrite16 (PCH_PCR_ADDRESS(PID_DMI, R_PCH_DMI_PCR_LPCIOD), (unsigned short)Decode);
    MmioWrite16 (PCH_PCR_ADDRESS(PID_DMI, R_PCH_DMI_PCR_LPCIOE), (unsigned short)Enable);

    SioExist = is_sio_exist();

    if ((SioExist & (PILOTIV_EXIST | PC8374_EXIST)) == (PILOTIV_EXIST | PC8374_EXIST) ) {
    // Both are there, we use DEFAULT_SIO as debug port anyway
    if (DEFAULT_SIO == PILOTIV_EXIST) {
        SioEnable = PILOTIV_EXIST;
    } else {
        SioEnable = PC8374_EXIST;
    }
    } else {
    SioEnable = SioExist;
    }

    // ASPEED AST2500/AST2600 UART init.
    if (SioEnable == ASPEED_EXIST) {
    // Unlock SIO
    outb (ASPEED2500_SIO_UNLOCK, ASPEED2500_SIO_INDEX_PORT);
    outb (ASPEED2500_SIO_UNLOCK, ASPEED2500_SIO_INDEX_PORT);

    // COM1 & COM2
    for (Index = 0; Index < sizeof (mASPEED2500Table) / sizeof (SIO_REG_TABLE); Index++) {
        outb (mASPEED2500Table[Index].Index, ASPEED2500_SIO_INDEX_PORT);
        outb (mASPEED2500Table[Index].Data, ASPEED2500_SIO_DATA_PORT);
    }

    // Lock SIO
    outb (ASPEED2500_SIO_LOCK, ASPEED2500_SIO_INDEX_PORT);
    }
}
