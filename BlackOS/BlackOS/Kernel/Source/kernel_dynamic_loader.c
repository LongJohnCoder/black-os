#include "kernel_dynamic_loader.h"
#include "kernel_thread.h"
#include "config.h"

#include <stddef.h>

void kernel_dynamic_loader_run(uint32_t* data, uint32_t size)
{
	// Start with relocating the .GOT and .GOT PLT table addresses
	kernel_dynamic_loader_relocate(data);
	
	// This is the address that contains the entry for the program
	uint8_t* program_entry = (uint8_t *)((uint8_t *)data + *(data + 1));
	
	// The processor supports only thumb mode so we have to make sure bit 1 in the program counter
	// is set to one. Otherwise we will get a hard fault.
	program_entry = (uint8_t *)((uint32_t)program_entry | 1);
	
	// Add offset
	data += 6;
	
	// Get the stack size, name length and name pointer
	uint32_t stack_size = (uint32_t)(*data++);
	uint32_t name_length = (uint32_t)(*data++);
	char* name = (char *)data;
	
	// Check if the name if valid and start the thread
	if (kernel_dynamic_loader_check_name(name, name_length))
	{
		kernel_add_thread(name, (thread_function_pointer)program_entry, NULL, THREAD_LEVEL_4, stack_size);
	}

}

uint8_t kernel_dynamic_loader_check_name(char* data, uint32_t size)
{
	if (size > KERNEL_THREAD_MAX_NAME_LENGTH)
	{
		
		return 0;
	}
	
	for (uint32_t i = 0; i < size; i++)
	{
		if (*data++ == '\0')
		{
			return 1;
		}
	}
	
	return 0;
}

void kernel_dynamic_loader_relocate(uint32_t* data)
{
	// This functions will resolve the absolute addressing in the application.
	// It will relocate the entries in the .GOT and .GOT PLT section
	
	uint8_t* program_start = (uint8_t *)data;
	
	// Skip the application entry
	data++;
	data++;
	
	// Get the offsets to the .GOT table
	uint8_t* start_got = program_start + *data++;
	uint8_t* end_got = program_start + *data++;
	
	// Get the offsets to the .GOT PLT table
	uint8_t* start_got_plt = program_start + *data++;
	uint8_t* end_got_plt = program_start + *data++;
	
	
	// Resolve .GOT addressed by performing dynamic linking
	for (uint32_t* relocate_interator = (uint32_t *)start_got; relocate_interator < (uint32_t *)end_got; relocate_interator++)
	{
		// Add the global offset to each entry
		*relocate_interator += (uint32_t)program_start;
	}
	
	// Resolve .GOT PLT addressed by performing dynamic linking
	for (uint32_t* relocate_interator = (uint32_t *)start_got_plt; relocate_interator < (uint32_t *)end_got_plt; relocate_interator++)
	{
		// Add the global offset to each entry
		*relocate_interator += (uint32_t)program_start;
	}
}