#ifndef SPI_H
#define SPI_H

#include "sam.h"

typedef enum
{
	SPI_FIXED_CHIP_SELECT_0 = 0b0000,
	SPI_FIXED_CHIP_SELECT_1 = 0b0001,
	SPI_FIXED_CHIP_SELECT_2 = 0b0011,
	SPI_FIXED_CHIP_SELECT_3 = 0b0111,
	SPI_FIXED_CHIP_SELECT_NONE = 0b1111
} spi_fixed_chip_select;

typedef enum
{
	SPI_MODE_SLAVE,
	SPI_MODE_MASTER
} spi_master_slave_mode;

typedef enum
{
	SPI_FIXED_PERIPHERAL,
	SPI_VARIABLE_PERIPHERAL	
} spi_peripheral_mode;

typedef enum
{	
	SPI_CHIP_SELECT_0,
	SPI_CHIP_SELECT_1,
	SPI_CHIP_SELECT_2,
	SPI_CHIP_SELECT_3
} spi_chip_select;

typedef enum
{
	SPI_8_BIT,
	SPI_9_BIT,
	SPI_10_BIT,
	SPI_11_BIT,
	SPI_12_BIT,
	SPI_13_BIT,
	SPI_14_BIT,
	SPI_15_BIT,
	SPI_16_BIT
} spi_bits_per_transfer;


typedef enum
{
	SPI_CLOCK_CAPTURE_ON_TRAILING,
	SPI_CLOCK_CAPTURE_ON_LEADING
} spi_clock_phase;

typedef enum
{
	SPI_CLOCK_INACTIVE_LOW,
	SPI_CLOCK_INACTIVE_HIGH
}spi_clock_polarity;

//================================= W R I T E   P R O T E C T I O N =================================//

void spi_write_protection_enable(Spi* hardware);

void spi_write_protection_disable(Spi* hardware);




//=========================================== C O N F I G ===========================================//

void spi_mode_config(Spi* hardware, uint8_t delay_between_chip_selekt, spi_fixed_chip_select fixed_chip_select, 
						uint8_t local_loopback_enable, uint8_t wait_data_read_befor_transfere,
						uint8_t mode_fault_detection_disable, uint8_t chip_select_decode_enable,
						spi_peripheral_mode peripheral_mode , spi_master_slave_mode master_slave_mode);



void spi_chip_select_config(Spi* hardware, spi_chip_select chip_select, uint8_t delay_between_consecutive_transfers,
							uint8_t delay_before_clock, uint8_t serial_clock_bit_rate, spi_bits_per_transfer bits_per_transfer,
							uint8_t chip_select_active_after_transfer, uint8_t chip_select_not_active_after_transfer,
							spi_clock_polarity clock_polarity, spi_clock_phase clock_phase);


void spi_set_bit_size(Spi* hardware, spi_chip_select chip_select, spi_bits_per_transfer bits_per_transfer);


//========================= E N A B L E   /   D I S A B L E   /   R E S E T =========================//

void spi_enable(Spi* hardware);

void spi_disable(Spi* hardware);

void spi_software_reset (Spi* hardware);




//========================================= T R A N S M I T =========================================//

void spi_trasmit_data_16_bit(Spi* hardware, uint16_t transmit_data);

void spi_trasmit_data_8_bit(Spi* hardware, uint8_t transmit_data);

void spi_flush(Spi* hardware);




//=========================================== S T A T U S ===========================================//

uint32_t spi_get_status_register(Spi* hardware);




#endif