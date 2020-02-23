#ifndef HSMCI_H
#define HSMCI_H

#include "sam.h"

#define HSMCI_DEBUG 1

typedef enum
{
	HSMCI_MR_PADDING_00,
	HSMCI_MR_PADDING_FF
} Hsmci_mr_padding_e;

typedef enum
{
	HSMCI_TIMEOUT_MULTIPLIER_1,
	HSMCI_TIMEOUT_MULTIPLIER_16,
	HSMCI_TIMEOUT_MULTIPLIER_128,
	HSMCI_TIMEOUT_MULTIPLIER_256,
	HSMCI_TIMEOUT_MULTIPLIER_1024,
	HSMCI_TIMEOUT_MULTIPLIER_4096,
	HSMCI_TIMEOUT_MULTIPLIER_65536,
	HSMCI_TIMEOUT_MULTIPLIER_1048576
} Hsmci_data_timeout_multiplier_e;

typedef enum
{
	HSMCI_SD_BUS_WIDTH_1_BIT = 0,
	HSMCI_SD_BUS_WIDTH_4_BIT = 2,
	HSMCI_SD_BUS_WIDTH_8_BIT = 3
} Hsmci_sd_bus_width_e;

typedef enum
{
	HSMCI_SLOT_A,
	HSMCI_SLOT_B,
	HSMCI_SLOT_C,
	HSMCI_SLOT_D
} Hsmci_sd_slot_select_e;

typedef enum
{
	HSMCI_COMMAND_SDIO_SPECIAL_COMMAND_STD,
	HSMCI_COMMAND_SDIO_SPECIAL_COMMAND_SUSPEND,
	HSMCI_COMMAND_SDIO_SPECIAL_COMMAND_RESUME
} Hsmci_command_sdio_special_command_e;

typedef enum
{
	HSMCI_COMMAND_TRANSFER_TYPE_SINGLE,
	HSMCI_COMMAND_TRANSFER_TYPE_MULTIPLE,
	HSMCI_COMMAND_TRANSFER_TYPE_STREAM,
	HSMCI_COMMAND_TRANSFER_TYPE_BYTE,
	HSMCI_COMMAND_TRANSFER_TYPE_BLOCK
} Hsmci_command_transfer_type_e;

typedef enum
{
	HSMCI_COMMAND_TRANSFER_DIRECTION_WRITE,
	HSMCI_COMMAND_TRANSFER_DIRECTION_READ
} Hsmci_command_transfer_direction_e;

typedef enum
{
	HSMCI_COMMAND_TRANSFER_COMMAND_NO_DATA,
	HSMCI_COMMAND_TRANSFER_COMMAND_START_DATA,
	HSMCI_COMMAND_TRANSFER_COMMAND_STOP_DATA,
} Hsmci_command_transfer_command_e;

typedef enum
{
	HSMCI_COMMAND_MAX_LATENCY_5_CYCLE,
	HSMCI_COMMAND_MAX_LATENCY_64_CYCLE
} Hsmci_command_max_latency_e;

typedef enum
{
	HSMCI_COMMAND_SPECIAL_COMMAND_STD,
	HSMCI_COMMAND_SPECIAL_COMMAND_INIT,
	HSMCI_COMMAND_SPECIAL_COMMAND_SYNC,
	HSMCI_COMMAND_SPECIAL_COMMAND_CE_ATA,
	HSMCI_COMMAND_SPECIAL_COMMAND_IT_CMD,
	HSMCI_COMMAND_SPECIAL_COMMAND_IT_RESP,
	HSMCI_COMMAND_SPECIAL_COMMAND_BOR,
	HSMCI_COMMAND_SPECIAL_COMMAND_EBO
} Hsmci_command_special_command_e;

typedef enum
{
	HSMCI_COMMAND_RESPONCE_TYPE_NORESP,
	HSMCI_COMMAND_RESPONCE_TYPE_48_BIT,
	HSMCI_COMMAND_RESPONCE_TYPE_136_BIT,
	HSMCI_COMMAND_RESPONCE_TYPE_R1B,
} Hsmci_command_responce_type_e;

typedef enum
{
	HSMCI_OK,
	HSMCI_ERROR
} Hsmci_status_e;

typedef enum
{
	CHECK_CRC,
	DONT_CHECK_CRC
} Hsmci_check_crc_e;

typedef enum
{
	HSMCI_POWERSAVE_ON,
	HSMCI_POWERSAVE_OFF
} Hsmci_control_powersave_e;

typedef enum
{
	HSMCI_ENABLE,
	HSMCI_DISABLE
} Hsmci_control_enable_e;

#define HSMCI_STATUS_REGISTER_ERROR_MASK ((1 << HSMCI_SR_ACKRCV_Pos) | (1 << HSMCI_SR_BLKOVRE_Pos) | (1 << HSMCI_SR_CSTOE_Pos) | (1 << HSMCI_SR_DTOE_Pos) | (1 << HSMCI_SR_DCRCE_Pos) | (1 << HSMCI_SR_RTOE_Pos) | (1 << HSMCI_SR_CSTOE_Pos) | (1 << HSMCI_SR_RENDE_Pos) | (1 << HSMCI_SR_RDIRE_Pos) | (1 << HSMCI_SR_RINDE_Pos))


//====================================== W R I T E   P R O T E C T I O N ======================================//

void hsmci_write_protection_enable(const void* const hardware);

void hsmci_write_protection_disable(const void* const hardware);



//================================================= R E S E T =================================================//

void hsmci_software_reset(const void* const hardware);

void hsmci_soft_reset(const void* const hardware);



//=============================================== T I M E O U T ===============================================//

void hsmci_set_completion_timeout(const void* const hardware, Hsmci_data_timeout_multiplier_e completion_signal_timout_multiplier, uint8_t completion_signal_timeout_cycle_number);
						
void hsmci_set_data_timeout(const void* const hardware, Hsmci_data_timeout_multiplier_e data_timeout_multiplier, uint8_t data_timeout_cycle_number);



//=============================================== C O N T R O L ===============================================//

void hsmci_write_control_register(const void* const hardware, Hsmci_control_enable_e enable, Hsmci_control_powersave_e powersave);

void hsmci_sd_card_config(const void* const hardware, Hsmci_sd_bus_width_e bus_width, Hsmci_sd_slot_select_e slot_selct);

void hsmci_set_block_length(const void* const hardware, uint32_t block_length, uint32_t block_count);



//======================================= W R I T E   F U N C T I O N S =======================================//

void hsmci_write_argument_register(const void* const hardware, uint32_t argument);

void hsmci_write_data_register(const void* const hardware, uint32_t data);

							

//======================================== R E A D   F U N C T I O N S ========================================//

uint32_t hsmci_read_data_register(const void* const hardware);

void hsmci_read_data_register_reverse(const void* const hardware, uint8_t* data, uint8_t number_of_words);

uint32_t hsmci_read_48_bit_response_register(const void* const hardware);	

void hsmci_read_136_bit_response_register_extended(const void* const hardware, uint8_t* response);
						
uint32_t hsmci_read_status_register(const void* const hardware);
			
					

//===================================== C O M M A N D   F U N C T I O N S =====================================//

Hsmci_status_e hsmci_send_command(const void* hardware, uint32_t command_register, uint32_t argument, Hsmci_check_crc_e crc);

void hsmci_write_command_register(const void* const hardware, uint32_t command);

Hsmci_status_e hsmci_send_addressed_data_transfer_command(	const void* const hardware, uint32_t command_register, uint32_t argument,
															uint16_t block_size, uint16_t number_of_blocks, Hsmci_check_crc_e crc);

uint32_t hsmci_construct_command_register(	uint8_t boot_ack,
										uint8_t ata_with_command_completion_enable,
										Hsmci_command_sdio_special_command_e sdio_special_command,
										Hsmci_command_transfer_type_e transfer_type,
										Hsmci_command_transfer_direction_e transfer_direction,
										Hsmci_command_transfer_command_e transfer_command,
										Hsmci_command_max_latency_e max_latency,
										uint8_t open_drain_enable,
										Hsmci_command_special_command_e special_command,
										Hsmci_command_responce_type_e responce_type,
										uint8_t command_number);
										


//========================================= M O D E   R E G I S T E R =========================================//

void hsmci_force_byte_transfer_enable(const void* const hardware);

void hsmci_force_byte_transfer_disable(const void* const hardware);

void hsmci_read_proof_enable(const void* const hardware);

void hsmci_read_proof_disable(const void* const hardware);

void hsmci_write_proof_enable(const void* const hardware);

void hsmci_write_proof_disable(const void* const hardware);

void hsmci_set_padding_value(const void* const hardware, Hsmci_mr_padding_e padding);

void hsmci_set_bus_speed(const void* const hardware, uint32_t bus_speed, uint32_t cpu_peripheral_speed);

void hsmci_write_mode_register(	const void* const hardware,
								uint8_t odd_clock_divider,
								Hsmci_mr_padding_e padding,
								uint8_t force_byte_tranfer_enable,
								uint8_t write_proof_enable,
								uint8_t read_proof_enable,
								uint8_t power_save_divider,
								uint8_t clock_divider);



//======================================= C O N F I G   R E G I S T E R =======================================//

void hsmci_high_speed_enable(const void* const hardware);

void hsmci_write_configuration_register(const void* const hardware,
										uint8_t synhronize_last_block,
										uint8_t high_speed_mode,
										uint8_t flow_error_reset_control_mode,
										uint8_t transfer_after_data_written_to_fifo);

#endif