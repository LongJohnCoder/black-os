#include "board_serial_x.h"
#include "clock.h"
#include "gpio.h"
#include "usart.h"
#include "interrupt.h"

#include <stdio.h>
#include <stdarg.h>

static char serial_print_buffer[256];

static const unsigned s_value[] = {1000000000u, 100000000u, 10000000u, 1000000u, 100000u, 10000u, 1000u, 100u, 10u, 1u};

static void board_serial_x_port_config(void)
{
	gpio_set_pin_function(BOARD_SERIAL_X_TX_PORT, BOARD_SERIAL_X_TX_PIN, PERIPHERAL_FUNCTION_C);
	gpio_set_pin_function(BOARD_SERIAL_X_RX_PORT, BOARD_SERIAL_X_RX_PIN, PERIPHERAL_FUNCTION_C);
}

static void board_serial_x_clock_config(void)
{
	clock_peripheral_clock_enable(ID_USART0);
}

static void board_serial_x_mode_config(void)
{
	usart_mode_config(USART0, USART_STOP_BIT_1, USART_PARITY_NO, USART_MODE_ASYNCHRONOUS, BOARD_SERIAL_X_CD_VALUE);
	
	usart_interrupt_enable(USART0, USART_IRQ_RX_READY);
	interrupt_enable_peripheral_interrupt(USART0_IRQn, IRQ_LEVEL_1);
}

void board_serial_x_config(void)
{
	board_serial_x_port_config();
	board_serial_x_clock_config();
	board_serial_x_mode_config();
}

void board_serial_x_write(char data)
{
	usart_write(USART0, data);
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

void board_serial_x_print(char* data, ...)
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
		usart_write(USART0, *start);
		start++;
	}
}

void board_serial_x_write_percent(char first, char second)
{
	board_serial_x_print("%d.%d", first, second);
	board_serial_x_write('%');
}