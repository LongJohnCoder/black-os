// Copyright (c) 2020 Bjørn Brodtkorb
//
// This software is provided "as is" without warranty of any kind.
// Permission is granted, free of charge, to copy and modify this
// software, if this copyright notice is included in all copies of
// the software.

#include "kernel_startup.h"
#include "watchdog.h"
#include "interrupt.h"
#include "config.h"
#include "clock.h"
#include "flash.h"

#include "board_serial.h"
#include "board_button.h"
#include "board_serial_programming.h"

#include "systick.h"
#include "dynamic_memory.h"
#include "kernel.h"
#include "board_sd_card.h"
#include "file_system_command_line.h"
#include "file_system_fat.h"
#include "graphics_driver.h"
#include "dram.h"
#include "dma.h"


//--------------------------------------------------------------------------------------------------//


// Cache dependencies
#include <core_cm7.h>


//--------------------------------------------------------------------------------------------------//


void kernel_startup(void)
{
	// First we boot the chip and the peripherals
	//SystemInit();
	
	// Disable interrupt under system configuration
	interrupt_global_disable();
	
	systick_disable();
	
	// Disable watchdog
	watchdog_disable();
	
	// Set flash wait states before boosting the clock
	flash_set_wait_states(FLASH_NUMBER_OF_WAIT_STATES);
	
	// Configure the clock network
	clock_sources_config(CLOCK_SOURCE_CRYSTAL, CLOCK_CRYSTAL_STARTUP_TIME);
	clock_main_clock_config(CLOCK_SOURCE_CRYSTAL);
	clock_pll_config(CLOCK_PLLA_MULTIPLIER, CLOCK_PLLA_DIVISION, CLOCK_PLLA_STARTUP_TIME);
	clock_master_clock_config(CLOCK_MASTER_CLOCK_SOURCE_PLLA_CLOCK, CLOCK_MASTER_CLOCK_PRESCALER_1, CLOCK_MASTER_CLOCK_DIVISION_DIV_2);

	// Enable I-cache and D-cache
	SCB_EnableICache();
	SCB_EnableDCache();
	SCB_CleanInvalidateDCache();
	
	// Start the dynamic memory
	// WARNING: Hard fault will occur if the dynamic memory is not configured
	dram_config();
	dynamic_memory_config();
	
	// Configure the DMA core
	dma_config();

	// Critical section ended so we enable interrupt
	// Note the the kernel_config will turn off interrupt
 	interrupt_global_enable();	
	
	// Start the serial interface
	board_serial_config();
	board_serial_programming_config();
	
	// Configure the on board button
	board_button_config();
	
	// Print hello message
	board_serial_print("\n------------------ X Operating System ------------------\n\n");
		
	//graphics_driver_config();

	
	// Now we start up the kernel and its components
	kernel_thread_config();
	
	file_system_command_line_config();
}


//--------------------------------------------------------------------------------------------------//