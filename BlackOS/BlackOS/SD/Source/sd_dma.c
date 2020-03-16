#include "sd_dma.h"

static uint32_t hsmci_transferPos;
static uint16_t hsmci_blocksize, hsmci_numBlocks;

const uint32_t sd_mmc_trans_units[7] = {
	10, 100, 1000, 10000, 0, 0, 0
};
//! SD transfer multiplier factor codes (1/10) list
const uint32_t sd_trans_multipliers[16] = {
	0, 10, 12, 13, 15, 20, 25, 30, 35, 40, 45, 50, 55, 60, 70, 80
};

uint32_t HSMCI_sendCommand(uint32_t cmdr, uint32_t cmd, uint32_t arg)
{
	
	cmdr	|=	HSMCI_CMDR_CMDNB(cmd)	//set command
	|	HSMCI_CMDR_SPCMD_STD;	//standard command

	//check command response and command definitions
	if (cmd & SDMMC_RESP_PRESENT) {
		cmdr |= HSMCI_CMDR_MAXLAT;
		
		if (cmd & SDMMC_RESP_136) {
			cmdr |= HSMCI_CMDR_RSPTYP_136_BIT;
			} else if (cmd & SDMMC_RESP_BUSY) {
			cmdr |= HSMCI_CMDR_RSPTYP_R1B;
			} else {
			cmdr |= HSMCI_CMDR_RSPTYP_48_BIT;
		}
	}
	
	if (cmd & SDMMC_CMD_OPENDRAIN) {
		cmdr |= HSMCI_CMDR_OPDCMD_OPENDRAIN;
	}
	
	//write arg
	HSMCI -> HSMCI_ARGR = arg;
	
	//write and start command
	HSMCI -> HSMCI_CMDR = cmdr;
	
	while (! (HSMCI -> HSMCI_SR & HSMCI_SR_CMDRDY) );			// wait for command to be send
	
	if (cmd & SDMMC_RESP_BUSY) {
		while (! (HSMCI -> HSMCI_SR & HSMCI_SR_NOTBUSY) );		//ready for new data transfer
	}

	return 1;		//return cmd send
}

uint32_t HSMCI_getResponse(void)
{
	return (HSMCI -> HSMCI_RSPR[0]);
}

void HSMCI_adtc_start(uint32_t cmd, uint32_t arg, uint16_t blocksize, uint16_t num_blocks, uint8_t use_DMA)
{
	uint32_t cmdr;
	
	if (use_DMA) {
		HSMCI->HSMCI_DMA = HSMCI_DMA_DMAEN;
		} else {
		HSMCI->HSMCI_DMA = 0;
	}
	
	// Enabling Read/Write Proof allows to stop the HSMCI Clock during
	// read/write  access if the internal FIFO is full.
	// This will guarantee data integrity, not bandwidth.
	HSMCI->HSMCI_MR |= HSMCI_MR_WRPROOF | HSMCI_MR_RDPROOF;
	// Force byte transfer if needed
	if (blocksize & 0x3) {
		HSMCI->HSMCI_MR |= HSMCI_MR_FBYTE;
		} else {
		HSMCI->HSMCI_MR &= ~HSMCI_MR_FBYTE;
	}
	
	
	if (cmd & SDMMC_CMD_WRITE) {
		cmdr = HSMCI_CMDR_TRCMD_START_DATA | HSMCI_CMDR_TRDIR_WRITE;
		} else {
		cmdr = HSMCI_CMDR_TRCMD_START_DATA | HSMCI_CMDR_TRDIR_READ;
	}
	
	if (cmd & SDMMC_CMD_SDIO_BYTE) {
		cmdr |= HSMCI_CMDR_TRTYP_BYTE;
		// Value 0 corresponds to a 512-byte transfer
		HSMCI->HSMCI_BLKR = ((blocksize % 512) << HSMCI_BLKR_BCNT_Pos);
		} else {
		HSMCI->HSMCI_BLKR = (blocksize << HSMCI_BLKR_BLKLEN_Pos) |	(num_blocks << HSMCI_BLKR_BCNT_Pos);
		if (cmd & SDMMC_CMD_SDIO_BLOCK) {
			cmdr |= HSMCI_CMDR_TRTYP_BLOCK;
			} else if (cmd & SDMMC_CMD_STREAM) {
			cmdr |= HSMCI_CMDR_TRTYP_STREAM;
			} else if (cmd & SDMMC_CMD_SINGLE_BLOCK) {
			cmdr |= HSMCI_CMDR_TRTYP_SINGLE;
			} else if (cmd & SDMMC_CMD_MULTI_BLOCK) {
			cmdr |= HSMCI_CMDR_TRTYP_MULTIPLE;
		}
	}
	
	hsmci_transferPos = 0;			//reset transferpos
	hsmci_blocksize = blocksize;
	hsmci_numBlocks = num_blocks;
	
	HSMCI_sendCommand(cmdr, cmd, arg);
	
}

void HSMCI_readBlocks(void *dest, uint16_t num_blocks)
{
	uint32_t datasize = num_blocks * hsmci_blocksize;
	
	XDMAC -> XDMAC_GD = (XDMAC_GD_DI0 << HSMCI_XDMAC_CH);	//disable DMA channel
	XDMAC ->XdmacChid[HSMCI_XDMAC_CH].XDMAC_CBC = XDMAC_CBC_BLEN(0);
	XDMAC -> XdmacChid[HSMCI_XDMAC_CH].XDMAC_CDS_MSP = 0;
	XDMAC -> XdmacChid[HSMCI_XDMAC_CH].XDMAC_CSUS = 0;
	XDMAC -> XdmacChid[HSMCI_XDMAC_CH].XDMAC_CDUS = 0;
	
	
	if((uint32_t)dest & 3) {
		XDMAC -> XdmacChid[HSMCI_XDMAC_CH].XDMAC_CUBC = datasize;
		HSMCI->HSMCI_MR |= HSMCI_MR_FBYTE;
		XDMAC -> XdmacChid[HSMCI_XDMAC_CH].XDMAC_CC	= XDMAC_CC_TYPE_PER_TRAN
		| XDMAC_CC_MBSIZE_SINGLE
		| XDMAC_CC_DSYNC_PER2MEM
		| XDMAC_CC_CSIZE_CHK_1
		| XDMAC_CC_DWIDTH_BYTE
		| XDMAC_CC_SIF_AHB_IF1
		| XDMAC_CC_DIF_AHB_IF0
		| XDMAC_CC_SAM_FIXED_AM
		| XDMAC_CC_DAM_INCREMENTED_AM
		| XDMAC_CC_PERID(0);
		} else {
		XDMAC -> XdmacChid[HSMCI_XDMAC_CH].XDMAC_CUBC = datasize / 4;
		HSMCI->HSMCI_MR &= ~HSMCI_MR_FBYTE;
		XDMAC -> XdmacChid[HSMCI_XDMAC_CH].XDMAC_CC	= XDMAC_CC_TYPE_PER_TRAN
		| XDMAC_CC_MBSIZE_SINGLE
		| XDMAC_CC_DSYNC_PER2MEM
		| XDMAC_CC_CSIZE_CHK_1
		| XDMAC_CC_DWIDTH_WORD
		| XDMAC_CC_SIF_AHB_IF1
		| XDMAC_CC_DIF_AHB_IF0
		| XDMAC_CC_SAM_FIXED_AM
		| XDMAC_CC_DAM_INCREMENTED_AM
		| XDMAC_CC_PERID(0);
	}
	
	XDMAC -> XdmacChid[HSMCI_XDMAC_CH].XDMAC_CSA = (uint32_t)&(HSMCI->HSMCI_FIFO[0]);
	XDMAC -> XdmacChid[HSMCI_XDMAC_CH].XDMAC_CDA = (uint32_t)dest;
	
	XDMAC -> XdmacChid[HSMCI_XDMAC_CH].XDMAC_CIS;	//read interrupt reg to clear any flags prior to enabling channel
	XDMAC -> XDMAC_GE = ( XDMAC_GE_EN0 << HSMCI_XDMAC_CH );

	//block written increment counter
	hsmci_transferPos += datasize;
	
}

uint32_t HSMCI_WaitEndBlockTransfer(void)
{
	XDMAC -> XdmacChid[HSMCI_XDMAC_CH].XDMAC_CIS; //clear flag
	uint32_t timeout = 0xFFFFF;
	
	do {
		if ( (HSMCI -> HSMCI_SR) & (HSMCI_SR_UNRE | HSMCI_SR_OVRE | HSMCI_SR_DTOE | HSMCI_SR_DCRCE) ) {		//also do a hardware timout check
			XDMAC -> XDMAC_GD = (XDMAC_GD_DI0 << HSMCI_XDMAC_CH);	//disable DMA channel, because no point in finishing after error is given
			return 0;
		}
		if ( ((uint32_t)hsmci_blocksize * hsmci_numBlocks) > hsmci_transferPos ) {	// This was not the last block of data
			if (XDMAC -> XdmacChid[HSMCI_XDMAC_CH].XDMAC_CIS & XDMAC_CIS_BIS) {
				return 1;
			}
		}
		if (--timeout == 0) {
			break;
		}
	} while ( ! ( (HSMCI -> HSMCI_SR) & HSMCI_SR_XFRDONE) );
	
	if (!timeout) {	//timeout reached
		return 0;
	} else return 1;
}

uint8_t SD_readBlocks(sd_card* card, void *dst, uint16_t num_blocks, uint32_t startaddr)
{
	uint32_t cmd, arg;
	uint32_t retry = 200000;

	//wait for data ready
	do {
		HSMCI_sendCommand(0, SDMMC_MCI_CMD13_SEND_STATUS, (uint32_t)card->relative_card_address << 16);
		if (HSMCI_getResponse() & CARD_STATUS_READY_FOR_DATA) {
			break;
		}
		
		if (retry-- == 0) {
			return 0;
		}
	} while (1);
	
	if (num_blocks > 1) {
		cmd = SDMMC_CMD18_READ_MULTIPLE_BLOCK;
		} else {
		cmd = SDMMC_CMD17_READ_SINGLE_BLOCK;
	}
	
	if (card->card_type == SDHC) {
		arg = startaddr;
		} else {
		arg = (startaddr * HSMCI_SD_BLOCKSZ);
	}

	HSMCI_adtc_start(cmd, arg, HSMCI_SD_BLOCKSZ, num_blocks, 1);
	
	//check for error code?
	
	HSMCI_readBlocks(dst, num_blocks);
	HSMCI_WaitEndBlockTransfer();

	HSMCI_sendCommand(0, SDMMC_CMD12_STOP_TRANSMISSION, 0);
	
	return 1;
}