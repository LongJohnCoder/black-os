#ifndef FILE_SYSTEM_COMMAND_LINE_H
#define FILE_SYSTEM_COMMAND_LINE_H

#include "sam.h"
#include "kernel_thread.h"

extern kernel_thread_control* file_thread;

void file_system_command_line_config(void);

void file_system_command_line_print_directory(void);

#endif