#pragma once

#include <stddef.h>
#include "arena.h"
#include "pair.h"


typedef struct Object
{
    size_t pair_size;
    struct Pair *kv_pairs;
} Object;


Object *make_object(Arena *arena,
                    char *data,
                    size_t *offset);
