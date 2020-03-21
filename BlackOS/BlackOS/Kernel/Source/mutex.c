// Copyright (c) 2020 Bjørn Brodtkorb
//
// This software is provided "as is" without warranty of any kind.
// Permission is granted, free of charge, to copy and modify this
// software, if this copyright notice is included in all copies of
// the software.

#include "mutex.h"
#include "scheduler.h"
#include "atomic.h"


//--------------------------------------------------------------------------------------------------//


void mutex_lock(volatile mutex_s *mutex)
{
	uint32_t status = 0;
	
	do {
		while (__LDREXW(&(mutex->lock)) != 0)
		{
			reschedule();
		}
		
		status = __STREXW(1, &(mutex->lock));
		
	} while (status != 0);
	
	// Do not start any other memory access until DMB has finished
	__DMB();
	
	return;
}


//--------------------------------------------------------------------------------------------------//


void mutex_unlock(volatile mutex_s *mutex)
{ // Note: __LDREXW and __STREXW are CMSIS functions
	__DMB(); // Ensure memory operations completed before
	// releasing lock
	(mutex->lock) = 0;
	return;
}


//--------------------------------------------------------------------------------------------------//