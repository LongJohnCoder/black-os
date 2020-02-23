#ifndef FILE_SYSTEM_H
#define FILE_SYSTEM_H

#include "sam.h"
#include "file_system_fat.h"

void file_system_config(void);

file_result_t file_system_command_ls(void);

file_result_t file_system_command_cd(char* arg);

file_result_t file_system_command_cat(char* arg);

file_result_t file_system_command_run(char* arg);

void file_system_print_directory(void);

#endif