// Copyright (c) 2020 Bjørn Brodtkorb
//
// This software is provided "as is" without warranty of any kind.
// Permission is granted, free of charge, to copy and modify this
// software, if this copyright notice is included in all copies of
// the software.

#ifndef ATOMIC_H
#define ATOMIC_H


//--------------------------------------------------------------------------------------------------//


#include "sam.h"


//--------------------------------------------------------------------------------------------------//


void atomic_increment(uint32_t* memory);

void atomic_decrement(uint32_t* memory);

void atomic_set(uint32_t* memory, uint32_t value);


//--------------------------------------------------------------------------------------------------//


#endif