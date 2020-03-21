// Copyright (c) 2020 Bjørn Brodtkorb
//
// This software is provided "as is" without warranty of any kind.
// Permission is granted, free of charge, to copy and modify this
// software, if this copyright notice is included in all copies of
// the software.

#include "kernel.h"
#include "board_led.h"
#include "board_serial.h"
#include "gpio.h"


//--------------------------------------------------------------------------------------------------//


#include <stddef.h>


//--------------------------------------------------------------------------------------------------//


void runtime_agent(void* arg)
{
	while (1)
	{
		syscall_sleep(1000);
		kernel_print_runtime_statistics();
	}
}


//--------------------------------------------------------------------------------------------------//


void waveform(void* arg)
{
	gpio_set_pin_function(PIOB, 2, PERIPHERAL_FUNCTION_OFF);
	gpio_set_pin_direction_output(PIOB, 2);
	while (1)
	{
		syscall_sleep(1);
		gpio_toogle_pin_value(PIOB, 2);
	}
}


//--------------------------------------------------------------------------------------------------//


void welcome_thread(void* arg)
{
	// Print a happy message to the screen
	board_serial_print("Kernel successfully started\n\n");
}


//--------------------------------------------------------------------------------------------------//

volatile uint32_t counter = 1000;

void increment(void* arg)
{
	for (uint32_t i = 0; i < 100000000; i++)
	{
		counter++;	
	}
	board_serial_print("Counter increment: %d\n", counter);
}


//--------------------------------------------------------------------------------------------------//


void decrement(void* arg)
{
	for (uint32_t i = 0; i < 100000000; i++)
	{
		counter--;
	}
	
	board_serial_print("Counter decrement: %d\n", counter);
}


//--------------------------------------------------------------------------------------------------//


int main(void)
{
	// This functions starts up the kernel and initializes the basic drivers
	kernel_startup();

	
	// Add some threads for test & debug purposes
	kernel_add_thread("blink", blink_thread, NULL, THREAD_PRIORITY_NORMAL, 200);
	kernel_add_thread("runtime", runtime_agent, NULL, THREAD_PRIORITY_NORMAL, 200);
	kernel_add_thread("welcome", welcome_thread, NULL, THREAD_PRIORITY_LOW, 50);
	kernel_add_thread("waveform", waveform, NULL, THREAD_PRIORITY_LOW, 200);
	kernel_add_thread("increment", increment, NULL, THREAD_PRIORITY_LOW, 200);
	kernel_add_thread("decrement", decrement, NULL, THREAD_PRIORITY_LOW, 200);
	
	
	// Start the kernel
	kernel_start();
	
	while (1);
}


//--------------------------------------------------------------------------------------------------//