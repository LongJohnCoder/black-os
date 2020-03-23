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
	
	while (1)
	{
		asm volatile ("nop");
	}
}


//--------------------------------------------------------------------------------------------------//


void computing_1(void* arg)
{
	while (1)
	{
		asm volatile ("nop");

	}
}


//--------------------------------------------------------------------------------------------------//


void computing_2(void* arg)
{
	while (1)
	{
		asm volatile ("nop");
	}
}


//--------------------------------------------------------------------------------------------------//


void computing_3(void* arg)
{
	while (1)
	{
		asm volatile ("nop");
	}
}


//--------------------------------------------------------------------------------------------------//


void computing_4(void* arg)
{
	while (1)
	{
		asm volatile ("nop");

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
	thread_new("waveform", waveform_1, NULL, THREAD_PRIORITY_REAL_TIME, 100);
	thread_new("waveform", waveform_2, NULL, THREAD_PRIORITY_REAL_TIME, 100);
	thread_new("waveform", waveform_3, NULL, THREAD_PRIORITY_REAL_TIME, 100);
	thread_new("waveform", waveform_4, NULL, THREAD_PRIORITY_REAL_TIME, 100);
	thread_new("waveform", waveform_5, NULL, THREAD_PRIORITY_REAL_TIME, 100);
	thread_new("waveform", waveform_6, NULL, THREAD_PRIORITY_REAL_TIME, 100);


	//thread_new("computing", computing_1, NULL, THREAD_PRIORITY_REAL_TIME, 100);
	//thread_new("computing", computing_2, NULL, THREAD_PRIORITY_REAL_TIME, 100);
	//thread_new("computing", computing_3, NULL, THREAD_PRIORITY_REAL_TIME, 100);
	//thread_new("computing", computing_4, NULL, THREAD_PRIORITY_REAL_TIME, 100);
	
	// Start the kernel
	kernel_launch();
	
	while (1);
}



//--------------------------------------------------------------------------------------------------//