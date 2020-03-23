// Copyright (c) 2020 Bjørn Brodtkorb
//
// This software is provided without warranty of any kind.
// Permission is granted, free of charge, to copy and modify this
// software, if this copyright notice is included in all copies of
// the software.

#include "kernel.h"
#include "board_led.h"
#include "board_serial.h"
#include "gpio.h"
#include "mutex.h"
#include "dynamic_memory.h"
#include "thread.h"


//--------------------------------------------------------------------------------------------------//


#include <stddef.h>


//--------------------------------------------------------------------------------------------------//


void runtime_stats(void* arg)
{
	while (1)
	{
		syscall_sleep(1000);
		print_runtime_statistics();
	}
}


//--------------------------------------------------------------------------------------------------//


void waveform_1(void* arg)
{
	gpio_set_pin_function(PIOB, 2, PERIPHERAL_FUNCTION_OFF);
	gpio_set_pin_direction_output(PIOB, 2);
	while (1)
	{
		syscall_sleep(10);
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
		syscall_sleep(10);
		gpio_toogle_pin_value(PIOB, 3);
	}
}


//--------------------------------------------------------------------------------------------------//


void waveform_3(void* arg)
{
	gpio_set_pin_function(PIOD, 0, PERIPHERAL_FUNCTION_OFF);
	gpio_set_pin_direction_output(PIOD, 0);
	while (1)
	{
		syscall_sleep(10);
		gpio_toogle_pin_value(PIOD, 0);
	}
}


//--------------------------------------------------------------------------------------------------//


void waveform_4(void* arg)
{
	gpio_set_pin_function(PIOB, 13, PERIPHERAL_FUNCTION_OFF);
	gpio_set_pin_direction_output(PIOB, 13);
	while (1)
	{
		syscall_sleep(10);
		gpio_toogle_pin_value(PIOB, 13);
	}
}


//--------------------------------------------------------------------------------------------------//


void waveform_5(void* arg)
{
	gpio_set_pin_function(PIOC, 31, PERIPHERAL_FUNCTION_OFF);
	gpio_set_pin_direction_output(PIOC, 31);
	while (1)
	{
		syscall_sleep(10);
		gpio_toogle_pin_value(PIOC, 31);
	}
}


//--------------------------------------------------------------------------------------------------//


void waveform_6(void* arg)
{
	gpio_set_pin_function(PIOA, 22, PERIPHERAL_FUNCTION_OFF);
	gpio_set_pin_direction_output(PIOA, 22);
	while (1)
	{
		syscall_sleep(10);
		gpio_toogle_pin_value(PIOA, 22);
	}
}


//--------------------------------------------------------------------------------------------------//


void welcome_thread(void* arg)
{
	// Print a happy message to the screen
	board_serial_print("Kernel successfully started\n\n");
}


//--------------------------------------------------------------------------------------------------//


void computing_1(void* arg)
{
	while (1)
	{
		asm volatile ("nop");
		syscall_sleep(1);

	}
}


//--------------------------------------------------------------------------------------------------//


void computing_2(void* arg)
{
	while (1)
	{
		asm volatile ("nop");
		syscall_sleep(1);
	}
}


//--------------------------------------------------------------------------------------------------//


void computing_3(void* arg)
{
	while (1)
	{
		asm volatile ("nop");
		syscall_sleep(1);
	}
}


//--------------------------------------------------------------------------------------------------//


void computing_4(void* arg)
{
	while (1)
	{
		asm volatile ("nop");
		syscall_sleep(1);

	}
}


//--------------------------------------------------------------------------------------------------//


void computing_5(void* arg)
{
	while (1)
	{
		asm volatile ("nop");
		syscall_sleep(1);

	}
}


//--------------------------------------------------------------------------------------------------//


void computing_6(void* arg)
{
	while (1)
	{
		asm volatile ("nop");
		syscall_sleep(1);

	}
}


//--------------------------------------------------------------------------------------------------//


void computing_7(void* arg)
{
	while (1)
	{
		asm volatile ("nop");
		syscall_sleep(1);

	}
}


//--------------------------------------------------------------------------------------------------//


void computing_8(void* arg)
{
	while (1)
	{
		asm volatile ("nop");
		syscall_sleep(1);

	}
}


//--------------------------------------------------------------------------------------------------//


void computing_9(void* arg)
{
	while (1)
	{
		asm volatile ("nop");
		syscall_sleep(1);

	}
}


//--------------------------------------------------------------------------------------------------//


void computing_10(void* arg)
{
	while (1)
	{
		asm volatile ("nop");
		syscall_sleep(1);

	}
}


//--------------------------------------------------------------------------------------------------//


void computing_11(void* arg)
{
	while (1)
	{
		asm volatile ("nop");
		syscall_sleep(1);

	}
}


//--------------------------------------------------------------------------------------------------//


void computing_12(void* arg)
{
	while (1)
	{
		asm volatile ("nop");
		syscall_sleep(1);

	}
}


//--------------------------------------------------------------------------------------------------//


void computing_13(void* arg)
{
	while (1)
	{
		asm volatile ("nop");
		syscall_sleep(1);

	}
}


//--------------------------------------------------------------------------------------------------//


void computing_14(void* arg)
{
	while (1)
	{
		asm volatile ("nop");
		syscall_sleep(1);

	}
}


//--------------------------------------------------------------------------------------------------//


void computing_15(void* arg)
{
	while (1)
	{
		asm volatile ("nop");
		syscall_sleep(1);

	}
}


//--------------------------------------------------------------------------------------------------//


void computing_16(void* arg)
{
	while (1)
	{
		asm volatile ("nop");
		syscall_sleep(1);

	}
}


//--------------------------------------------------------------------------------------------------//


void computing_17(void* arg)
{
	while (1)
	{
		asm volatile ("nop");
		syscall_sleep(1);

	}
}


//--------------------------------------------------------------------------------------------------//


int main(void)
{
	// This functions starts up the kernel and initializes the basic drivers
	kernel_startup();
	
	
	// Add some threads for test & debug purposes
	thread_new("blink", blink_thread, NULL, THREAD_PRIORITY_NORMAL, 100);
	thread_new("runtime", runtime_stats, NULL, THREAD_PRIORITY_NORMAL, 100);
	thread_new("welcome", welcome_thread, NULL, THREAD_PRIORITY_REAL_TIME, 50);
	thread_new("waveform 1", waveform_1, NULL, THREAD_PRIORITY_REAL_TIME, 100);
	thread_new("waveform 2", waveform_2, NULL, THREAD_PRIORITY_REAL_TIME, 100);
	thread_new("waveform 3", waveform_3, NULL, THREAD_PRIORITY_REAL_TIME, 100);
	thread_new("waveform 4", waveform_4, NULL, THREAD_PRIORITY_REAL_TIME, 100);
	thread_new("waveform 5", waveform_5, NULL, THREAD_PRIORITY_REAL_TIME, 100);
	thread_new("waveform 6", waveform_6, NULL, THREAD_PRIORITY_REAL_TIME, 100);


	thread_new("computing 1", computing_1, NULL, THREAD_PRIORITY_REAL_TIME, 100);
	thread_new("computing 2", computing_2, NULL, THREAD_PRIORITY_REAL_TIME, 100);
	thread_new("computing 3", computing_3, NULL, THREAD_PRIORITY_REAL_TIME, 100);
	thread_new("computing 4", computing_4, NULL, THREAD_PRIORITY_REAL_TIME, 100);
	thread_new("computing 5", computing_5, NULL, THREAD_PRIORITY_REAL_TIME, 100);
	thread_new("computing 6", computing_6, NULL, THREAD_PRIORITY_REAL_TIME, 100);
	thread_new("computing 7", computing_7, NULL, THREAD_PRIORITY_REAL_TIME, 100);
	thread_new("computing 8", computing_8, NULL, THREAD_PRIORITY_REAL_TIME, 100);
	thread_new("computing 9", computing_9, NULL, THREAD_PRIORITY_REAL_TIME, 100);
	thread_new("computing 10", computing_10, NULL, THREAD_PRIORITY_REAL_TIME, 100);
	thread_new("computing 11", computing_11, NULL, THREAD_PRIORITY_REAL_TIME, 100);
	thread_new("computing 12", computing_12, NULL, THREAD_PRIORITY_REAL_TIME, 100);
	thread_new("computing 13", computing_13, NULL, THREAD_PRIORITY_REAL_TIME, 100);
	thread_new("computing 14", computing_14, NULL, THREAD_PRIORITY_REAL_TIME, 100);
	thread_new("computing 15", computing_15, NULL, THREAD_PRIORITY_REAL_TIME, 100);
	thread_new("computing 16", computing_16, NULL, THREAD_PRIORITY_REAL_TIME, 100);
	thread_new("computing 17", computing_17, NULL, THREAD_PRIORITY_REAL_TIME, 100);
	
	// Start the kernel
	kernel_launch();
	
	while (1);
}



//--------------------------------------------------------------------------------------------------//