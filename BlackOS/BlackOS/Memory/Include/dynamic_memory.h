#ifndef DYNAMIC_MEMORY_H
#define DYNAMIC_MEMORY_H

#include "sam.h"
#include "config.h"

// This dynamic memory implementation uses a lightweight algorithm and is optimized for
// allocations across multiple sections. The user can request dynamic memory and specify where to
// put it, and the memory driver will automatically allocate memory in that section. To delete
// memory only the pointer is required.
//
// This implementation uses only free blocks to navigate in memory, thus increasing allocation
// speed. It combines adjacent memory block to prevent memory fragmentation.

// This is the dynamic memory descriptor
// Every block that is allocated or freed will start with this descriptor
typedef struct dynamic_memory_descriptor_s
{
	// Pointer to next element in the linked list
	struct dynamic_memory_descriptor_s* next;
	
	// Size of the memory block
	uint32_t size;
		
} dynamic_memory_descriptor;



// List of sections with dynamic memory support
// This will be the input to all dynamic memory functions
typedef enum
{
	SRAM,
	DRAM_BANK_0,
	DRAM_BANK_1
} Dynamic_memory_section;


// Dynamic memory section descriptor
typedef struct
{
	// Name of the section
	char name[DYNAMIC_MEMORY_SECTION_NAME_SIZE];
	
	// The section addresses does not need to be aligned
	// since this is handled by software
	uint32_t start_address;
	uint32_t end_address;
	
	// Specifies the total and free size
	uint32_t total_memory;
	uint32_t free_memory;
	
	// Configuration for the dynamic memory section
	uint32_t minimum_block_size;
	uint32_t allignment;
	
	// Define descriptors
	dynamic_memory_descriptor start_descriptor_object;
	dynamic_memory_descriptor* start_descriptor;
	dynamic_memory_descriptor* end_descriptor;
	
} Dynamic_memory_section_s;



void dynamic_memory_config(void);

void* dynamic_memory_new(Dynamic_memory_section memory_section, uint32_t size);

void* dynamic_memory_new_zero(Dynamic_memory_section memory_section, uint32_t size);

void dynamic_memory_free(void* memory_object);



uint32_t dynamic_memory_get_total_size(Dynamic_memory_section memory_section);

uint32_t dynamic_memory_get_used_size(Dynamic_memory_section memory_section);

uint32_t dynamic_memory_get_free_size(Dynamic_memory_section memory_section);

uint8_t dynamic_memory_get_used_percentage(Dynamic_memory_section memory_section);

#endif