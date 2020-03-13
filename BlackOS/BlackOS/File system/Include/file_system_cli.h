// Copyright (c) 2020 Bjørn Brodtkorb
//
// This software is provided "as is" without warranty of any kind.
// Permission is granted, free of charge, to copy and modify this
// software, if this copyright notice is included in all copies of
// the software.

#ifndef FILE_SYSTEM_CLI_H
#define FILE_SYSTEM_CLI_H


//--------------------------------------------------------------------------------------------------//


#include "sam.h"


//--------------------------------------------------------------------------------------------------//


// Every message sent to this function should be terminated with the null character
void cli_input_decode(char* data);


//--------------------------------------------------------------------------------------------------//


#endif