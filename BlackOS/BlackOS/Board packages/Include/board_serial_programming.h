// Copyright (c) 2020 Bjørn Brodtkorb
//
// This software is provided "as is" without warranty of any kind.
// Permission is granted, free of charge, to copy and modify this
// software, if this copyright notice is included in all copies of
// the software.

#ifndef BOARD_SERIAL_PROGRAMMING_H
#define BOARD_SERIAL_PROGRAMMING_H


//--------------------------------------------------------------------------------------------------//


#include "sam.h"


//--------------------------------------------------------------------------------------------------//


#define BOARD_SERIAL_PROGRAMMING_RX_PIN	0
#define BOARD_SERIAL_PROGRAMMING_RX_PORT	PIOB

#define BOARD_SERIAL_PROGRAMMING_TX_PIN	1
#define BOARD_SERIAL_PROGRAMMING_TX_PORT	PIOB

#define BOARD_SERIAL_PROGRAMMING_CPU_FREQ	150000000
#define BOARD_SERIAL_PROGRAMMING_BAUD_RATE	115200
#define BOARD_SERIAL_PROGRAMMING_CD_VALUE	BOARD_SERIAL_PROGRAMMING_CPU_FREQ / (16 * BOARD_SERIAL_PROGRAMMING_BAUD_RATE)


//--------------------------------------------------------------------------------------------------//


void board_serial_programming_config(void);

void board_serial_programming_write_percent(char first, char second);

void board_serial_programming_write(char data);

void board_serial_programming_print(char* data, ...);


//--------------------------------------------------------------------------------------------------//


#endif