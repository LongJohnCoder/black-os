// Copyright (c) 2020 Bjørn Brodtkorb
//
// This software is provided "as is" without warranty of any kind.
// Permission is granted, free of charge, to copy and modify this
// software, if this copyright notice is included in all copies of
// the software.

#ifndef FILE_SYSTEM_H
#define FILE_SYSTEM_H


//--------------------------------------------------------------------------------------------------//


#include "sam.h"
#include "file_system_fat.h"


//--------------------------------------------------------------------------------------------------//


void file_system_config(void);


//--------------------------------------------------------------------------------------------------//


file_result_t file_system_command_ls(void);

file_result_t file_system_command_cd(char* arg);

file_result_t file_system_command_cat(char* arg);

file_result_t file_system_command_run(char* arg);

void file_system_print_directory(void);


//--------------------------------------------------------------------------------------------------//


#endif