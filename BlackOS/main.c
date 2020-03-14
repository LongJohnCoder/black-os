// Copyright (c) 2020 Bjørn Brodtkorb
//
// This software is provided "as is" without warranty of any kind.
// Permission is granted, free of charge, to copy and modify this
// software, if this copyright notice is included in all copies of
// the software.

#include "kernel.h"
#include "board_led.h"
#include "gpio.h"
#include "board_serial_programming.h"

#include "timer.h"
#include "clock.h"
#include "interrupt.h"


//--------------------------------------------------------------------------------------------------//


#include <stddef.h>


//--------------------------------------------------------------------------------------------------//


void runtime_agent(void* arg)
{
	while (1)
	{
		service_thread_delay(1000);
		kernel_print_runtime_statistics();
	}
}


//--------------------------------------------------------------------------------------------------//


int main(void)
{
	// This functions starts up the kernel and initializes the basic drivers
	kernel_startup();
	
	// Add some threads for test & debug purposes
	kernel_add_thread("blink", blink_thread, NULL, THREAD_PRIORITY_NORMAL, 200);
	kernel_add_thread("runtime", runtime_agent, NULL, THREAD_PRIORITY_NORMAL, 200);

	// Start the kernel
	kernel_start();
	
	while (1);
}


//--------------------------------------------------------------------------------------------------//