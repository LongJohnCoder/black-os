// Copyright (c) 2020 Bjørn Brodtkorb
//
// This software is provided "as is" without warranty of any kind.
// Permission is granted, free of charge, to copy and modify this
// software, if this copyright notice is included in all copies of
// the software.

#ifndef KERNEL_THREAD_H
#define KERNEL_THREAD_H


//--------------------------------------------------------------------------------------------------//


#include "sam.h"
#include "config.h"


//--------------------------------------------------------------------------------------------------//


// Function pointer to a thread
typedef void (*thread_function)(void *);


//--------------------------------------------------------------------------------------------------//


// Thread priorities
typedef enum
{
	THREAD_PRIORITY_LOW,
	THREAD_PRIORITY_NORMAL,
	THREAD_PRIORITY_HIGH,
	THREAD_PRIORITY_EXTRA_HIGH
} kernel_thread_priority;



typedef enum
{
	THREAD_STATE_SUSPENDED,
	THREAD_STATE_EXIT_PENDING,
	THREAD_STATE_RUNNING
} thread_state;


//--------------------------------------------------------------------------------------------------//


typedef struct List_node_s
{
	// Pointers to the next and previous element
	struct List_node_s* next;
	struct List_node_s* prev;


	uint64_t value;


	// Pointer to the thread
	void* object;
	
} list_node_s;


//--------------------------------------------------------------------------------------------------//


typedef struct List_s
{
	// Pointer to the first and last element in the list
	struct List_node_s* first;
	struct List_node_s* last;
	
	
	// Keep track of the size of the list
	uint16_t size;
	
} list_s;


//--------------------------------------------------------------------------------------------------//


typedef struct kernel_thread_control_s
{
	// Points to the top of the stack
	uint32_t* stack_pointer;
	
	
	// Points to the next thread in the list
	struct kernel_thread_control_s* next;
	
	
	// We use the list interface
	struct List_node_s list_item;
	
	
	struct List_s* current_list;
	struct List_s* next_list;


	// Pointer to the stack base so that we can delete the memory
	uint32_t* stack_base;
	uint32_t stack_size;
	
	
	// Priority of the thread
	kernel_thread_priority priority;
	
	
	// Time to wake is used for the thread delay function
	uint32_t tick_to_wake;
	
	
	// These variables are used to calculate the runtime statistics
	uint32_t last_runtime;
	uint32_t runtime;
	
	
	thread_state state;
	
	
	// Store the name of the thread
	char name[KERNEL_THREAD_MAX_NAME_LENGTH];
	
} thread_s;


//--------------------------------------------------------------------------------------------------//


extern list_s running_queue;
extern thread_s* kernel_current_thread_pointer;
extern list_s delay_queue;


//--------------------------------------------------------------------------------------------------//


void kernel_start(void);

void kernel_thread_config(void);

thread_s* kernel_add_thread(char* thread_name, thread_function thread_func, void* thread_parameter, kernel_thread_priority priority, uint32_t stack_size);

void kernel_reschedule(void);

void kernel_thread_delay(uint32_t ticks);


//--------------------------------------------------------------------------------------------------//


void kernel_list_insert_first(list_node_s* list_item, list_s* list);

void kernel_list_insert_last(list_node_s* list_item, list_s* list);

void kernel_list_insert_delay(list_node_s* list_item, list_s* list);

uint8_t kernel_list_remove_first(list_s* list);

uint8_t kernel_list_remove_last(list_s* list);

uint8_t kernel_list_remove_item(list_node_s* list_item, list_s* list);


//--------------------------------------------------------------------------------------------------//


void kernel_print_running_queue(list_s* list);

void kernel_print_runtime_statistics(void);


//--------------------------------------------------------------------------------------------------//


#endif