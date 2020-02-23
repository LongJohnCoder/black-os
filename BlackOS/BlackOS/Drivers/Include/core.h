#ifndef CORE_H
#define CORE_H

#include "sam.h"

//================================= C R I T I C A L   S E C T I O N =================================//

void core_enter_critical_section(uint32_t volatile *atomic);

void core_leave_critical_section(uint32_t volatile *atomic);


#define CRITICAL_SECTION_ENTER()				\
{												\
	volatile uint32_t __atomic;					\
	core_enter_critical_section(&__atomic);

	#define CRITICAL_SECTION_LEAVE()			\
	core_leave_critical_section(&__atomic);		\
}

#endif