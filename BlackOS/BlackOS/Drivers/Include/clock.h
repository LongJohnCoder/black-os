#ifndef CLOCK_H
#define CLOCK_H

#include "sam.h"

typedef enum
{
	CLOCK_SOURCE_CRYSTAL,
	CLOCK_SOURCE_RC
} clock_source;

typedef enum
{
	CLOCK_MASTER_CLOCK_DIVISION_NO_DIV,
	CLOCK_MASTER_CLOCK_DIVISION_DIV_2,
	CLOCK_MASTER_CLOCK_DIVISION_DIV_4,
	CLOCK_MASTER_CLOCK_DIVISION_DIV_3
} clock_master_clock_division;

typedef enum
{
	CLOCK_MASTER_CLOCK_PRESCALER_1,
	CLOCK_MASTER_CLOCK_PRESCALER_2,
	CLOCK_MASTER_CLOCK_PRESCALER_4,
	CLOCK_MASTER_CLOCK_PRESCALER_8,
	CLOCK_MASTER_CLOCK_PRESCALER_16,
	CLOCK_MASTER_CLOCK_PRESCALER_32,
	CLOCK_MASTER_CLOCK_PRESCALER_64,
	CLOCK_MASTER_CLOCK_PRESCALER_3
} clock_master_clock_prescaler;

typedef enum
{
	CLOCK_MASTER_CLOCK_SOURCE_SLOW_CLOCK,
	CLOCK_MASTER_CLOCK_SOURCE_MAIN_CLOCK,
	CLOCK_MASTER_CLOCK_SOURCE_PLLA_CLOCK
} clock_master_clock_source;

typedef enum
{
	CLOCK_SOURCE_FREQUENCY_12_MHZ,
	CLOCK_SOURCE_FREQUENCY_16_MHZ
} clock_source_frequency;

typedef enum
{
	CLOCK_USB_SOURCE_PLL,
	CLOCK_USB_SOURCE_USB_PLL
} clock_usb_source;

typedef enum
{
	PROGRAMMABLE_CLOCK_0,
	PROGRAMMABLE_CLOCK_1,
	PROGRAMMABLE_CLOCK_2,
	PROGRAMMABLE_CLOCK_3,
	PROGRAMMABLE_CLOCK_4,
	PROGRAMMABLE_CLOCK_5,
	PROGRAMMABLE_CLOCK_6,
	PROGRAMMABLE_CLOCK_7
} programmable_clock;




//==================================== S Y S T E M   C L O C K S ====================================//

void clock_sources_config(clock_source clock_oscillator, uint8_t startup_time);

void clock_main_clock_config(clock_source clock_oscillator);

void clock_pll_config(uint16_t multiplication_factor, uint8_t division_factor, uint8_t startup_time);

void clock_master_clock_config(clock_master_clock_source source, clock_master_clock_prescaler prescaler, clock_master_clock_division division);

uint32_t clock_get_cpu_frequency(void);




//======================================= U S B   C L O C K S =======================================//

void clock_usb_pll_config(clock_source_frequency source_frequency, uint8_t startup_time, uint8_t divider);

void clock_usb_config(clock_usb_source usb_source, uint8_t division);

void clock_usb_full_speed_enable(void);

void clock_usb_full_speed_disable(void);




//============================== P R O G R A M M A B L E   C L O C K S ==============================//

void clock_programmable_clocks_enable(programmable_clock clock, clock_master_clock_source source, uint8_t prescaler);

void clock_programmable_clock_disable(programmable_clock clock);





//================================ P E R I P H E R A L   C L O C K S ================================//

void clock_peripheral_clock_enable(uint8_t id_peripheral);

void clock_peripheral_clock_disable(uint8_t id_peripheral);

#endif