// Copyright (c) 2020 Bjørn Brodtkorb
//
// This software is provided "as is" without warranty of any kind.
// Permission is granted, free of charge, to copy and modify this
// software, if this copyright notice is included in all copies of
// the software.

#include "board_uart.h"
#include "clock.h"
#include "interrupt.h"
#include "gpio.h"
#include "uart.h"


//--------------------------------------------------------------------------------------------------//


static void board_uart_clock_config(void)
{
	clock_peripheral_clock_enable(ID_UART4);
}


//--------------------------------------------------------------------------------------------------//


void board_uart_pin_config(void)
{
	gpio_set_pin_function(PIOD, 19, PERIPHERAL_FUNCTION_C);
	gpio_set_pin_function(PIOD, 18, PERIPHERAL_FUNCTION_C);
}


//--------------------------------------------------------------------------------------------------//


static void board_uart_mode_config(void)
{
	uart_write_protection_disable(UART4);
	
	uart_mode_config(UART4, UART_PARITY_NO, UART_CH_MOD_NORMAL, UART_SOURCE_CLOCK_MCK, 0);
	uart_set_buad_rate(UART4, 115200);
	
	// We enable interrupt
	uart_interrupt_enable(UART4, UART_INTERRUPT_RX_READY);
	interrupt_enable_peripheral_interrupt(UART4_IRQn, IRQ_LEVEL_4);
	
	uart_transmitter_enable(UART4);
}


//--------------------------------------------------------------------------------------------------//


void board_uart_config(void)
{
	board_uart_clock_config();
	board_uart_pin_config();
	board_uart_mode_config();
}


//--------------------------------------------------------------------------------------------------//


void board_uart_write(char* data)
{
	while (*data != '\0')
	{
		uart_write(UART4, *data++);
	}
}


//--------------------------------------------------------------------------------------------------//


void UART4_Handler()
{
	// Read the status register to clear the interrupt flag
	uart_read_interrupt_status(UART4);
	
	// Here we will read the mouse data that is sent to the beard
}


//--------------------------------------------------------------------------------------------------//