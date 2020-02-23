#ifndef MATRIX_H
#define MATRIX_H

#include "sam.h"


typedef enum
{
	MATRIX_CS_SDRAM,
	MATRIX_CS_NAND_FLASH,
	MATRIX_NONE
} Matrix_chip_select_1_e;

void matrix_write_protection_disable(Matrix* hardware);

void matrix_write_protection_enable(Matrix* hardware);

void matrix_write_flash_chip_select_configuration_register(	Matrix* hardware,
															Matrix_chip_select_1_e assign_cs1,
															uint8_t assign_cs2_flash,
															uint8_t assign_cs3_flash,
															uint8_t assign_cs0_flash);

#endif