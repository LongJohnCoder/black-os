// Copyright (c) 2020 Bjørn Brodtkorb
//
// This software is provided without warranty of any kind.
// Permission is granted, free of charge, to copy and modify this
// software, if this copyright notice is included in all copies of
// the software.

#ifndef GRAPHICS_DRIVER_H
#define GRAPHICS_DRIVER_H


//--------------------------------------------------------------------------------------------------//


#include "sam.h"
#include "gpio.h"
#include "spi.h"


//--------------------------------------------------------------------------------------------------//


#define GRAPHICS_INLINE __attribute__((always_inline))


#define GRAPHICS_DRIVER_CS_PIN		25
#define GRAPHICS_DRIVER_CS_PORT		PIOD


#define GRAPHICS_DRIVER_MOSI_PIN	21
#define GRAPHICS_DRIVER_MOSI_PORT	PIOD


#define GRAPHICS_DRIVER_SCK_PIN		22	
#define GRAPHICS_DRIVER_SCK_PORT	PIOD


#define GRAPHICS_DRIVER_RESET_PIN	0
#define GRAPHICS_DRIVER_RESET_PORT	PIOB


#define GRAPHICS_DRIVER_D_C_PIN		1
#define GRAPHICS_DRIVER_D_C_PORT	PIOB


#define ILI9341_TFTWIDTH   240
#define ILI9341_TFTHEIGHT  320


#define ILI9341_NOP        0x00
#define ILI9341_SWRESET    0x01
#define ILI9341_RDDID      0x04
#define ILI9341_RDDST      0x09


#define ILI9341_SLPIN      0x10
#define ILI9341_SLPOUT     0x11 
#define ILI9341_PTLON      0x12 
#define ILI9341_NORON      0x13 


#define ILI9341_RDMODE     0x0A
#define ILI9341_RDMADCTL   0x0B
#define ILI9341_RDPIXFMT   0x0C 
#define ILI9341_RDIMGFMT   0x0D
#define ILI9341_RDSELFDIAG 0x0F


#define ILI9341_INVOFF     0x20 
#define ILI9341_INVON      0x21 
#define ILI9341_GAMMASET   0x26 
#define ILI9341_DISPOFF    0x28
#define ILI9341_DISPON     0x29


#define ILI9341_CASET      0x2A
#define ILI9341_PASET      0x2B
#define ILI9341_RAMWR      0x2C
#define ILI9341_RAMRD      0x2E


#define ILI9341_PTLAR      0x30
#define ILI9341_MADCTL     0x36
#define ILI9341_VSCRSADD   0x37
#define ILI9341_VSCRDEF    0x33
#define ILI9341_PIXFMT     0x3A


#define ILI9341_FRMCTR1    0xB1
#define ILI9341_FRMCTR2    0xB2
#define ILI9341_FRMCTR3    0xB3
#define ILI9341_INVCTR     0xB4
#define ILI9341_DFUNCTR    0xB6


#define ILI9341_PWCTR1     0xC0
#define ILI9341_PWCTR2     0xC1
#define ILI9341_PWCTR3     0xC2
#define ILI9341_PWCTR4     0xC3
#define ILI9341_PWCTR5     0xC4
#define ILI9341_VMCTR1     0xC5
#define ILI9341_VMCTR2     0xC7


#define ILI9341_RDID1      0xDA
#define ILI9341_RDID2      0xDB
#define ILI9341_RDID3      0xDC
#define ILI9341_RDID4      0xDD


#define ILI9341_GMCTRP1    0xE0
#define ILI9341_GMCTRN1    0xE1


#define ILI9341_BLACK       0x0000
#define ILI9341_NAVY        0x000F
#define ILI9341_DARKGREEN   0x03E0
#define ILI9341_DARKCYAN    0x03EF
#define ILI9341_MAROON      0x7800
#define ILI9341_PURPLE      0x780F
#define ILI9341_OLIVE       0x7BE0
#define ILI9341_LIGHTGREY   0xC618
#define ILI9341_DARKGREY    0x7BEF
#define ILI9341_BLUE        0x001F
#define ILI9341_GREEN       0x07E0
#define ILI9341_CYAN        0x07FF
#define ILI9341_RED         0xF800
#define ILI9341_MAGENTA     0xF81F
#define ILI9341_YELLOW      0xFFE0
#define ILI9341_WHITE       0xFFFF
#define ILI9341_ORANGE      0xFD20
#define ILI9341_GREENYELLOW 0xAFE5
#define ILI9341_PINK        0xFC18


#define GRAY				0x73AE
#define DARK_GREY			0x4228


#define MADCTL_MY  0x80
#define MADCTL_MX  0x40
#define MADCTL_MV  0x20
#define MADCTL_ML  0x10
#define MADCTL_RGB 0x00 
#define MADCTL_BGR 0x08
#define MADCTL_MH  0x04


#define GRAPHICS_DRIVER_DISPLAY_WIDTH		320
#define GRAPHICS_DRIVER_DISPLAY_HEIGHT		240


//--------------------------------------------------------------------------------------------------//


void graphics_driver_config(void);


//--------------------------------------------------------------------------------------------------//


void graphics_driver_config_write_data(uint8_t data);

void graphics_driver_config_write_command(uint8_t data);

void graphics_driver_set_address_window(uint16_t x_start, uint16_t y_start, uint16_t width, uint16_t height);

void graphics_driver_configuration_sequence();

void graphics_driver_config_write_data_16_bit(uint16_t data);

void graphics_driver_draw_framebuffer(uint16_t* framebuffer);

 void graphics_driver_data(void);


//--------------------------------------------------------------------------------------------------//


void graphics_update_cursor(uint16_t x, uint16_t y);


//--------------------------------------------------------------------------------------------------//


#endif