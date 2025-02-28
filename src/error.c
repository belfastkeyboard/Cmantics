#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>


#define BUFFSZ 256


void throw(const char *file,
           const char *function,
           int line,
           const char *message,
           ...)
{
    char buffer[BUFFSZ] = { 0 };

    va_list args;

    va_start(args,
             message);

    vsprintf(buffer,
             message,
             args);

    va_end(args);

    fprintf(stderr,
            "%s:%d: %s: %s.\n",
            file,
            line,
            function,
            buffer);

    fflush(stderr);

    exit(EXIT_FAILURE);
}