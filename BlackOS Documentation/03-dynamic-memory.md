# Dynamic Memory

Dynamic memory is used when an execution program asks the operating system to give it a block of the main memory. This allocation happends at runtime, which means the the kernel does not know about the allocation until it happends. There are many different implementations of dynamic memory, each has some advantages and some disadvantages. The allocation algorithm used in the BlackOS is based upon explicit free lists algorithm. 

## Introduction

The dynamic memory algorithm is based upon continous tracking of free memory blocks. A memory block is a pysical unit of memory, and all the memory blocks forms a linked list. The list is sorted by address, whith the lowest address first. Allocated memory is removed from the list, and is not tracked by the kernel, while memory that is freed gets insterted into the list and gets tracked by the kernel. After reset, the memory consists of one big free block of memory. When the user asks for memory, the kernel starts iterating through the list of free blocks. The kernel stops when it finds a free block that is large enough to hold the requested memory. If the requested memory is less than the free block, the kernel will try to split the free block. The bottom part is removed from the list (allocated), and the top part becomes a new free block. This only happends if the remaining top block is large enough to contain a nwe allocation. 

When the memory is beeing freed, the kernel just inserts is into the list of free blocks. The kernel will check the previous block and the next block, and check if any of them overlaps with the inserted block. In this is the case, they will be combined into a single large block.

# Memory layout

The processor can have several memory sources. In our case we have the internal SRAM and four external DRAM banks. Due to refresh penalties in the dynamic memory, code and data should be placed in different banks. This is of course somthing that the dynamic memory must take into acccount. It must be able to allocate data in a specified memory section. 

The dynamic memory starts by specifying some memory section. A memory section is configured by doing three things. First the start address, stop address, allignment and minimum block size is put into a struct. Then this struct are placed in an array (which the kernel use). Lastly the memory section must be referenced in an enum. This enum is passed as an argument to the allocation function. 

This is the struct that specifies the parameters for the memory section
```c
Dynamic_memory_section_s dynamic_section_dram_bank_0 = 
{
	.start_address		= 0x70000000,
	.end_address		= 0x7007FFFF,
	.allignment		= 8,
	.minimum_block_size     = 8,
	.name			= "DRAM bank 0"
};
```

The struct is then added to this list so that the kernel can reference it
```c
Dynamic_memory_section_s* dynamic_memory_sections[] = 
{
	&dynamic_section_sram,
	&dynamic_section_dram_bank_0,
	&dynamic_section_dram_bank_1,
	NULL
};
```

This is used by the calling application to ensure that the data is allocated in the right section
```c
typedef enum
{
	SRAM,
	DRAM_BANK_0,
	DRAM_BANK_1
} Dynamic_memory_section;
```

## Memory descriptor

Before every memory block is a memory descriptor. The descriptor consists of 8 bytes, and contains information about the memory block. The first four bytes points to another memory descriptor. The last four bytes contains the size. Since the block size never will exceed a 32-bits number, we can use the upper bits to store aditional information. Bit 31 determines if the memory block is used or free. Bits [30:28] stores which memory section that is used. Since the memory free function only takes in a pointer to the memory, we have to determine from which memory section we should free the memory. Bit [30:28] tells us that. If a memory block is free, the memory descriptor will point to the next free block. If the memory block is used the memory descriptor will contain the NULL pointer. 

## Configuration

After reset the kernel initializes all memory sections. After the configuration all the sections should consist of one large free memory block. The start address and end address is first alligned with the preffered allignment. The whole memory section is then written to zero. The memory section descriptor should provide information about the memory. This includes a pointer to the first element in the linked list of free blocks.

## Allocation

## Free

## Memory leaks

Every memory block starts with 8 bytes of information. This is called a memory descriptor and contains the block size in number of bytes, and a pointer to another block. In order to speed up the allocation time, only free blocks are tracked. One of the features required by the kernel, is the ability to allocate memory in a spesific section. This is due to DRAM access rates being slow when accessing different banks. 

In order to use the dynamic memory it must be configured first. The includes initialization of all the different memory sections (DRAM BANK0, DRAM BANK1, SRAM, osv.). The memory should be reset to zero, and the descriptors should be updated. More info later. After the configuration each memory section consists of one big block of free memory 
