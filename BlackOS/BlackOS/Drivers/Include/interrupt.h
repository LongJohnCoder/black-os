#ifndef INTERRUPT_H
#define INTERRUPT_H

#include "sam.h"

typedef enum
{
	IRQ_LEVEL_0,
	IRQ_LEVEL_1,
	IRQ_LEVEL_2,
	IRQ_LEVEL_3,
	IRQ_LEVEL_4,
	IRQ_LEVEL_5,
	IRQ_LEVEL_6,
	IRQ_LEVEL_7
} interrupt_priority;

void interrupt_enable_peripheral_interrupt(IRQn_Type irq_type, interrupt_priority irq_priority);

void interrupt_disable_peripheral_interrupt(IRQn_Type irq_type);

void interrupt_global_enable(void);

void interrupt_global_disable(void);

void interrupt_set_pending(IRQn_Type irq_type);

#endif