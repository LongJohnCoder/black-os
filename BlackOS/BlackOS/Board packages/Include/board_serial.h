#ifndef BOARD_SERIAL_H
#define BOARD_SERIAL_H

#include "sam.h"

#define BOARD_SERIAL_RX_PIN		4
#define BOARD_SERIAL_RX_PORT	PIOB

#define BOARD_SERIAL_TX_PIN		21
#define BOARD_SERIAL_TX_PORT	PIOA

#define BOARD_SERIAL_CPU_FREQ	150000000
#define BOARD_SERIAL_BAUD_RATE	115200
#define BOARD_SERIAL_CD_VALUE	BOARD_SERIAL_CPU_FREQ / (16 * BOARD_SERIAL_BAUD_RATE)

void board_serial_config(void);

void board_serial_print(char* data, ...);

void board_print_buffer(char* buffer, uint32_t size);

void board_serial_write(char data);

void board_serial_print_address(char* data, uint32_t addr);

void board_serial_print_percentage_symbol(char* data, uint8_t percent, uint8_t newline);

void board_serial_print_register(char* data, uint32_t reg);

#endif