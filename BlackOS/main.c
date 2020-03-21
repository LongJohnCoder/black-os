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
#include "mutex.h"
#include "dynamic_memory.h"


//--------------------------------------------------------------------------------------------------//


#include <stddef.h>


//--------------------------------------------------------------------------------------------------//


void runtime_agent(void* arg)
{
	while (1)
	{
		syscall_sleep(1000);
		print_runtime_statistics();
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


void waveform_2(void* arg)
{
	gpio_set_pin_function(PIOB, 3, PERIPHERAL_FUNCTION_OFF);
	gpio_set_pin_direction_output(PIOB, 3);
	while (1)
	{
		syscall_sleep(1);
		gpio_toogle_pin_value(PIOB, 3);
	}
}


//--------------------------------------------------------------------------------------------------//


void welcome_thread(void* arg)
{
	// Print a happy message to the screen
	board_serial_print("Kernel successfully started\n\n");
}


//--------------------------------------------------------------------------------------------------//


int main(void)
{
	// This functions starts up the kernel and initializes the basic drivers
	kernel_startup();
	
	
	// Add some threads for test & debug purposes
	thread_new("blink", blink_thread, NULL, THREAD_PRIORITY_NORMAL, 200);
	thread_new("runtime", runtime_agent, NULL, THREAD_PRIORITY_NORMAL, 200);
	thread_new("welcome", welcome_thread, NULL, THREAD_PRIORITY_LOW, 50);
	thread_new("waveform", waveform, NULL, THREAD_PRIORITY_LOW, 200);
	thread_new("waveform_2", waveform_2, NULL, THREAD_PRIORITY_LOW, 200);
	
	
	// Start the kernel
	kernel_launch();
	
	while (1);
}



//--------------------------------------------------------------------------------------------------//