#ifndef CONFIG_H
#define CONFIG_H

#include "sam.h"
#include "board_serial.h"

//============================================ F L A S H ============================================//

#define FLASH_NUMBER_OF_WAIT_STATES			10			// This must be set according to the processor frequency




//============================================= H E A P =============================================//

#define DYNAMIC_MEMORY_ALIGN				8			// This defined the heap alignment in number of bytes
#define DYNAMIC_MEMORY_MINIMUM_BLOCK_SIZE	8			// This defines the minimum allowed block size in bytes

#define DYNAMIC_MEMORY_FILL					1			// This specifies if the dynamic memory should be initialized
#define DYNAMIC_MEMORY_FILL_VALUE			0x00		// Fill value if the dynamic memory should be initialized

#define DYNAMIC_MEMORY_SECTION_NAME_SIZE	32			// Max name length for a memory section



//============================================ C L O C K ============================================//

#define CPU_FREQUENCY						300000000

#define CLOCK_CRYSTAL_STARTUP_TIME			62			// This is the usual crystal start up time

#define CLOCK_PLLA_MULTIPLIER				25			// PLL running at 300MHz
#define CLOCK_PLLA_DIVISION					1
#define CLOCK_PLLA_STARTUP_TIME				0b111111

#define CLOCK_USB_PLL_STARTUP_TIME			0b1111
#define CLOCK_USB_FULL_SPEED_DIVIDER		5




//========================================== S Y S T I C K ==========================================//

#define SYSTICK_FREQUENCY					CPU_FREQUENCY




//============================================ C H E C K ============================================//

#define CHECK_ENABLE						1			// Must be 1 to enable the check attribute
#define CHECK_ENABLE_BREAKPOINT				1			// Controls whether check triggers a breakpoint
#define CHECK_ENABLE_PRINT_HANDLER			1			// Controls whether check prints line number and file name

// The print handler must have the format print_function(char*, ...)

#define CHECK_PRINT_HANDLER					board_serial_print	// Function pointer to a print function




//=========================================== K E R N E L ===========================================//

#define KERNEL_THREAD_MAX_NAME_LENGTH		32			// Defines the maximum allowed thread name length

#define KERNEL_THREAD_xPSR					0x01000000	// xPSR register of the threads
#define KERNEL_THREAD_LR					0x00000000	// Link register of the threads

#define KERNEL_IDLE_THREAD_STACK_SIZE		100			// Specifies the stack size allocated for the idle thread

#define KERNEL_TICK_FREQUENCY				1000		// Frequency of the kernel interrupt





#define DMA_INTERRUPT_PRIORITY				IRQ_LEVEL_3

#endif