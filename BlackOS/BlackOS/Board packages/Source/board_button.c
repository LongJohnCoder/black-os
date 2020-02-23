#include "board_button.h"
#include "gpio.h"
#include "interrupt.h"
#include "clock.h"
#include "board_serial.h"
#include "kernel.h"

void board_button_config(void)
{
	clock_peripheral_clock_enable(ID_PIOA);
	
	gpio_set_pin_function(PIOA, 11, PERIPHERAL_FUNCTION_OFF);
	gpio_set_pin_direction_input(PIOA, 11);
	gpio_pullup_enable(PIOA, 11);
	gpio_debounce_filter_enable(PIOA, 11, 10);
	
	gpio_set_interrupt_source(PIOA, 11, GPIO_FALLING_EDGE);
	
	gpio_interrupt_enable(PIOA, 11);
	
	interrupt_enable_peripheral_interrupt(PIOA_IRQn, IRQ_LEVEL_3);
}

