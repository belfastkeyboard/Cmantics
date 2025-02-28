#pragma once

#include <stddef.h>
#include "arena.h"
#include "dict.h"
#include "pair.h"


typedef struct Object {
    struct Dict *dict;
} Object;


Object *make_object(Array *meta,
                    Arena *arena);


Object *parse_object(char *data,
                     size_t *offset,
                     Array *meta,
                     Arena *arena);
