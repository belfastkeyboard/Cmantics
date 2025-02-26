#pragma once

#include <stddef.h>
#include "arena.h"
#include "pair.h"


typedef struct Object
{
    size_t size;
    struct Pair **pairs;
} Object;


Object *make_object(char *data,
                    size_t *offset);
