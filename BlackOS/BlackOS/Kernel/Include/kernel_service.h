#ifndef KERNEL_SERVICE_H
#define KERNEL_SERVICE_H

#include "sam.h"

#define KERNEL_SERVICE_NOINLINE __attribute__((noinline))

// Service numbers
// Service 1 to 10 is reserved for the kernel
// Service 11-255 is free to use

typedef enum
{
	SERVICE_THREAD_DELAY = 1,
	SERVICE_SERIAL_PRINT = 10,
	SERVICE_DISPLAY_FLUSH = 11,
	SERVICE_DISPLAY_WRITE_FRAMEBUFFER = 12,
	SERVICE_DISAPLY_SCROLL = 13,
	SERVICE_GET_PIN_VALUE = 14
} kernel_services;

// Prototypes for the services that the kernel provide
void KERNEL_SERVICE_NOINLINE service_serial_print(char* data);

void KERNEL_SERVICE_NOINLINE service_thread_delay(uint32_t ticks);

void KERNEL_SERVICE_NOINLINE serivice_display_flush(void);

void KERNEL_SERVICE_NOINLINE service_display_write_frame_buffer(uint16_t x, uint16_t y, uint16_t color);

void KERNEL_SERVICE_NOINLINE service_display_scroll(uint16_t amount);


#endif