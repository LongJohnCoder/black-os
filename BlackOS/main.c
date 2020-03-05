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
#include "dynamic_memory.h"
#include "board_led.h"

#include <stddef.h>


int main(void)
{
	kernel_startup();

	kernel_add_thread("blink", blink_thread, NULL, THREAD_LEVEL_6, 200);

	
	kernel_start();
	
	while (1);
}