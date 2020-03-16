// Copyright (c) 2020 Bjørn Brodtkorb
//
// This software is provided "as is" without warranty of any kind.
// Permission is granted, free of charge, to copy and modify this
// software, if this copyright notice is included in all copies of
// the software.

#include "graphics_driver.h"
#include "gpio.h"
#include "clock.h"
#include "spi.h"
#include "systick.h"
#include "check.h"


//--------------------------------------------------------------------------------------------------//


static void graphic_driver_pin_config (void)
{
	// Configure SPI pins
	gpio_set_pin_function(GRAPHICS_DRIVER_CS_PORT, GRAPHICS_DRIVER_CS_PIN, PERIPHERAL_FUNCTION_B);
	gpio_set_pin_function(GRAPHICS_DRIVER_MOSI_PORT, GRAPHICS_DRIVER_MOSI_PIN, PERIPHERAL_FUNCTION_B);
	gpio_set_pin_function(GRAPHICS_DRIVER_SCK_PORT, GRAPHICS_DRIVER_SCK_PIN, PERIPHERAL_FUNCTION_B);

	// Configure D/C and reset pins
	gpio_set_pin_function(GRAPHICS_DRIVER_RESET_PORT, GRAPHICS_DRIVER_RESET_PIN, PERIPHERAL_FUNCTION_OFF);
	gpio_set_pin_function(GRAPHICS_DRIVER_D_C_PORT, GRAPHICS_DRIVER_D_C_PIN, PERIPHERAL_FUNCTION_OFF);
	
	// Set D/C and reset pins as output
	gpio_set_pin_direction_output(GRAPHICS_DRIVER_RESET_PORT, GRAPHICS_DRIVER_RESET_PIN);
	gpio_set_pin_direction_output(GRAPHICS_DRIVER_D_C_PORT, GRAPHICS_DRIVER_D_C_PIN);
	
	// No reset at config
	gpio_set_pin_value(GRAPHICS_DRIVER_RESET_PORT, GRAPHICS_DRIVER_RESET_PIN);
	
}


//--------------------------------------------------------------------------------------------------//


void graphics_driver_clock_config()
{
	clock_peripheral_clock_enable(ID_SPI0);
}


//--------------------------------------------------------------------------------------------------//


void graphics_driver_config(void)
{
	
	graphic_driver_pin_config();
	graphics_driver_clock_config();
	
	spi_write_protection_disable(SPI0);
	
	spi_mode_config(SPI0, 0, SPI_FIXED_CHIP_SELECT_1, 0, 0, 1, 0, SPI_FIXED_PERIPHERAL, SPI_MODE_MASTER);
	spi_chip_select_config(SPI0, SPI_CHIP_SELECT_1, 0, 0, 3, SPI_8_BIT, 0, 0, SPI_CLOCK_INACTIVE_LOW, SPI_CLOCK_CAPTURE_ON_LEADING);
	spi_enable(SPI0);
	
	graphics_driver_configuration_sequence();
}


//--------------------------------------------------------------------------------------------------//


void graphics_driver_command(void)
{
	gpio_clear_pin_value(GRAPHICS_DRIVER_D_C_PORT, GRAPHICS_DRIVER_D_C_PIN);
}


//--------------------------------------------------------------------------------------------------//


void graphics_driver_data(void)
{
	gpio_set_pin_value(GRAPHICS_DRIVER_D_C_PORT, GRAPHICS_DRIVER_D_C_PIN);
}


//--------------------------------------------------------------------------------------------------//


void graphics_driver_reset_on(void)
{
	gpio_clear_pin_value(GRAPHICS_DRIVER_RESET_PORT, GRAPHICS_DRIVER_RESET_PIN);
}


//--------------------------------------------------------------------------------------------------//


void graphics_driver_reset_off(void)
{
	gpio_set_pin_value(GRAPHICS_DRIVER_RESET_PORT, GRAPHICS_DRIVER_RESET_PIN);
}


//--------------------------------------------------------------------------------------------------//


void graphics_driver_config_write_data(uint8_t data)
{
	spi_flush(SPI0);
	graphics_driver_data();
	spi_trasmit_data_8_bit(SPI0, data);
}


//--------------------------------------------------------------------------------------------------//


void graphics_driver_config_write_command(uint8_t data)
{
	spi_flush(SPI0);
	graphics_driver_command();
	spi_trasmit_data_8_bit(SPI0, data);
}


//--------------------------------------------------------------------------------------------------//


void graphics_driver_config_write_data_16_bit(uint16_t data)
{
	spi_flush(SPI0);
	graphics_driver_data();	
	spi_trasmit_data_8_bit(SPI0, data >> 8);
	spi_trasmit_data_8_bit(SPI0, data);
}


//--------------------------------------------------------------------------------------------------//


void graphics_driver_set_address_window(uint16_t x_start, uint16_t y_start, uint16_t width, uint16_t height)
{
	uint16_t x_stop = x_start + width - 1;
	uint16_t y_stop = y_start + height - 1;
	
	// Check the parameters
	check(x_stop < GRAPHICS_DRIVER_DISPLAY_WIDTH);
	check(y_stop < GRAPHICS_DRIVER_DISPLAY_HEIGHT);
	
	graphics_driver_config_write_command(ILI9341_CASET);
	
	spi_flush(SPI0);
	graphics_driver_data();
	spi_trasmit_data_8_bit(SPI0, x_start >> 8);
	spi_trasmit_data_8_bit(SPI0, x_start);
	spi_trasmit_data_8_bit(SPI0, x_stop >> 8);
	spi_trasmit_data_8_bit(SPI0, x_stop);
	
	graphics_driver_config_write_command(ILI9341_PASET);
	
	spi_flush(SPI0);
	graphics_driver_data();
	spi_trasmit_data_8_bit(SPI0, y_start >> 8);
	spi_trasmit_data_8_bit(SPI0, y_start);
	spi_trasmit_data_8_bit(SPI0, y_stop >> 8);
	spi_trasmit_data_8_bit(SPI0, y_stop);
	
	graphics_driver_config_write_command(ILI9341_RAMWR);
}


//--------------------------------------------------------------------------------------------------//


void graphics_driver_draw_framebuffer(uint16_t* framebuffer)
{
	graphics_driver_set_address_window(0, 0, GRAPHICS_DRIVER_DISPLAY_HEIGHT, GRAPHICS_DRIVER_DISPLAY_WIDTH);
	
	spi_flush(SPI0);
	graphics_driver_data();

	for (uint32_t i = 0; i < GRAPHICS_DRIVER_DISPLAY_WIDTH * GRAPHICS_DRIVER_DISPLAY_HEIGHT; i++)
	{
		spi_trasmit_data_8_bit(SPI0, *framebuffer >> 8);
		spi_trasmit_data_8_bit(SPI0, *framebuffer);
		
		framebuffer++;
	}
}


//--------------------------------------------------------------------------------------------------//


void graphics_driver_configuration_sequence()
{
	// Perform hardware reset
	graphics_driver_reset_on();
	for (volatile uint32_t i = 0; i < 5000000; i++)
	{
		asm volatile ("nop");
	}
	graphics_driver_reset_off();
	for (volatile uint32_t i = 0; i < 5000000; i++)
	{
		asm volatile ("nop");
	}
	graphics_driver_config_write_command(ILI9341_SWRESET);
	for (volatile uint32_t i = 0; i < 5000000; i++)
	{
		asm volatile ("nop");
	}

	graphics_driver_config_write_command(0xef);
	graphics_driver_config_write_data(0x03);
	graphics_driver_config_write_data(0x80);
	graphics_driver_config_write_data(0x02);
	
	graphics_driver_config_write_command(0xcf);
	graphics_driver_config_write_data(0x00);
	graphics_driver_config_write_data(0xc1);
	graphics_driver_config_write_data(0x30);
	
	graphics_driver_config_write_command(0xed);
	graphics_driver_config_write_data(0x64);
	graphics_driver_config_write_data(0x03);
	graphics_driver_config_write_data(0x12);
	graphics_driver_config_write_data(0x81);
	
	graphics_driver_config_write_command(0xe8);
	graphics_driver_config_write_data(0x85);
	graphics_driver_config_write_data(0x00);
	graphics_driver_config_write_data(0x78);
	
	graphics_driver_config_write_command(0xcb);
	graphics_driver_config_write_data(0x39);
	graphics_driver_config_write_data(0x2c);
	graphics_driver_config_write_data(0x00);
	graphics_driver_config_write_data(0x34);
	graphics_driver_config_write_data(0x02);
	
	graphics_driver_config_write_command(0xf7);
	graphics_driver_config_write_data(0x20);

	graphics_driver_config_write_command(0xea);
	graphics_driver_config_write_data(0x00);
	graphics_driver_config_write_data(0x00);
	
	
	graphics_driver_config_write_command(ILI9341_PWCTR1);
	graphics_driver_config_write_data(0x23);
	
	graphics_driver_config_write_command(ILI9341_PWCTR2);
	graphics_driver_config_write_data(0x10);
	
	graphics_driver_config_write_command(ILI9341_VMCTR1);
	graphics_driver_config_write_data(0x3e);
	graphics_driver_config_write_data(0x28);
	
	graphics_driver_config_write_command(ILI9341_VMCTR2);
	graphics_driver_config_write_data(0x86);
	
	graphics_driver_config_write_command(ILI9341_MADCTL);
	graphics_driver_config_write_data(MADCTL_MV | MADCTL_MX | MADCTL_BGR);
	
	graphics_driver_config_write_command(ILI9341_VSCRSADD);
	graphics_driver_config_write_data(0x00);
	
	graphics_driver_config_write_command(ILI9341_PIXFMT);
	graphics_driver_config_write_data(0x55);
	
	graphics_driver_config_write_command(ILI9341_FRMCTR1);
	graphics_driver_config_write_data(0x00);
	graphics_driver_config_write_data(0x18);
	
	graphics_driver_config_write_command(ILI9341_DFUNCTR);
	graphics_driver_config_write_data(0x08);
	graphics_driver_config_write_data(0x82);
	graphics_driver_config_write_data(0x27);
	
	graphics_driver_config_write_command(0xf2);
	graphics_driver_config_write_data(0x00);

	
	graphics_driver_config_write_command(ILI9341_GAMMASET);
	graphics_driver_config_write_data(0x01);

	graphics_driver_config_write_command(ILI9341_GMCTRP1);
	graphics_driver_config_write_data(0x0f);
	graphics_driver_config_write_data(0x31);
	graphics_driver_config_write_data(0x2b);
	graphics_driver_config_write_data(0x0c);
	graphics_driver_config_write_data(0x0e);
	graphics_driver_config_write_data(0x08);
	graphics_driver_config_write_data(0x4e);
	graphics_driver_config_write_data(0xf1);
	graphics_driver_config_write_data(0x37);
	graphics_driver_config_write_data(0x07);
	graphics_driver_config_write_data(0x10);
	graphics_driver_config_write_data(0x03);
	graphics_driver_config_write_data(0x0e);
	graphics_driver_config_write_data(0x09);
	graphics_driver_config_write_data(0x00);
	
	graphics_driver_config_write_command(ILI9341_GMCTRN1);
	graphics_driver_config_write_data(0x00);
	graphics_driver_config_write_data(0x0e);
	graphics_driver_config_write_data(0x14);
	graphics_driver_config_write_data(0x03);
	graphics_driver_config_write_data(0x11);
	graphics_driver_config_write_data(0x07);
	graphics_driver_config_write_data(0x31);
	graphics_driver_config_write_data(0xc1);
	graphics_driver_config_write_data(0x48);
	graphics_driver_config_write_data(0x08);
	graphics_driver_config_write_data(0x0f);
	graphics_driver_config_write_data(0x0c);
	graphics_driver_config_write_data(0x31);
	graphics_driver_config_write_data(0x36);
	graphics_driver_config_write_data(0x0f);

	
	graphics_driver_config_write_command(ILI9341_SLPOUT);
	graphics_driver_config_write_data(0x80);
	
	graphics_driver_config_write_command(ILI9341_DISPON);
	graphics_driver_config_write_data(0x80);
	
	
	graphics_driver_set_address_window(0, 0, GRAPHICS_DRIVER_DISPLAY_WIDTH, GRAPHICS_DRIVER_DISPLAY_HEIGHT);
	
	spi_flush(SPI0);
	graphics_driver_data();
	spi_set_bit_size(SPI0, SPI_CHIP_SELECT_1, SPI_16_BIT);
	
	for (uint32_t i = 0; i < GRAPHICS_DRIVER_DISPLAY_HEIGHT*GRAPHICS_DRIVER_DISPLAY_WIDTH; i++)
	{
		spi_trasmit_data_16_bit(SPI0, 0x0000);
	}
	spi_set_bit_size(SPI0, SPI_CHIP_SELECT_1, SPI_8_BIT);	
}


//--------------------------------------------------------------------------------------------------//