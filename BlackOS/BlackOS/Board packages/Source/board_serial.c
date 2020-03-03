#include "board_serial.h"
#include "usart.h"
#include "clock.h"
#include "gpio.h"
#include "interrupt.h"
#include "dma.h"

#include <stdio.h>
#include <stdarg.h>

static char serial_print_buffer[256];

static const unsigned s_value[] = {1000000000u, 100000000u, 10000000u, 1000000u, 100000u, 10000u, 1000u, 100u, 10u, 1u};
	
static void board_serial_port_config(void)
{
	gpio_set_pin_function(BOARD_SERIAL_TX_PORT, BOARD_SERIAL_TX_PIN, PERIPHERAL_FUNCTION_A);
	gpio_set_pin_function(BOARD_SERIAL_RX_PORT, BOARD_SERIAL_RX_PIN, PERIPHERAL_FUNCTION_D);
}

static void board_serial_clock_config(void)
{
	clock_peripheral_clock_enable(ID_USART1);
}

static void board_serial_mode_config(void)
{
	usart_mode_config(USART1, USART_STOP_BIT_1, USART_PARITY_NO, USART_MODE_ASYNCHRONOUS, BOARD_SERIAL_CD_VALUE);
	usart_interrupt_enable(USART1, USART_IRQ_RX_READY);
	interrupt_enable_peripheral_interrupt(USART1_IRQn, IRQ_LEVEL_6);
}

void board_serial_config(void)
{
	board_serial_port_config();
	board_serial_clock_config();
	board_serial_mode_config();
}

void board_print_buffer(char* buffer, uint32_t size)
{
	dma_microblock_transaction_descriptor dma;
	
	dma.burst_size = DMA_BURST_SIZE_SINGLE;
	dma.chunk_size = DMA_CHUNK_SIZE_1;
	dma.data_width = DMA_DATA_WIDTH_BYTE;
	
	dma.destination_adressing_mode = DMA_DEST_ADDRESSING_FIXED;
	dma.destination_bus_interface = DMA_AHB_INTERFACE_1;
	dma.destination_pointer = (uint32_t *)(&(USART1->US_THR));
	
	dma.memory_fill = DMA_MEMORY_FILL_OFF;
	dma.peripheral_id = XDMAC_CC_PERID_USART1_TX_Val;
	
	dma.size = size;
	
	dma.dma_channel = 6;
	
	dma.source_addressing_mode = DMA_SOURCE_ADDRESSING_INCREMENTED;
	dma.source_bus_inteface = DMA_AHB_INTERFACE_0;
	dma.source_pointer = (uint32_t *)buffer;
	
	dma.synchronization = DMA_SYNC_MEMORY_TO_PERIPHERAL;
	dma.transfer_type = DMA_TRANSFER_TYPE_PERIPHERAL_TRANSFER;
	dma.trigger = DMA_TRIGGER_HARDWARE;
	
	uint32_t* cache_addr = (uint32_t *)((uint32_t)buffer & ~((uint32_t)32));
	
	// Cache can only be cleaned at 32-bytes alignment
	SCB_CleanDCache_by_Addr(cache_addr, size + 32);
	
	dma_setup_transaction(XDMAC, &dma);
}

static int printu_override(char *s, unsigned u)
{
	char tmp_buf[12];
	int  i, n = 0;
	int  m;

	if (u == 0) {
		*s = '0';
		return 1;
	}

	for (i = 0; i < 10; i++) {
		for (m = 0; m < 10; m++) {
			if (u >= s_value[i]) {
				u -= s_value[i];
				} else {
				break;
			}
		}
		tmp_buf[i] = m + '0';
	}
	for (i = 0; i < 10; i++) {
		if (tmp_buf[i] != '0') {
			break;
		}
	}
	for (; i < 10; i++) {
		*s++ = tmp_buf[i];
		n++;
	}
	return n;
}

void board_serial_print(char* data, ...)
{
	char* start = serial_print_buffer;
	char* s = serial_print_buffer;

	int     n = 0;
	va_list ap;
	va_start(ap, data);
	while (*data) {
		if (*data != '%') {
			*s = *data;
			s++;
			data++;
			n++;
			} else {
			data++;
			switch (*data) {
				case 'c': {
					char valch = va_arg(ap, int);
					*s         = valch;
					s++;
					data++;
					n++;
					break;
				}
				case 'd': {
					int vali = va_arg(ap, int);
					int nc;

					if (vali < 0) {
						*s++ = '-';
						n++;
						nc = printu_override(s, -vali);
						} else {
						nc = printu_override(s, vali);
					}

					s += nc;
					n += nc;
					data++;
					break;
				}
				case 'u': {
					unsigned valu = va_arg(ap, unsigned);
					int      nc   = printu_override(s, valu);
					n += nc;
					s += nc;
					data++;
					break;
				}
				case 's': {
					char *vals = va_arg(ap, char *);
					while (*vals) {
						*s = *vals;
						s++;
						vals++;
						n++;
					}
					data++;
					break;
				}
				default:
				*s = *data;
				s++;
				data++;
				n++;
			}
		}
	}
	va_end(ap);
	*s = 0;

	while (*start != '\0')
	{
		usart_write(USART1, *start);
		start++;
	}
}

void board_serial_write(char data)
{
	usart_write(USART1, data);
}

void board_serial_print_address(char* data, uint32_t addr)
{
	board_serial_print(data);
	board_serial_print("0x");
	for (uint8_t i = 0; i < 8; i++)
	{
		uint8_t val = ((addr >> (4 * (7 - i))) & 0b1111);
		
		if (val < 10)
		{
			board_serial_write('0' + val);
		}
		else
		{
			val -= 10;
			board_serial_write('A' + val);
		}
	}
	//board_serial_write('\n');
}

void board_serial_print_percentage_symbol(char* data, uint8_t percent, uint8_t newline)
{
	board_serial_print(data);
	board_serial_print("%d", percent);
	board_serial_write('%');
	if (newline)
	{
		board_serial_write('\n');
	}
}

void board_serial_print_register(char* data, uint32_t reg)
{
	board_serial_print("%s: ", data);
	
	for (uint8_t i = 0; i < 32; i++)
	{
		if ((i != 0) && (i % 8 == 0))
		{
			board_serial_write(' ');
		}
		
		if (reg & (1 << (31 - i)))
		{
			board_serial_write('1');
		}
		else
		{
			board_serial_write('0');
		}
	}
	board_serial_write('\n');
}