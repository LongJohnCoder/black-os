#ifndef KERNEL_THREAD_H
#define KERNEL_THREAD_H

#include "sam.h"
#include "config.h"

// Function pointer to a thread
typedef void (*thread_function_pointer)(void *);

// Thread priorities
typedef enum
{
	THREAD_LEVEL_0,
	THREAD_LEVEL_1,
	THREAD_LEVEL_2,
	THREAD_LEVEL_3,
	THREAD_LEVEL_4,
	THREAD_LEVEL_5,
	THREAD_LEVEL_6,
	THREAD_LEVEL_7,
} kernel_thread_priority;

// Just a forward declaration
struct kernel_list_s;

typedef struct kernel_list_item_s
{
	// Pointers to the next and previous element
	struct kernel_list_item_s* next;
	struct kernel_list_item_s* previous;

	// Pointer to the thread
	struct kernel_thread_control_s* thread_control;
	
} kernel_list_item;

typedef struct kernel_list_s
{
	// Pointer to the first and last element in the list
	struct kernel_list_item_s* first;
	struct kernel_list_item_s* last;
	
	// Keep track of the size of the list
	uint16_t size;
	
} kernel_list;

// Thread control structure
typedef struct kernel_thread_control_s
{
	// Points to the top of the stack
	uint32_t* stack_pointer;
	
	// Points to the next thread in the list
	struct kernel_thread_control_s* next;
	
	// We use the list interface
	struct kernel_list_item_s list_item;
	
	struct kernel_list_s* current_list;
	struct kernel_list_s* next_list;

	// Pointer to the stack base so that we can delete the memory
	uint32_t* stack_base;
	uint32_t stack_size;
	
	// Priority of the thread
	kernel_thread_priority priority;
	
	// Time to wake is used for the thread delay function
	uint32_t tick_to_wake;
	
	// The runtime variable will hold the total ticks the last second
	uint32_t last_runtime;
	uint32_t runtime;
	
	// Store the name of the thread
	char name[KERNEL_THREAD_MAX_NAME_LENGTH];
	
} kernel_thread_control;

extern kernel_list running_queue;
extern kernel_thread_control* kernel_current_thread_pointer;
extern kernel_list delay_queue;

void kernel_start(void);

void kernel_thread_config(void);

kernel_thread_control* kernel_add_thread(char* thread_name, thread_function_pointer thread_func, void* thread_parameter, kernel_thread_priority priority, uint32_t stack_size);

void kernel_thread_yield(void);

void kernel_thread_delay(uint32_t ticks);


void kernel_list_insert_first(kernel_list_item* list_item, kernel_list* list);
void kernel_list_insert_last(kernel_list_item* list_item, kernel_list* list);
void kernel_list_insert_delay(kernel_list_item* list_item, kernel_list* list);

uint8_t kernel_list_remove_first(kernel_list* list);
uint8_t kernel_list_remove_last(kernel_list* list);
uint8_t kernel_list_remove_item(kernel_list_item* list_item, kernel_list* list);


void kernel_print_running_queue(kernel_list* list);

void kernel_print_runtime_statistics(void);

#endif