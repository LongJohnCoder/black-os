// Copyright (c) 2020 Bj�rn Brodtkorb
//
// This software is provided "as is" without warranty of any kind.
// Permission is granted, free of charge, to copy and modify this
// software, if this copyright notice is included in all copies of
// the software.

#ifndef SCHEDULER_H
#define SCHEDULER_H


//--------------------------------------------------------------------------------------------------//


#include "sam.h"
#include "config.h"
#include "list.h"


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


typedef struct Thread_time_s
{
	// Total runtime in microseconds
	uint64_t total_runtime;
	
	// Window run time in microseconds - typically calculated over a 1 second interval
	uint32_t w_runtime;
	uint32_t w_new_runtime;
	
} thread_time_s;


//--------------------------------------------------------------------------------------------------//


typedef struct Tcb_s
{
	// Points to the top of the stack
	uint32_t*					stack_pointer;
	
	
	// Points to the next thread in the list
	struct Tcb_s*				next;
	
	
	// We use the list interface
	list_node_s					list_item;
	
	
	struct List_s*				current_list;
	struct List_s*				next_list;


	// Pointer to the stack base so that we can delete the memory
	uint32_t*					stack_base;
	uint32_t					stack_size;
	
	
	// Priority of the thread
	kernel_thread_priority		priority;
	
	
	// Time to wake is used for the thread delay function
	uint64_t					tick_to_wake;
	
	
	// These variables are used to calculate the runtime statistics
	uint64_t					last_runtime;
	uint64_t					runtime;
	
	
	thread_state				state;
	
	
	// New section
	
	// This section deals with timing and stuff
	thread_time_s				time;
	
	
	// Store the name of the thread
	char						name[KERNEL_THREAD_MAX_NAME_LENGTH];
	
} tcb_s;


//--------------------------------------------------------------------------------------------------//


void kernel_start(void);

void kernel_thread_config(void);

tcb_s* kernel_add_thread(char* thread_name, thread_function thread_func, void* thread_parameter, kernel_thread_priority priority, uint32_t stack_size);

void kernel_reschedule(void);

void kernel_thread_delay(uint32_t ticks);


//--------------------------------------------------------------------------------------------------//


void kernel_print_running_queue(list_s* list);

void kernel_print_runtime_statistics(void);


//--------------------------------------------------------------------------------------------------//


#endif