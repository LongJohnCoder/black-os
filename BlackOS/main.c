#include "kernel.h"
#include "board_led.h"

#include <stddef.h>


int main(void)
{
	kernel_startup();

	kernel_add_thread("blink", blink_thread, NULL, THREAD_PRIORITY_NORMAL, 200);

	kernel_start();
	
	while (1);
}