#ifndef CHECK_H
#define CHECK_H

#include "sam.h"
#include "config.h"

#if CHECK_ENABLE
#define check(condition) check_handler((condition), __FILE__, __LINE__)
#else
#define check(condition)
#endif

void check_handler(uint8_t condition, const char* filename, uint32_t line_number);

#endif