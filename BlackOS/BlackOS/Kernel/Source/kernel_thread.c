#include "kernel_thread.h"
#include "systick.h"
#include "interrupt.h"
#include "dynamic_memory.h"

#include "board_serial_x.h"

#include "check.h"
#include "core.h"

#include <stddef.h>
#include <core_cm7.h>

#define KERNEL_DEBUG 0

//--------------------------------------------------------------------------------------------------//

typedef enum
{
	SCHEDULER_STATUS_RUNNING,
	SCHEDULER_STATUS_SUSPENDED
} kernel_scheduler_status;

//--------------------------------------------------------------------------------------------------//

// The kernel tick count the number of milliseconds / time-slices that the kernel has run for
uint32_t kernel_tick;
uint32_t kernel_statistics_timer;

// We use three pointers for the kernel
//
//		- The idle pointer point to the idle thread, this will run when no other threads are in the running state
//		- The current pointer points to the current thread to be run
//		- The next pointer is set by the scheduler and points to the next thread to run
//
kernel_thread_control* kernel_current_thread_pointer;
kernel_thread_control* kernel_next_thread_pointer;
kernel_thread_control* kernel_idle_thread_pointer;

// This variable will hold the current state of the scheduler
kernel_scheduler_status scheduler_status;

// Some lists that the kernel will use for the threads
kernel_list running_queue;
kernel_list delay_queue;

// Global tick to wake variable. This variable gets updated every time a thread is added or removed from the delay
// list. It holds the tick to wake value of the first thread to be put in the running queue again. This reduces the
// overhead.
uint32_t kernel_tick_to_wake;
uint32_t kernel_runtime_tick;

//--------------------------------------------------------------------------------------------------//

uint32_t* kernel_thread_stack_init(uint32_t* stack_pointer, thread_function_pointer thread_func, void* param);

void kernel_scheduler(void);

void kernel_scheduler_start(void);

void kernel_idle_thread(void* param);

void kernel_thread_stack_overflow_event(char* data);		// Not implemented

void kernel_suspend_scheduler(void);

void kernel_resume_scheduler(void);

void kernel_reset_runtime(void);

//--------------------------------------------------------------------------------------------------//

kernel_thread_control* kernel_add_thread(char* thread_name, thread_function_pointer thread_func, void* thread_parameter, kernel_thread_priority priority, uint32_t stack_size)
{
	// We do NOT want any scheduler interrupting inside here
	kernel_suspend_scheduler();
	
	// First we have to allocate memory for the thread and 
	// for the stack that is going to be used by that thread
	kernel_thread_control* new_thread = (kernel_thread_control*)dynamic_memory_new(SRAM, sizeof(kernel_thread_control));
	
	// Allocate the stack
	new_thread->stack_base = (uint32_t*)dynamic_memory_new(SRAM, stack_size * sizeof(uint32_t));

	if ((new_thread == NULL) || (new_thread->stack_base == NULL))
	{
		// Allocation failed
		check(0);
		return NULL;
	}
	
	// Get the stack pointer to point to top of stack
	new_thread->stack_pointer = new_thread->stack_base + stack_size - 1;
	
	// Initialize the stack
	new_thread->stack_pointer = kernel_thread_stack_init(new_thread->stack_pointer, thread_func, thread_parameter);
	
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
	
	// Set the tick_to_wake to zero since otherwise the task will be delays
	// TODO: This should not be necessary
	new_thread->tick_to_wake = 0;
	
	// Set the thread priority
	new_thread->priority = priority;
	
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
		new_thread->list_item.thread_control = new_thread;
		
		kernel_list_insert_first(&(new_thread->list_item), &running_queue);
	}
	
	board_serial_print_percentage_symbol("Memory: ", dynamic_memory_get_used_percentage(SRAM), 1);
	
	SCB_CleanDCache();
	
	kernel_resume_scheduler();
	
	return new_thread;
}

//--------------------------------------------------------------------------------------------------//

uint32_t* kernel_thread_stack_init(uint32_t* stack_pointer, thread_function_pointer thread_func, void* param)
{
	// Add a small offset
	stack_pointer--;
	
	// xPSR
	*stack_pointer-- = (uint32_t)KERNEL_THREAD_xPSR;
	
	// Program counter
	*stack_pointer-- = (uint32_t)thread_func;
	
	// Link register
	*stack_pointer = (uint32_t)KERNEL_THREAD_LR;
	
	// R12, R3, R2, R1, R0
	stack_pointer -= 5;
	*stack_pointer = (uint32_t)param;
	
	// R11, R10, R9, R8, R7, R6, R5, R4
	stack_pointer -= 8;
	
	return stack_pointer;
}

//--------------------------------------------------------------------------------------------------//

void kernel_print_running_queue(kernel_list* list)
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
		kernel_list_item* list_iterator = list->first;
		board_serial_print("FIRST\t\t");
		while (list_iterator != NULL)
		{
			board_serial_print("%s\t\t", list_iterator->thread_control->name);
			
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
			board_serial_print_address("", (uint32_t)list_iterator->previous);
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
			board_serial_print("%d\t", list_iterator->thread_control->tick_to_wake);
			board_serial_print("\t");
			list_iterator = list_iterator->next;
		}
		board_serial_print("\n");
		
		
		board_serial_print("- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - \n");
		board_serial_print("\n\n");
	}
}

//--------------------------------------------------------------------------------------------------//

void kernel_start(void)
{
	// Configure the SysTick timer
	systick_config();
	systick_set_reload_value(300000000 / KERNEL_TICK_FREQUENCY);
	
	// Start the scheduler
	scheduler_status = SCHEDULER_STATUS_RUNNING;
	
	// Update the kernel tick and tick to wake
	kernel_tick = 0;
	kernel_statistics_timer = 0;
	
	// Set the tick to wake variabe to not trigger
	kernel_tick_to_wake = 0xffffffff;
	
	// Set the current thread to point to the first thread to run
	if (running_queue.last != NULL)
	{
		kernel_current_thread_pointer = running_queue.last->thread_control;
	}
	else
	{
		kernel_current_thread_pointer = kernel_idle_thread_pointer;
	}
	
	// DEBUG
	kernel_print_running_queue(&running_queue);
	kernel_print_running_queue(&delay_queue);
	
	// Set the priorities for the SysTick and PendSV exception
	//
	// Under normal operation the SysTick exception should have the highest priority
	// and the PendSV should have the lowest. In debug mode this will make the system
	// crash. This is because the systick exception handler will print things to the
	// screen, and therefore not return whithin the new timeslice. This means that
	// the scheduler runs several times without a context-switch.
	#if KERNEL_DEBUG
	interrupt_enable_peripheral_interrupt(SysTick_IRQn, IRQ_LEVEL_7);
	interrupt_enable_peripheral_interrupt(PendSV_IRQn, IRQ_LEVEL_6);
	#else
	interrupt_enable_peripheral_interrupt(SysTick_IRQn, IRQ_LEVEL_7);
	interrupt_enable_peripheral_interrupt(PendSV_IRQn, IRQ_LEVEL_6);
	#endif
	
	// This configures the kernels context switch mechanism
	// That includes configuring the new program stack
	kernel_scheduler_start();
}

//--------------------------------------------------------------------------------------------------//

void kernel_thread_config(void)
{
	// Interrupt should already be enabled
	interrupt_global_disable();
	
	// Add the idle thread on priority level 7 (lowest)
	kernel_add_thread("Idle", kernel_idle_thread, NULL, THREAD_LEVEL_7, KERNEL_IDLE_THREAD_STACK_SIZE);
}

//--------------------------------------------------------------------------------------------------//

// There will be an SVC for this function

void kernel_thread_yield(void)
{
	// Not sure is this is a good idea, since it might fuck up the timing
	systick_set_counter_value(0);
	
	// Since the scheduling algorithm is called in the SysTick exception rather than
	// in the PendSV exception we pend the SysTick handler instead of the PendSV handler. 
	SCB->ICSR |= (1 << SCB_ICSR_PENDSTSET_Pos);
}

//--------------------------------------------------------------------------------------------------//

// There will be an SVC for this function

void kernel_thread_delay(uint32_t ticks)
{
	// Suspend the scheduler since we do not want
	// a context switch inside here
	kernel_suspend_scheduler();
	
	// Calculate the right tick to wake
	uint32_t tmp = kernel_tick + ticks;
	
	// Write the value to the thread control block
	kernel_current_thread_pointer->tick_to_wake = tmp;
	
	// Update the next list to place the thread in
	kernel_current_thread_pointer->next_list = &delay_queue;
	
	// Let the scheduler start again
	kernel_resume_scheduler();
	
	// Free the resources used
	kernel_thread_yield();
}

//--------------------------------------------------------------------------------------------------//

// This is the kernels scheduler which decide what thread to run next
// The next thread to run should be placed in the kernel_current_thread_pointer
// variable

void kernel_scheduler(void)
{
	// Check the stack usage
	// Check the dynamic memory usage
	
	
	// Check the integrity of the scheduler
	volatile kernel_thread_control* thread_pointer_check = kernel_current_thread_pointer;
	
	kernel_current_thread_pointer->runtime++;
	
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
					// Put the element into the delay queue
					kernel_list_remove_last(&running_queue);
					kernel_list_insert_delay(&(kernel_current_thread_pointer->list_item), &delay_queue);
					#if KERNEL_DEBUG
					board_serial_print("DEL ADD %d\n",  delay_queue.first->thread_control->tick_to_wake);
					#endif
					
					// Update the kernel tick to wake
					kernel_tick_to_wake = delay_queue.first->thread_control->tick_to_wake;
				}
				
				kernel_current_thread_pointer->next_list = NULL;
			}
			else
			{
				// The thread should just be place first in the running queue
				kernel_list_remove_last(&running_queue);
				kernel_list_insert_first(&(kernel_current_thread_pointer->list_item), &running_queue);
			}
		}
		
		// Now we check if some delays has expired
		if (kernel_tick_to_wake <= kernel_tick)
		{
			kernel_list_item* list_iterator = delay_queue.first;
			
			check(delay_queue.first->thread_control->tick_to_wake <= kernel_tick_to_wake);
			
			uint16_t  i;
			
			for (i = 0; i < delay_queue.size; i++)
			{
				if (list_iterator->thread_control->tick_to_wake > kernel_tick_to_wake)
				{
					break;
				}
				list_iterator = list_iterator->next;
			}
			
			for (uint16_t k = 0; k < i; k++)
			{
				kernel_list_item* tmp = delay_queue.first;
				
				kernel_list_remove_first(&delay_queue);
				kernel_list_insert_first(tmp, &running_queue);
				#if KERNEL_DEBUG
				board_serial_print("DEL REMOVE %d\n", kernel_tick_to_wake);
				#endif
			}
			
			if (list_iterator == NULL)
			{
				// No threads left in the queue
				kernel_tick_to_wake = 0xffffffff;
			}
			else
			{
				kernel_tick_to_wake = delay_queue.first->thread_control->tick_to_wake;
			}
		}
		
		// Update the next thread to run
		if (running_queue.last == NULL)
		{
			kernel_next_thread_pointer = kernel_idle_thread_pointer;
		}
		else
		{
			kernel_next_thread_pointer = running_queue.last->thread_control;
		}
	}
	else
	{
		
	}
	
	if (kernel_next_thread_pointer->name[0] == 'k')	
	{
		asm volatile ("nop");
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
	kernel_tick++;
	kernel_runtime_tick++;
	
	if (kernel_runtime_tick >= 1000)
	{
		kernel_reset_runtime();
		kernel_runtime_tick = 0;
	}
	
	// Launch the scheduler
	kernel_scheduler();
	
	// Pend the PendSV exception
	SCB->ICSR |= SCB_ICSR_PENDSVSET_Msk;
}

//--------------------------------------------------------------------------------------------------//

void kernel_reset_runtime(void)
{
	kernel_idle_thread_pointer->last_runtime = kernel_idle_thread_pointer->runtime;
	kernel_idle_thread_pointer->runtime = 0;
	
	if (running_queue.size != 0)
	{
		for (kernel_list_item* i = running_queue.first; i != NULL; i = i->next)
		{
			i->thread_control->last_runtime = i->thread_control->runtime;
			i->thread_control->runtime = 0;
		}
	}
	if (delay_queue.size != 0)
	{
		for (kernel_list_item* i = delay_queue.first; i != NULL; i = i->next)
		{
			i->thread_control->last_runtime = i->thread_control->runtime;
			i->thread_control->runtime = 0;
		}
	}
}

//--------------------------------------------------------------------------------------------------//

void kernel_print_runtime_statistics(void)
{
	int32_t cpu_usage = 1000 - kernel_idle_thread_pointer->last_runtime;
	char k = cpu_usage / 10;
	board_serial_x_write_percent(k, cpu_usage - (k * 10));
	board_serial_x_print(" : CPU Usage");
	board_serial_x_print("\n");
	
	if (running_queue.size != 0)
	{
		for (kernel_list_item* i = running_queue.first; i != NULL; i = i->next)
		{
			uint8_t tmp = i->thread_control->last_runtime / 10;
			board_serial_x_write_percent(tmp, i->thread_control->last_runtime - (tmp * 10));
			board_serial_x_print(" : %s", i->thread_control->name);
			board_serial_x_print("\n");
		}
	}
	if (delay_queue.size != 0)
	{
		for (kernel_list_item* i = delay_queue.first; i != NULL; i = i->next)
		{
			uint8_t tmp = i->thread_control->last_runtime / 10;
			board_serial_x_write_percent(tmp, i->thread_control->last_runtime - (tmp * 10));
			board_serial_x_print(" : %s", i->thread_control->name);
			board_serial_x_print("\n");
		}
	}
	board_serial_x_print("\n\n");
}

//--------------------------------------------------------------------------------------------------//

void kernel_idle_thread(void* param)
{
	while (1)
	{
		// The idle thread should not do anything
	}
}

//--------------------------------------------------------------------------------------------------//

void kernel_thread_stack_overflow_event(char* data)
{
	board_serial_print("Warning: Stack overflow on ");
	board_serial_print(data);
	board_serial_print(" thread\n");
}

//--------------------------------------------------------------------------------------------------//

void kernel_suspend_scheduler(void)
{
	(void)scheduler_status;
	scheduler_status = SCHEDULER_STATUS_SUSPENDED;
}

//--------------------------------------------------------------------------------------------------//

void kernel_resume_scheduler(void)
{
	(void)scheduler_status;
	scheduler_status = SCHEDULER_STATUS_RUNNING;
}

//--------------------------------------------------------------------------------------------------//

// Sice we a using a linked list we must asure that every element is unique. Otherwise the
// functionality will be messed up. This simple funciton searched a list for a match. This
// function should be called before inserting a new element.

static uint8_t kernel_list_search(kernel_list_item* list_item, kernel_list* list)
{
	if (list->first == NULL)
	{
		return 0;
	}
	
	kernel_list_item* list_iterator = list->first;
	
	while (list_iterator != NULL)
	{
		// If there is a match return
		if (list_item == list_iterator)
		{
			return 1;
		}
		
		list_iterator = list_iterator->next;
	}
	
	return 0;
}

//--------------------------------------------------------------------------------------------------//

void kernel_list_insert_first(kernel_list_item* list_item, kernel_list* list)
{
	// Check if the size is zero
	if (list->size == 0)
	{
		// Update the global pointers
		list->first = list_item;
		list->last = list_item;
		
		// Update the list item pointer
		list_item->next = NULL;
		list_item->previous = NULL;
	}
	else
	{
		// Search the list to make sure it dont exist
		check(kernel_list_search(list_item, list) == 0);
		
		// Update the list item pointers
		list_item->next = list->first;
		list_item->previous = NULL;
		
		// Link backwards from first node
		check(list->first->previous == NULL);
		list->first->previous = list_item;
		
		// Update the first pointer
		list->first = list_item;
	}
	
	// Increment the size
	list->size++;
}

//--------------------------------------------------------------------------------------------------//

void kernel_list_insert_last(kernel_list_item* list_item, kernel_list* list)
{
	// Check if the size is zero
	if (list->size == 0)
	{
		// Update the global pointers
		list->first = list_item;
		list->last = list_item;
		
		// Update the list item pointer
		list_item->next = NULL;
		list_item->previous = NULL;
	}
	else
	{
		// Search the list to make sure it dont exist
		check(kernel_list_search(list_item, list) == 0);
		
		// Update the list item pointers
		list_item->next = NULL;
		list_item->previous = list->last;
		
		// Link backwards from first node
		check(list->last->next == NULL);
		list->last->next = list_item;
		
		// Update the first pointer
		list->last = list_item;
	}
	
	// Increment the size
	list->size++;
}

//--------------------------------------------------------------------------------------------------//

void kernel_list_insert_delay(kernel_list_item* list_item, kernel_list* list)
{
	// Check if the size if zero
	if (list->size == 0)
	{
		// Update the global pointers
		list->first = list_item;
		list->last = list_item;
		
		// Update the list item pointer
		list_item->next = NULL;
		list_item->previous = NULL;
		
		list->size = 1;
	}
	else
	{
		// Search the list to make sure it dont exist
		check(kernel_list_search(list_item, list) == 0);
		
		// Check the tick value
		uint32_t tick = list_item->thread_control->tick_to_wake;
		
		if (tick <= list->first->thread_control->tick_to_wake)
		{
			// Insert at the beginning
			kernel_list_insert_first(list_item, list);
			
			// Increment handled
		}
		else if (tick >= list->last->thread_control->tick_to_wake)
		{
			// Insert at the end
			kernel_list_insert_last(list_item, list);
			
			// Increment handled
		}
		else
		{
			kernel_list_item* list_iterator = list->first->next;
			
			// Iterate though the list
			while (list_iterator != NULL)
			{
				// Check tick against the following item
				if (tick < list_iterator->thread_control->tick_to_wake)
				{
					// Insert the item behind list_iterator
					kernel_list_item* prev_item = list_iterator->previous;
					
					list_item->next = list_iterator;
					list_iterator->previous = list_item;
					
					prev_item->next = list_item;
					list_item->previous = prev_item;
					
					list->size++;
					
					return;
				}
				
				list_iterator = list_iterator->next;
			}
			
			// Should not rech here
			check(0);
		}
	}
}

//--------------------------------------------------------------------------------------------------//

uint8_t kernel_list_remove_first(kernel_list* list)
{
	if (list->size == 0)
	{
		// Nothing to remove
		return 0;
	}
	else if (list->size == 1)
	{
		// Remove the only element present
		list->first->next = NULL;
		list->first->previous = NULL;
		
		// Update the list first and last element
		list->first = NULL;
		list->last = NULL;
		
		list->size = 0;
	}
	else
	{
		// Remove the first element
		check(list->first != NULL);
		check(list->last != NULL);
		// Update the first pointer
		list->first = list->first->next;
		
		// Remove the forward link from the first element
		list->first->previous->next = NULL;
		
		// Remove the backward link from the new first item
		list->first->previous = NULL;
		
		list->size--;
	}
	
	return 1;
}

//--------------------------------------------------------------------------------------------------//

uint8_t kernel_list_remove_last(kernel_list* list)
{
	if (list->size == 0)
	{
		// Nothing to remove
		return 0;
	}
	else if (list->size == 1)
	{
		// Remove the only element present
		list->first->next = NULL;
		list->first->previous = NULL;
		
		// Update the list first and last element
		list->first = NULL;
		list->last = NULL;
		
		list->size = 0;
	}
	else
	{
		// Remove the last element
		list->last = list->last->previous;
		
		// Remove the forward link from the first element
		list->last->next->previous = NULL;
		
		// Remove the backward link from the new first item
		list->last->next = NULL;
		
		list->size--;
	}
	
	return 1;
}

//--------------------------------------------------------------------------------------------------//

uint8_t kernel_list_remove_item(kernel_list_item* list_item, kernel_list* list)
{
	if (list->size == 0)
	{
		return 0;
	}
	else if (list_item == list->first)
	{
		return kernel_list_remove_first(list);
	}
	else if (list_item == list->last)
	{
		return kernel_list_remove_last(list);
	}
	else
	{
		// The element is not the first and not the last
		// Start at the second element
		kernel_list_item* list_iterator = list->first->next;
		
		while (list_iterator != list->last)
		{
			// Check for a match
			if (list_item == list_iterator)
			{
				// Remove the connections
				list_item->next->previous = list_item->previous;
				list_item->previous->next = list_item->next;
				
				// Update the list_item pointers
				list_item->next = NULL;
				list_item->previous = NULL;
				
				list->size--;
				
				return 1;
			}
			
			list_iterator = list_iterator->next;
		}
		
		return 0;
	}
}

//--------------------------------------------------------------------------------------------------//