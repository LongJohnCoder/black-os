
#ifndef USART_H
#define USART_H

#include "sam.h"

#define USART_IRQ_TX_EMPTY	(1 << US_IER_TXEMPTY_Pos)
#define USART_IRQ_RX_READY	(1 << US_IER_RXRDY_Pos)
#define USART_IRQ_TX_READY	(1 << US_IER_TXRDY_Pos)

typedef enum
{
	USART_STOP_BIT_1,
	USART_STOP_BIT_1_5,
	USART_STOP_BIT_2
} usart_stop_bits;

typedef enum
{
	USART_PARITY_EVEN,
	USART_PARITY_ODD,
	USART_PARITY_SPACE,
	USART_PARITY_MARK,
	USART_PARITY_NO,
	USART_PARITY_MULTIDROP
} usart_parity;

typedef enum
{
	USART_MODE_ASYNCHRONOUS,
	USART_MODE_SYNCHRONOUS
} usart_synchronous_mode;

void usart_mode_config(Usart* hardware, usart_stop_bits sp, usart_parity pb, usart_synchronous_mode sm, uint16_t cd_value);

void usart_interrupt_enable(Usart* hardware, uint32_t interrupt_mask);

void usart_interrupt_disable(Usart* hardware, uint32_t interrupt_mask);

void usart_write(Usart* hardware, char data);

char usart_read(Usart* hardware);

uint32_t usart_get_status_register(Usart* hardware);

#endif