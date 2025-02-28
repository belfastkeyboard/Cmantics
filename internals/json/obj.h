#pragma once

#include "../array.h"
#include "../dict.h"


typedef struct Object {
    struct Dict *dict;
} Object;


Object *make_object(Array *meta,
                    Arena *arena);
