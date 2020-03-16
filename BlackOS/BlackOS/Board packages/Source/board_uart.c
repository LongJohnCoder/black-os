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
#include "graphics_driver.h"


//--------------------------------------------------------------------------------------------------//


static volatile char cursor_buffer[4];

volatile uint16_t location_x;
volatile uint16_t location_y;

static volatile char cursor_index = -1;


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
	uart_receiver_enable(UART4);
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
	char data_received = uart_read(UART4);
	
	/*
	
	if (data_received == 0xfb)
	{
		cursor_index = 0;
	}
	else
	{
		if (cursor_index != -1)
		{
			cursor_buffer[cursor_index++] = data_received;
		}
	}
	
	if (cursor_index >= 4)
	{
		cursor_index = -1;
		
		location_x = (uint16_t)cursor_buffer[0] | (uint16_t)((cursor_buffer[1] << 8));
		location_y = cursor_buffer[2] | (cursor_buffer[3] << 8);
		
		graphics_update_cursor(location_x, location_y);
	}
	*/
	
	(void)data_received;
}


//--------------------------------------------------------------------------------------------------//