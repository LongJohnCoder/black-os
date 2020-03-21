// Copyright (c) 2020 Bjørn Brodtkorb
//
// This software is provided without warranty of any kind.
// Permission is granted, free of charge, to copy and modify this
// software, if this copyright notice is included in all copies of
// the software.

#ifndef BOARD_UART_H
#define BOARD_UART_H


//--------------------------------------------------------------------------------------------------//


#include "sam.h"


//--------------------------------------------------------------------------------------------------//


void board_uart_config(void);


//--------------------------------------------------------------------------------------------------//


void board_uart_write(char* data);


//--------------------------------------------------------------------------------------------------//


#endif