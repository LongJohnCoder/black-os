#include "sam.h"
#include "kernel.h"
#include "gpio.h"
#include "config.h"
#include "usb_host_core.h"
#include "systick.h"
#include "graphics_driver.h"
#include "board_sd_card.h"
#include "file_system_fat.h"
#include "file_system_io.h"
#include "usart.h"
#include "interrupt.h"
#include "kernel_thread.h"
#include "file_system_command_line.h"
#include "dram.h"
#include "board_serial_x.h"
#include "dma.h"
#include "text.h"

#include <stddef.h>
volatile uint8_t button_pressed = 0;

void blink(void* arg)
{
	
	
	gpio_set_pin_function(PIOC, 8, PERIPHERAL_FUNCTION_OFF);
	gpio_set_pin_direction_output(PIOC, 8);
	
	uint8_t pin_value;
	
	while (1)
	{
		service_get_pin_value(PIOA, 11, &pin_value);
		if (pin_value)
		{
			gpio_clear_pin_value(PIOC, 8);
			service_thread_delay(30);
			gpio_set_pin_value(PIOC, 8);
			service_thread_delay(970);
		}
	}
}


void stats(void* args)
{
	while(1)
	{
		
		kernel_print_runtime_statistics();
		service_thread_delay(1000);
	}
}


int main(void)
{
	kernel_startup();
	
	board_serial_x_config();

	kernel_add_thread("blink", blink, NULL, THREAD_LEVEL_6, 200);
	kernel_add_thread("fuck", stats, NULL, THREAD_LEVEL_6, 200);
	
	kernel_start();

	
	while (1)
	{

	}
}

void PIOA_Handler()
{
	(void)gpio_get_interrupt_status_register(PIOA);
	
	if (file_thread->current_list == &delay_queue)
	{
		board_serial_print("File in delay list\n");
	}
	else if (file_thread->current_list == &running_queue)
	{
		board_serial_print("File in running list\n");
	}
	else
	{
		board_serial_print("WARNING\n\n");
	}
	
	
	board_serial_print("Button pressed\n");
	
	
	
	button_pressed = 1;
}

