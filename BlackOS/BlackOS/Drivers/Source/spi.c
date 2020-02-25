#include "spi.h"
#include "core.h"

//--------------------------------------------------------------------------------------------------//

void spi_write_protection_enable(Spi* hardware)
{
	CRITICAL_SECTION_ENTER()
	hardware->SPI_WPMR = (SPI_WPMR_WPKEY_PASSWD_Val << SPI_WPMR_WPKEY_Pos) | (1 << SPI_WPMR_WPEN_Pos) | (0b11 << 1);
	CRITICAL_SECTION_LEAVE()
}

//--------------------------------------------------------------------------------------------------//

void spi_write_protection_disable(Spi* hardware)
{
	CRITICAL_SECTION_ENTER()
	hardware->SPI_WPMR = (SPI_WPMR_WPKEY_PASSWD_Val << SPI_WPMR_WPKEY_Pos);
	CRITICAL_SECTION_LEAVE()
}

//--------------------------------------------------------------------------------------------------//

void spi_mode_config(Spi* hardware, uint8_t delay_between_chip_selekt, spi_fixed_chip_select fixed_chip_select,
						uint8_t local_loopback_enable, uint8_t wait_data_read_befor_transfere,
						uint8_t mode_fault_detection_disable, uint8_t chip_select_decode_enable,
						spi_peripheral_mode peripheral_mode , spi_master_slave_mode master_slave_mode)
{
	uint32_t tmp = (delay_between_chip_selekt << SPI_MR_DLYBCS_Pos) | (fixed_chip_select << SPI_MR_PCS_Pos) |
					(local_loopback_enable << SPI_MR_LLB_Pos) | (wait_data_read_befor_transfere << SPI_MR_WDRBT_Pos) |
					(mode_fault_detection_disable << SPI_MR_MODFDIS_Pos) | (chip_select_decode_enable << SPI_MR_PCSDEC_Pos) |
					(peripheral_mode << SPI_MR_PS_Pos) | (master_slave_mode << SPI_MR_MSTR_Pos);
					
	CRITICAL_SECTION_ENTER()
	hardware->SPI_MR = tmp;
	CRITICAL_SECTION_LEAVE()
}

//--------------------------------------------------------------------------------------------------//

void spi_enable(Spi* hardware)
{
	CRITICAL_SECTION_ENTER()
	hardware->SPI_CR = (1 << SPI_CR_SPIEN_Pos);
	CRITICAL_SECTION_LEAVE()
}

//--------------------------------------------------------------------------------------------------//

void spi_disable(Spi* hardware)
{
	CRITICAL_SECTION_ENTER()
	hardware->SPI_CR = (1 << SPI_CR_SPIDIS_Pos);
	CRITICAL_SECTION_LEAVE()
}

//--------------------------------------------------------------------------------------------------//

void spi_software_reset (Spi* hardware)
{
	CRITICAL_SECTION_ENTER()
	hardware->SPI_CR = (1 << SPI_CR_SWRST_Pos);
	CRITICAL_SECTION_LEAVE()
}

//--------------------------------------------------------------------------------------------------//

void spi_trasmit_data_16_bit(Spi* hardware, uint16_t transmit_data)
{
	while (!(hardware->SPI_SR & (1 << SPI_SR_TDRE_Pos)))
	{
		
	}
	
	CRITICAL_SECTION_ENTER()
	hardware->SPI_TDR = transmit_data;
	CRITICAL_SECTION_LEAVE()
}

//--------------------------------------------------------------------------------------------------//

void spi_trasmit_data_8_bit(Spi* hardware, uint8_t transmit_data)
{	
	while (!(hardware->SPI_SR & (1 << SPI_SR_TDRE_Pos)))
	{
		
	}
	
	CRITICAL_SECTION_ENTER()
	hardware->SPI_TDR = transmit_data;
	CRITICAL_SECTION_LEAVE()
}

//--------------------------------------------------------------------------------------------------//

void spi_flush(Spi* hardware)
{	
	while(!(hardware->SPI_SR & (1 << SPI_SR_TXEMPTY_Pos)))
	{
		
	}
}

//--------------------------------------------------------------------------------------------------//

uint32_t spi_get_status_register(Spi* hardware)
{
	uint32_t tmp = hardware->SPI_SR;
	
	return tmp;
}

//--------------------------------------------------------------------------------------------------//


void spi_chip_select_config(Spi* hardware, spi_chip_select chip_select, uint8_t delay_between_consecutive_transfers,
							uint8_t delay_before_clock, uint8_t serial_clock_bit_rate, spi_bits_per_transfer bits_per_transfer,
							uint8_t chip_select_active_after_transfer, uint8_t chip_select_not_active_after_transfer,
							spi_clock_polarity clock_polarity, spi_clock_phase clock_phase)
{
	uint32_t tmp = (delay_between_consecutive_transfers << SPI_CSR_DLYBCT_Pos) | (delay_before_clock << SPI_CSR_DLYBS_Pos) |
					(serial_clock_bit_rate << SPI_CSR_SCBR_Pos) | (bits_per_transfer << SPI_CSR_BITS_Pos) |
					(chip_select_active_after_transfer << SPI_CSR_BITS_Pos) | (chip_select_not_active_after_transfer << SPI_CSR_CSNAAT_Pos) |
					(clock_phase << SPI_CSR_NCPHA_Pos) | (clock_polarity << SPI_CSR_CPOL_Pos);
					
	CRITICAL_SECTION_ENTER()
	hardware->SPI_CSR[chip_select] = tmp;
	CRITICAL_SECTION_LEAVE()
}

//--------------------------------------------------------------------------------------------------//

void spi_set_bit_size(Spi* hardware, spi_chip_select chip_select, spi_bits_per_transfer bits_per_transfer)
{
	uint32_t tmp = hardware->SPI_CSR[chip_select] & ~SPI_CSR_BITS_Msk;
	
	CRITICAL_SECTION_ENTER()
	hardware->SPI_CSR[chip_select] = tmp | (bits_per_transfer << SPI_CSR_BITS_Pos);
	CRITICAL_SECTION_LEAVE()
}

//--------------------------------------------------------------------------------------------------//