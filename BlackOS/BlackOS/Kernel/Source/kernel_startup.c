#include "kernel_startup.h"
#include "watchdog.h"
#include "interrupt.h"
#include "config.h"
#include "clock.h"
#include "flash.h"

#include "board_serial.h"
#include "board_button.h"

#include "systick.h"
#include "dynamic_memory.h"
#include "kernel.h"
#include "board_sd_card.h"
#include "file_system_command_line.h"
#include "file_system_fat.h"
#include "graphics_driver.h"
#include "dram.h"
#include "dma.h"

// Cache dependencies
#include <core_cm7.h>

//--------------------------------------------------------------------------------------------------//

void kernel_startup(void)
{
	// First we boot the chip and the peripherals
	SystemInit();
	
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
	
	// Configure the on board button
	board_button_config();
	
	// Print hello message
	board_serial_print("\n------------------ X Operating System ------------------\n\n");
		
	graphics_driver_config();
	
	
	// Now we start up the kernel and its components
	//kernel_thread_config();
	
	//file_system_command_line_config();
	SCB_CleanDCache();
	uint32_t* test_data = dynamic_memory_new_zero(DRAM_BANK_0, 24, 0x22);
	board_serial_print_percentage_symbol("Memory: ", dynamic_memory_get_used_percentage(DRAM_BANK_0), 1);
	SCB_CleanDCache();

	uint32_t* test_data_1 = dynamic_memory_new_zero(DRAM_BANK_0, 24, 0x33);
	board_serial_print_percentage_symbol("Memory: ", dynamic_memory_get_used_percentage(DRAM_BANK_0), 1);
	SCB_CleanDCache();

	uint32_t* test_data_2 = dynamic_memory_new_zero(DRAM_BANK_0, 24, 0x44);
	board_serial_print_percentage_symbol("Memory: ", dynamic_memory_get_used_percentage(DRAM_BANK_0), 1);
	SCB_CleanDCache();

	uint32_t* test_data_3 = dynamic_memory_new_zero(DRAM_BANK_0, 24, 0x55);
	board_serial_print_percentage_symbol("Memory: ", dynamic_memory_get_used_percentage(DRAM_BANK_0), 1);
	SCB_CleanDCache();

	uint32_t* test_data_4 = dynamic_memory_new_zero(DRAM_BANK_0, 24, 0x66);
	board_serial_print_percentage_symbol("Memory: ", dynamic_memory_get_used_percentage(DRAM_BANK_0), 1);
	SCB_CleanDCache();





	// Test
	uint32_t* test_data_5 = dynamic_memory_new_zero(DRAM_BANK_1, 24, 0x22);
	board_serial_print_percentage_symbol("Memory: ", dynamic_memory_get_used_percentage(DRAM_BANK_1), 1);
	SCB_CleanDCache();

	uint32_t* test_data_6 = dynamic_memory_new_zero(DRAM_BANK_1, 24, 0x33);
	board_serial_print_percentage_symbol("Memory: ", dynamic_memory_get_used_percentage(DRAM_BANK_1), 1);
	SCB_CleanDCache();

	uint32_t* test_data_7 = dynamic_memory_new_zero(DRAM_BANK_1, 24, 0x44);
	board_serial_print_percentage_symbol("Memory: ", dynamic_memory_get_used_percentage(DRAM_BANK_1), 1);
	SCB_CleanDCache();

	uint32_t* test_data_8 = dynamic_memory_new_zero(DRAM_BANK_1, 24, 0x55);
	board_serial_print_percentage_symbol("Memory: ", dynamic_memory_get_used_percentage(DRAM_BANK_1), 1);
	SCB_CleanDCache();

	uint32_t* test_data_9 = dynamic_memory_new_zero(DRAM_BANK_1, 24, 0x66);
	board_serial_print_percentage_symbol("Memory: ", dynamic_memory_get_used_percentage(DRAM_BANK_1), 1);
	SCB_CleanDCache();
	// Print hello message
	board_serial_print("\n------------------ X Operating System ------------------\n\n");

	dynamic_memory_free(test_data_6);
	board_serial_print_percentage_symbol("Memory: ", dynamic_memory_get_used_percentage(DRAM_BANK_1), 1);
	SCB_CleanDCache();
	
	dynamic_memory_free(test_data_7);
	board_serial_print_percentage_symbol("Memory: ", dynamic_memory_get_used_percentage(DRAM_BANK_1), 1);
	SCB_CleanDCache();

	

	dynamic_memory_free(test_data_2);
	board_serial_print_percentage_symbol("Memory: ", dynamic_memory_get_used_percentage(DRAM_BANK_0), 1);
	SCB_CleanDCache();


	uint32_t* test_data_10 = dynamic_memory_new_zero(DRAM_BANK_1, 8, 0xFF);
	board_serial_print("Free memory: %d\n", dynamic_memory_get_used_size(DRAM_BANK_0));
	SCB_CleanDCache();


	uint32_t* test_data_11 = dynamic_memory_new_zero(DRAM_BANK_0, 9, 0xFF);
	board_serial_print("Free memory: %d\n", dynamic_memory_get_used_size(DRAM_BANK_1));
	SCB_CleanDCache();
}

//--------------------------------------------------------------------------------------------------//