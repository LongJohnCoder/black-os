#include "flash.h"
#include "core.h"

//--------------------------------------------------------------------------------------------------//

void flash_set_wait_states(uint8_t wait_states)
{
	uint32_t tmp = EFC->EEFC_FMR;
	
	// Write the number of wait states
	tmp &= ~EEFC_FMR_FWS_Msk;
	tmp |= ((wait_states << EEFC_FMR_FWS_Pos) & EEFC_FMR_FWS_Msk);
	
	// Write register
	CRITICAL_SECTION_ENTER()
	EFC->EEFC_FMR = tmp;
	CRITICAL_SECTION_LEAVE()
}

//--------------------------------------------------------------------------------------------------//