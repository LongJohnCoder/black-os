#ifndef FILE_SYSTEM_CLI_H
#define FILE_SYSTEM_CLI_H

#include "sam.h"

// Every message sent to this function should be terminated with the null character
void cli_input_decode(char* data);

#endif