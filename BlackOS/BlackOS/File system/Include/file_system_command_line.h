// Copyright (c) 2020 Bjørn Brodtkorb
//
// This software is provided "as is" without warranty of any kind.
// Permission is granted, free of charge, to copy and modify this
// software, if this copyright notice is included in all copies of
// the software.

#ifndef FILE_SYSTEM_COMMAND_LINE_H
#define FILE_SYSTEM_COMMAND_LINE_H


//--------------------------------------------------------------------------------------------------//


#include "sam.h"
#include "scheduler.h"


//--------------------------------------------------------------------------------------------------//


extern thread_s* file_thread;


//--------------------------------------------------------------------------------------------------//


void file_system_command_line_config(void);

void file_system_command_line_print_directory(void);


//--------------------------------------------------------------------------------------------------//


#endif