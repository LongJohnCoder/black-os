#ifndef GPIO_H
#define GPIO_H

#include "sam.h"

typedef enum {
	PERIPHERAL_FUNCTION_A,
	PERIPHERAL_FUNCTION_B,
	PERIPHERAL_FUNCTION_C,
	PERIPHERAL_FUNCTION_D,
	PERIPHERAL_FUNCTION_OFF
} gpio_peripheral_function;

typedef enum {
	GPIO_RISING_EDGE,
	GPIO_FALLING_EDGE,
	GPIO_HIGH_LEVEL,
	GPIO_LOW_LEVEL,
	GPIO_STANDARD
} gpio_interrupt_source;




//==================================== G P I O   F U N C T I O N ====================================//

void gpio_set_pin_function(Pio* hardware, uint8_t pin, gpio_peripheral_function func);




//=============================== G P I O   P I N   D I R E C T I O N ===============================//

void gpio_set_pin_direction_output(Pio* hardware, uint8_t pin);

void gpio_set_pin_direction_input(Pio* hardware, uint8_t pin);




//=================================== G P I O   P I N   V A L U E ===================================//

void gpio_set_pin_value(Pio* hardware, uint8_t pin);

void gpio_clear_pin_value(Pio* hardware, uint8_t pin);

void gpio_toogle_pin_value(Pio* hardware, uint8_t pin);




//====================================== G P I O   F I L T E R ======================================//

void gpio_glitch_filter_enable(Pio* hardware, uint8_t pin);

void gpio_glitch_filter_disable(Pio* hardware, uint8_t pin);

void gpio_debounce_filter_enable(Pio* hardware, uint8_t pin, float ms);

void gpio_debounce_filter_disable(Pio* hardware, uint8_t pin);




//====================================== G P I O   P U L L U P ======================================//

void gpio_pullup_enable(Pio* hardware, uint8_t pin);

void gpio_pullup_disable(Pio* hardware, uint8_t pin);

void gpio_pulldown_enable(Pio* hardware, uint8_t pin);

void gpio_pulldown_disable(Pio* hardware, uint8_t pin);




//================================== G P I O   P I N   S T A T U S ==================================//

uint32_t gpio_get_pin_value_status_register(Pio* hardware);




//=================================== G P I O   I N T E R R U P T ===================================//

void gpio_interrupt_enable(Pio* hardware, uint8_t pin);

void gpio_interrupt_disable(Pio* hardware, uint8_t pin);

void gpio_set_interrupt_source(Pio* hardware, uint8_t pin, gpio_interrupt_source md);

uint32_t gpio_get_interrupt_status_register(Pio* hardware);

#endif