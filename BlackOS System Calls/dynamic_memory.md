# Dynamic Memory Calls

The dynamic memory core provides the user with functions for dynamically allocation and deleting memory. These functions are avalible though the system call layer. Since the allocation and deallocation used only a fraction of a timeslice, the whole function finishes inside the system call interrupt.

## Functions

```c
void* dynamic_memory_new(Dynamic_memory_section memory_section, uint32_t size);

void* dynamic_memory_new_zero(Dynamic_memory_section memory_section, uint32_t size);

void dynamic_memory_free(void* memory_object);

uint32_t dynamic_memory_get_total_size(Dynamic_memory_section memory_section);

uint32_t dynamic_memory_get_used_size(Dynamic_memory_section memory_section);

uint32_t dynamic_memory_get_free_size(Dynamic_memory_section memory_section);

uint8_t dynamic_memory_get_used_percentage(Dynamic_memory_section memory_section);
```

## Description

### Sections

The avalible sections in the default configuration is 
- SRAM
- DRAM_BANK_0
- DRAM_BANK_1

### New memory

memory_section: the requested memory section to allocate memory from
size: size in bytes of the requested memory

```c
void* dynamic_memory_new(Dynamic_memory_section memory_section, uint32_t size);
```
