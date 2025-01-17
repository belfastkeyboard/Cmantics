#pragma once

#include <errno.h>


__attribute__((noreturn))
__attribute__((format(printf, 4, 5)))
void throw(const char *file,
           const char *function,
           int line,
           const char *message,
           ...);
