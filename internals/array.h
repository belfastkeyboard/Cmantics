#pragma once

#include "../internals/arena.h"
#include "../json.h"

typedef struct Array
{
    size_t size;
    Value **values;
} Array;


Array *make_array(Arena *arena,
                  char *data,
                  size_t *offset);
