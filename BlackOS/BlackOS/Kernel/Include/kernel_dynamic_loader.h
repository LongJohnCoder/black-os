#ifndef KERNEL_DYNAMIC_LOADER_H
#define KERNEL_DYNAMIC_LOADER_H

#include "sam.h"

void kernel_dynamic_loader_run(uint32_t* data, uint32_t size);

uint8_t kernel_dynamic_loader_check_name(char* data, uint32_t size);

void kernel_dynamic_loader_relocate(uint32_t* data);

#endif