// Copyright (c) 2020 Bjørn Brodtkorb
//
// This software is provided "as is" without warranty of any kind.
// Permission is granted, free of charge, to copy and modify this
// software, if this copyright notice is included in all copies of
// the software.

#include "scheduler.h"
#include "systick.h"
#include "interrupt.h"
#include "dynamic_memory.h"
#include "board_serial_programming.h"
#include "check.h"
#include "core.h"
#include "board_serial.h"
#include "cache.h"


//--------------------------------------------------------------------------------------------------//


#include <stddef.h>
#include <core_cm7.h>


//--------------------------------------------------------------------------------------------------//


typedef enum
{
	SCHEDULER_STATUS_RUNNING,
	SCHEDULER_STATUS_SUSPENDED
} kernel_scheduler_status;


//--------------------------------------------------------------------------------------------------//


// The kernel tick count the number of milliseconds / time-slices that the kernel has run for
uint64_t kernel_tick;
uint64_t kernel_statistics_timer;

// We use three pointers for the kernel
//
//		- The idle pointer point to the idle thread, this will run when no other threads are in the running state
//		- The current pointer points to the current thread to be run
//		- The next pointer is set by the scheduler and points to the next thread to run
//
tcb_s* kernel_current_thread_pointer;
tcb_s* kernel_next_thread_pointer;
tcb_s* kernel_idle_thread_pointer;

// This variable will hold the current state of the scheduler
kernel_scheduler_status scheduler_status;

// Some lists that the kernel will use for the threads
list_s running_queue;
list_s delay_queue;
list_s suspended_list;

// Global tick to wake variable. This variable gets updated every time a thread is added or removed from the delay
// list. It holds the tick to wake value of the first thread to be put in the running queue again. This reduces the
// overhead.
uint64_t kernel_tick_to_wake;
uint64_t kernel_runtime_tick;


uint8_t reschedule_pending;
uint64_t reschedule_runtime;
uint32_t systick_divider;


void (*fast_program_interface_delete)(void);


//--------------------------------------------------------------------------------------------------//


uint32_t* thread_stack_init(uint32_t* stack_pointer, thread_function thread_func, void* param);

void round_robin_scheduler(void);

void scheduler_start(void);

void round_robin_idle_thread(void* param);

void thread_stack_overflow_event(char* data);		// Not implemented

void suspend_scheduler(void);

void resume_scheduler(void);

void reset_runtime(void);


//--------------------------------------------------------------------------------------------------//


tcb_s* thread_new(char* thread_name, thread_function thread_func, void* thread_parameter, kernel_thread_priority priority, uint32_t stack_size)
{
	// We do NOT want any scheduler interrupting inside here
	suspend_scheduler();
	
	// First we have to allocate memory for the thread and 
	// for the stack that is going to be used by that thread
	tcb_s* new_thread = (tcb_s*)dynamic_memory_new(DRAM_BANK_0, sizeof(tcb_s) + stack_size * sizeof(uint32_t));
	
	// Allocate the stack
	new_thread->stack_base = (uint32_t *)((uint8_t *)new_thread + sizeof(tcb_s));
	

	if ((new_thread == NULL) || (new_thread->stack_base == NULL))
	{
		// Allocation failed
		check(0);
		return NULL;
	}
	
	// Get the stack pointer to point to top of stack
	new_thread->stack_pointer = new_thread->stack_base + stack_size - 1;
	
	// Initialize the stack
	new_thread->stack_pointer = thread_stack_init(new_thread->stack_pointer, thread_func, thread_parameter);
	
	// Set the thread name
	for (uint32_t i = 0; i < KERNEL_THREAD_MAX_NAME_LENGTH; i++)
	{
		// We have reached the end of the string
		if (*thread_name == '\0')
		{
			break;
		}
		new_thread->name[i] = *thread_name++;
	}
	
	// Set the thread priority
	new_thread->priority = priority;
	new_thread->state = THREAD_STATE_RUNNING;
	new_thread->stack_size = 4 * stack_size;
	
	
	
	
	
	
	
	// The first thread to be made is the IDLE thread
	if (kernel_idle_thread_pointer == NULL)
	{
		// Just loop that single thread
		new_thread->next = new_thread;
		
		kernel_idle_thread_pointer = new_thread;
	}
	else
	{		
		new_thread->current_list = &running_queue;
		new_thread->next_list = NULL;
		new_thread->list_item.object = new_thread;
		
		list_insert_first(&(new_thread->list_item), &running_queue);
	}
	
	//cache_clean_addresses((uint32_t *)new_thread, sizeof(thread_s));
	//cache_clean_addresses((uint32_t *)new_thread->stack_base, stack_size);
	
	SCB_CleanDCache();
	
	resume_scheduler();
	
	return new_thread;
}


//--------------------------------------------------------------------------------------------------//


void scheduler_set_dynamic_loader_handler(void (*handler)(void))
{
	fast_program_interface_delete = handler;
}


//--------------------------------------------------------------------------------------------------//


void kernel_delete_thread(void)
{
	kernel_current_thread_pointer->state = THREAD_STATE_EXIT_PENDING;
	
	reschedule();
	
	while (1);
}


//--------------------------------------------------------------------------------------------------//


uint32_t* thread_stack_init(uint32_t* stack_pointer, thread_function thread_func, void* param)
{
	// Add a small offset
	stack_pointer--;
	
	// xPSR
	*stack_pointer-- = (uint32_t)KERNEL_THREAD_xPSR;
	
	// Program counter
	*stack_pointer-- = (uint32_t)thread_func;
	
	// Link register
	*stack_pointer = (uint32_t)kernel_delete_thread;
	
	// R12, R3, R2, R1, R0
	stack_pointer -= 5;
	*stack_pointer = (uint32_t)param;
	
	// R11, R10, R9, R8, R7, R6, R5, R4
	stack_pointer -= 8;
	
	return stack_pointer;
}


//--------------------------------------------------------------------------------------------------//


void print_running_queue(list_s* list)
{
	if (list->first == NULL)
	{
		board_serial_print("No elements in queue\n");
	}
	else
	{
		board_serial_print("- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - \n");
		if (list == &running_queue)
		{
			board_serial_print("Running queue:\t\t\t");
		}
		else
		{
			board_serial_print("Delay queue:\t\t\t");
		}
		list_node_s* list_iterator = list->first;
		board_serial_print("FIRST\t\t");
		while (list_iterator != NULL)
		{
			board_serial_print("%s\t\t", ((tcb_s *)(list_iterator->object))->name);
			
			list_iterator = list_iterator->next;
		}
		board_serial_print("LAST\n");
			board_serial_print("- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - \n");

		list_iterator = list->first;
		board_serial_print("Address\t\t\t\t\t");
		while (list_iterator != NULL)
		{
			board_serial_print_address("", (uint32_t)list_iterator);
			board_serial_print("\t");
			list_iterator = list_iterator->next;
		}
		
		
		list_iterator = list->first;
		board_serial_print("\n");
		board_serial_print("Next\t\t\t\t");
		board_serial_print_address("", (uint32_t)list->first);
		board_serial_print("\t");
		while (list_iterator != NULL)
		{
			board_serial_print_address("", (uint32_t)list_iterator->next);
			board_serial_print("\t");
			list_iterator = list_iterator->next;
		}
		
		
		list_iterator = list->first;
		board_serial_print("\n");
		board_serial_print("Previous\t\t\t\t\t");

		while (list_iterator != NULL)
		{
			board_serial_print_address("", (uint32_t)list_iterator->prev);
			board_serial_print("\t");
			list_iterator = list_iterator->next;
		}
		board_serial_print("\t");
		board_serial_print_address("", (uint32_t)list->last);
		board_serial_print("\n");
		
		
		list_iterator = list->first;
		board_serial_print("\n");
		board_serial_print("Previous\t\t\t\t\t");

		while (list_iterator != NULL)
		{
			board_serial_print("%d\t", ((tcb_s *)(list_iterator->object))->tick_to_wake);
			board_serial_print("\t");
			list_iterator = list_iterator->next;
		}
		board_serial_print("\n");
		
		
		board_serial_print("- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - \n");
		board_serial_print("\n\n");
	}
}


//--------------------------------------------------------------------------------------------------//


void kernel_launch(void)
{
	// Configure the SysTick timer
	systick_config();
	systick_set_reload_value(300000000 / KERNEL_TICK_FREQUENCY);
	
	
	systick_divider = 300000000 / KERNEL_TICK_FREQUENCY / 1000;
	
	// Start the scheduler
	scheduler_status = SCHEDULER_STATUS_RUNNING;
	
	
	// Update the kernel tick and tick to wake
	kernel_tick = 0;
	kernel_statistics_timer = 0;
	
	
	// Set the tick to wake variable to not trigger
	kernel_tick_to_wake = 0xffffffff;
	
	
	// Set the current thread to point to the first thread to run
	if (running_queue.last != NULL)
	{
		kernel_current_thread_pointer = running_queue.last->object;
		
		list_remove_last(&running_queue);
	}
	else
	{
		kernel_current_thread_pointer = kernel_idle_thread_pointer;
	}
	
	// Set the priorities for the SysTick and PendSV exception
	//
	// Under normal operation the SysTick exception should have the highest priority
	// and the PendSV should have the lowest. In debug mode this will make the system
	// crash. This is because the SysTick exception handler will print things to the
	// screen, and therefore not return within the new time slice. This means that
	// the scheduler runs several times without a context-switch.
	interrupt_enable_peripheral_interrupt(SysTick_IRQn, IRQ_LEVEL_0);
	interrupt_enable_peripheral_interrupt(PendSV_IRQn, IRQ_LEVEL_7);
	
	
	// This configures the kernels context switch mechanism
	// That includes configuring the new program stack
	scheduler_start();
}


//--------------------------------------------------------------------------------------------------//


void thread_config(void)
{
	// Interrupt should already be enabled
	interrupt_global_disable();
	
	// Add the idle thread on priority level 7 (lowest)
	thread_new("Idle", round_robin_idle_thread, NULL, THREAD_PRIORITY_NORMAL, KERNEL_IDLE_THREAD_STACK_SIZE);
}


//--------------------------------------------------------------------------------------------------//


void reschedule(void)
{
	reschedule_runtime = 1000 - systick_get_counter_value() / systick_divider;
	reschedule_pending = 1;
	
	// Not sure is this is a good idea, since it might fuck up the timing
	systick_set_counter_value(0);
	
	// Since the scheduling algorithm is called in the SysTick exception rather than
	// in the PendSV exception we pend the SysTick handler instead of the PendSV handler. 
	SCB->ICSR |= (1 << SCB_ICSR_PENDSTSET_Pos);
}


//--------------------------------------------------------------------------------------------------//


void thread_delay(uint32_t ticks)
{
	// Suspend the scheduler since we do not want
	// a context switch inside here
	suspend_scheduler();
	
	// Calculate the right tick to wake
	uint32_t tmp = kernel_tick + ticks * 1000;
	
	// Write the value to the thread control block
	kernel_current_thread_pointer->tick_to_wake = tmp;
	kernel_current_thread_pointer->list_item.value = tmp;
	
	// Update the next list to place the thread in
	kernel_current_thread_pointer->next_list = &delay_queue;
	
	// Let the scheduler start again
	resume_scheduler();
	
	// Free the resources used
	reschedule();
}


//--------------------------------------------------------------------------------------------------//


// This is the kernels scheduler which decide what thread to run next
// The next thread to run should be placed in the kernel_current_thread_pointer
// variable

void round_robin_scheduler(void)
{
	// Check the stack usage
	// Check the dynamic memory usage
	
	
	// Check the integrity of the scheduler
	volatile tcb_s* thread_pointer_check = kernel_current_thread_pointer;
	
	// Do not allow any context switch when the scheduler is suspended
	if (scheduler_status == SCHEDULER_STATUS_RUNNING)
	{
		if (kernel_current_thread_pointer != kernel_idle_thread_pointer)
		{
			if (kernel_current_thread_pointer->next_list != NULL)
			{
				// Check which list the thread is going to be placed in
				if (kernel_current_thread_pointer->next_list == &delay_queue)
				{
					list_insert_delay(&(kernel_current_thread_pointer->list_item), &delay_queue);
					
					// Update the kernel tick to wake
					kernel_tick_to_wake = ((tcb_s *)(delay_queue.first->object))->tick_to_wake;
				}
				
				kernel_current_thread_pointer->next_list = NULL;
			}
			else
			{
				
				if (kernel_current_thread_pointer->state == THREAD_STATE_EXIT_PENDING)
				{
					if (kernel_current_thread_pointer->ID == 6969)
					{
						// This thread belong to the fast programming interface. So we must
						// delete the program memory buffer as well
						if (fast_program_interface_delete != NULL)
						{
							fast_program_interface_delete();
						}
					}
					
					// Then we have to delete the memory resources
					//dynamic_memory_free(kernel_current_thread_pointer->stack_base);
					dynamic_memory_free(kernel_current_thread_pointer);
				}
				else
				{
					list_insert_first(&(kernel_current_thread_pointer->list_item), &running_queue);
				}
			}
		}
		
		// Now we check if some delays has expired
		if (kernel_tick_to_wake <= kernel_tick)
		{
			list_node_s* list_iterator = delay_queue.first;
			
			check(((tcb_s *)(delay_queue.first->object))->tick_to_wake <= kernel_tick_to_wake);
			
			uint16_t  i;
			
			for (i = 0; i < delay_queue.size; i++)
			{
				if (((tcb_s *)(list_iterator->object))->tick_to_wake > kernel_tick_to_wake)
				{
					break;
				}
				list_iterator = list_iterator->next;
			}
			
			for (uint16_t k = 0; k < i; k++)
			{
				list_node_s* tmp = delay_queue.first;
				
				list_remove_first(&delay_queue);
				list_insert_last(tmp, &running_queue);
			}
			
			if (list_iterator == NULL)
			{
				// No threads left in the queue
				kernel_tick_to_wake = 0xffffffff;
			}
			else
			{
				kernel_tick_to_wake = ((tcb_s *)(delay_queue.first->object))->tick_to_wake;
			}
		}
		
		// Update the next thread to run
		if (running_queue.last == NULL)
		{
			kernel_next_thread_pointer = kernel_idle_thread_pointer;
		}
		else
		{
			kernel_next_thread_pointer = running_queue.last->object;
			
			list_remove_last(&running_queue);
		}
	}
	else
	{
		// The scheduler is not running
	}
	
	// This function must leave the current thread pointer unchanged. If changed, the whole
	// kernel will crash, and the result will be undefined.
	check(thread_pointer_check == kernel_current_thread_pointer);
}


//--------------------------------------------------------------------------------------------------//


// The kernel does not use the SysTick handler for context switching
// since this would block all interrupts. Instead it is pending the PendSV
// interrupt, which has the lowest priority. This handler will take care
// of the context switching. 

void SysTick_Handler()
{
	if (reschedule_pending)
	{
		reschedule_pending = 0;
		
		kernel_tick += reschedule_runtime;
		kernel_runtime_tick += reschedule_runtime;
		kernel_current_thread_pointer->thread_time.new_window_time += reschedule_runtime;
	}
	else
	{
		kernel_tick += 1000;
		kernel_runtime_tick += 1000;
		kernel_current_thread_pointer->thread_time.new_window_time += 1000;
	}
	
	if (kernel_runtime_tick >= 1000000)
	{
		reset_runtime();
		kernel_runtime_tick = 0;
	}
	
	// Launch the scheduler
	round_robin_scheduler();
	
	// Pend the PendSV exception that will execute the actual context switch
	SCB->ICSR |= SCB_ICSR_PENDSVSET_Msk;
}


//--------------------------------------------------------------------------------------------------//


void reset_runtime(void)
{
	kernel_idle_thread_pointer->thread_time.window_time = kernel_idle_thread_pointer->thread_time.new_window_time;
	kernel_idle_thread_pointer->thread_time.new_window_time = 0;
	
	if (running_queue.size != 0)
	{
		for (list_node_s* i = running_queue.first; i != NULL; i = i->next)
		{
			((tcb_s *)(i->object))->thread_time.window_time = ((tcb_s *)(i->object))->thread_time.new_window_time;
			((tcb_s *)(i->object))->thread_time.new_window_time = 0;
		}
	}
	if (delay_queue.size != 0)
	{
		for (list_node_s* i = delay_queue.first; i != NULL; i = i->next)
		{
			((tcb_s *)(i->object))->thread_time.window_time = ((tcb_s *)(i->object))->thread_time.new_window_time;
			((tcb_s *)(i->object))->thread_time.new_window_time = 0;
		}
	}
}


//--------------------------------------------------------------------------------------------------//


void print_runtime_statistics(void)
{
	tcb_s* tmp_thread;
	
	board_serial_programming_print("Stack\tCPU\n");
	
	int32_t cpu_usage = 1000000 - kernel_idle_thread_pointer->thread_time.window_time;
	char k = cpu_usage / 10000;
	board_serial_programming_print("\t");
	board_serial_programming_write_percent(k, cpu_usage / 1000 - (k * 10));
	board_serial_programming_print(" : CPU");
	
	
	uint32_t use;
	uint32_t tot;
	
	use = dynamic_memory_get_used_size(DRAM_BANK_0);
	tot = dynamic_memory_get_total_size(DRAM_BANK_0);
	
	k = 100 * use / tot;
	uint32_t l = (100 * use) % tot;
	
	board_serial_programming_print("\t\t");
	board_serial_programming_write_percent(k, 100 * l / tot);
	board_serial_programming_print("DRAM Bank 0");
	
	use = dynamic_memory_get_used_size(DRAM_BANK_1);
	tot = dynamic_memory_get_total_size(DRAM_BANK_1);
	
	k = 100 * use / tot;
	l = (100 * use) % tot;
	
	board_serial_programming_print("\t\t");
	board_serial_programming_write_percent(k, 100 * l / tot);
	board_serial_programming_print("DRAM Bank 1");
	
	use = dynamic_memory_get_used_size(SRAM);
	tot = dynamic_memory_get_total_size(SRAM);
	
	k = 100 * use / tot;
	l = (100 * use) % tot;
	
	board_serial_programming_print("\t\t");
	board_serial_programming_write_percent(k, 100 * l / tot);
	board_serial_programming_print("SRAM");
	
	board_serial_programming_print("\n");
	
	if (running_queue.size != 0)
	{
		for (list_node_s* i = running_queue.first; i != NULL; i = i->next)
		{
			tmp_thread = (tcb_s *)(i->object);
			
			uint32_t used_stack = tmp_thread->stack_size - ((uint32_t)tmp_thread->stack_pointer - (uint32_t)tmp_thread->stack_base);
			k = used_stack * 100 / tmp_thread->stack_size;
			l = (used_stack * 100) % tmp_thread->stack_size;
			board_serial_programming_write_percent(k, 100 * l / tmp_thread->stack_size);
			board_serial_programming_print("\t");
			
			uint8_t tmp = tmp_thread->thread_time.window_time / 10000;
			board_serial_programming_write_percent(tmp, tmp_thread->thread_time.window_time / 1000 - (tmp * 10));
			board_serial_programming_print(" : %s", tmp_thread->name);
			board_serial_programming_print("\n");
		}
	}
	if (delay_queue.size != 0)
	{
		for (list_node_s* i = delay_queue.first; i != NULL; i = i->next)
		{
			tmp_thread = (tcb_s *)(i->object);
			
			uint32_t used_stack = tmp_thread->stack_size - ((uint32_t)tmp_thread->stack_pointer - (uint32_t)tmp_thread->stack_base);
			k = used_stack * 100 / tmp_thread->stack_size;
			l = (used_stack * 100) % tmp_thread->stack_size;
			board_serial_programming_write_percent(k, 100 * l / tmp_thread->stack_size);
			board_serial_programming_print("\t");
			
			uint8_t tmp = tmp_thread->thread_time.window_time / 10000;
			board_serial_programming_write_percent(tmp, tmp_thread->thread_time.window_time / 1000 - (tmp * 10));
			board_serial_programming_print(" : %s", tmp_thread->name);
			board_serial_programming_print("\n");
		}
	}
	board_serial_programming_print("\n\n");
}


//--------------------------------------------------------------------------------------------------//


void round_robin_idle_thread(void* param)
{
	while (1)
	{
		// The idle thread should not do anything
	}
}


//--------------------------------------------------------------------------------------------------//


void thread_stack_overflow_event(char* data)
{
	board_serial_print("Warning: Stack overflow on ");
	board_serial_print(data);
	board_serial_print(" thread\n");
}


//--------------------------------------------------------------------------------------------------//


void suspend_scheduler(void)
{
	(void)scheduler_status;
	scheduler_status = SCHEDULER_STATUS_SUSPENDED;
}


//--------------------------------------------------------------------------------------------------//


void resume_scheduler(void)
{
	(void)scheduler_status;
	scheduler_status = SCHEDULER_STATUS_RUNNING;
}


//--------------------------------------------------------------------------------------------------//