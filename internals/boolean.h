#pragma once

#include <stdbool.h>
#include "arena.h"


typedef struct Boolean
{
    bool boolean;
    char *string;
} Boolean;

Boolean make_boolean(bool boolean,
                     const char *string,
                     Arena *arena);
