#ifndef BOARD_SERIAL_X_H
#define BOARD_SERIAL_X_H

#include "sam.h"

#define BOARD_SERIAL_X_RX_PIN		0
#define BOARD_SERIAL_X_RX_PORT	PIOB

#define BOARD_SERIAL_X_TX_PIN		1
#define BOARD_SERIAL_X_TX_PORT	PIOB

#define BOARD_SERIAL_X_CPU_FREQ	150000000
#define BOARD_SERIAL_X_BAUD_RATE	115200
#define BOARD_SERIAL_X_CD_VALUE	BOARD_SERIAL_X_CPU_FREQ / (16 * BOARD_SERIAL_X_BAUD_RATE)


void board_serial_x_config(void);

void board_serial_x_write_percent(char first, char second);

void board_serial_x_write(char data);

void board_serial_x_print(char* data, ...);

#endif